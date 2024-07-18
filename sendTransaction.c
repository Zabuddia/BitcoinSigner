#include "sendTransaction.h"
#include "lib/all_libs.h"
#include "globals.h"
#include "config.h"
#include "bitcoin/ecc/privatekey.h"
#include "bitcoin/tx/tx.h"
#include "bitcoin/helper/helper.h"

enum send_transaction_state {
    SEND_TRANSACTION_WAITING,
    SEND_TRANSACTION_GETKEY_COMPRESS_YES,
    SEND_TRANSACTION_GETKEY_COMPRESS_NO,
    SEND_TRANSACTION_GETKEY_TESTNET_YES,
    SEND_TRANSACTION_GETKEY_TESTNET_NO,
    SEND_TRANSACTION_GETKEY,
    SEND_TRANSACTION_GETKEY_CONFIRM,
    SEND_TRANSACTION_GETUTXOS,
    SEND_TRANSACTION_DISPLAY_UTXO,
    SEND_TRANSACTION_NEXT_UTXO,
    SEND_TRANSACTION_PREVIOUS_UTXO,
    SEND_TRANSACTION_TOGGLE_UTXO,
    SEND_TRANSACTION_UTXO_CONFIRM,
    SEND_TRANSACTION_GETADDR,
    SEND_TRANSACTION_ADDR_CONFIRM,
    SEND_TRANSACTION_GETAMOUNT,
    SEND_TRANSACTION_AMOUNT_CONFIRM,
    SEND_TRANSACTION_GETFEE,
    SEND_TRANSACTION_FEE_CONFIRM,
    SEND_TRANSACTION_FETCHING,
    SEND_TRANSACTION_CONFIRM,
    SEND_TRANSACTION_BROADCAST,
    SEND_TRANSACTION_DISPLAYING,
    SEND_TRANSACTION_TERMINATED
} send_transaction_state;

static char private_key[100];
static PrivateKey* key;
static char public_key[100];
static char target_address[100];
static uint64_t amount;
static uint64_t txFee;
static uint64_t change;
static char** txids;
static int32_t* vouts;
static int32_t num_utxos;
static int32_t* utxo_indexes;
static int32_t num_utxo_indexes;
static uint64_t* utxo_balances;
static bool compressed;
static bool testnet;
static int32_t current_utxo_index;
static char tx_hex[10000];
static bool no_utxos;
static float estimated_transaction_size;

static void display_compress_yes() {
    no_utxos = false;
    compressed = true;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want compressed format?", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "YES", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "NO", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_compress_no() {
    compressed = false;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want compressed format?", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "YES", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "NO", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
}

static void display_testnet_yes() {
    testnet = true;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want testnet?", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "YES", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "NO", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_testnet_no() {
    testnet = false;
    display_draw_string(STARTING_X, STARTING_Y, "Do you want testnet?", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "YES", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "NO", DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
}

static void display_getkey() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the private key to send the transaction with.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    keyboard_input(private_key);
    mpz_t secret_num;
    hash_to_mpz_t((const uint8_t*)private_key, strlen(private_key), secret_num);
    key = PrivateKey_init(secret_num);
    S256Point_address(key->point, (uint8_t*)public_key, compressed, testnet);
}

static void display_getkey_confirm() {
    display_draw_string(STARTING_X, STARTING_Y, "Private key: ", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_SMALL_FONT, private_key, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_SMALL_FONT * 2, "Public key: ", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_SMALL_FONT * 3, public_key, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 3, "Press the center or right button to confirm.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 5, "Press the left button to re-enter the private key.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static bool is_duplicate(int32_t* array, int32_t size, int32_t value) {
    for (int32_t i = 0; i < size; i++) {
        if (array[i] == value) {
            return true;
        }
    }
    return false;
}

static void format_utxo_info(char *str, int i, const char *txid, long balance) {
    char temp[256];
    sprintf(temp, "%d.", i);
    strcat(str, temp);
    sprintf(temp, "%s:", txid);
    strcat(str, temp);
    sprintf(temp, "%ld", balance);
    strcat(str, temp);
}

static void display_getutxos() {
    display_draw_string(STARTING_X, STARTING_Y, "Fetching UTXOs...", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    char utxo_response[10000] = {0};
    get_utxos(public_key, utxo_response);
    extract_all_utxo_info(utxo_response, &txids, &vouts, &num_utxos);
    if (num_utxos == 0) {
        display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "No UTXOs found. Try again.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
        no_utxos = true;
        return;
    }
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "Press the center button to continue.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "Press the center button to select/deselect UTXO", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 4, "Press the key 1 button when done looking at UTXOs", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    current_utxo_index = 0;
    utxo_indexes = (int32_t*)malloc(num_utxos * sizeof(int32_t));
    for (int32_t i = 0; i < num_utxos; i++) {
        utxo_indexes[i] = -1;
    }
    utxo_balances = (uint64_t*)malloc(num_utxos * sizeof(uint64_t));
    for (int32_t i = 0; i < num_utxos; i++) {
        utxo_balances[i] = get_utxo_balance(txids[i], public_key);
    }
    display_draw_string(STARTING_X, STARTING_Y, "Finished fetching UTXOs", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    while (!center_button_pressed()) ;
    // char str[10000] = {0};
    // for (int32_t i = 0; i < num_utxos; i++) {
    //     uint64_t balance = get_utxo_balance(txids[i], public_key);
    //     format_utxo_info(str, i, txids[i], balance);
    //     strcat(str, "     ");
    //     printf("Txid: %s\n", txids[i]);
    //     printf("Vout: %d\n", vouts[i]);
    // }
    // display_draw_string(STARTING_X, STARTING_Y, str, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    // utxo_indexes = (int32_t*)malloc(num_utxos * sizeof(int32_t));
    // printf("Enter the number of utxos you would like to use: \n");
    // scanf("%d", &num_utxo_indexes);
    // for (int32_t i = 0; i < num_utxo_indexes; i++) {
    //     printf("Enter the index of the utxo you would like to use: \n");
    //     scanf("%d", &utxo_indexes[i]);
    //     while (is_duplicate(utxo_indexes, i, utxo_indexes[i])) {
    //         printf("You have already selected that utxo. Please select a different one.\n");
    //         scanf("%d", &utxo_indexes[i]);
    //     }
    // }
    // printf("UTXO indexes: ");
    // for (int32_t i = 0; i < num_utxo_indexes; i++) {
    //     printf("%d ", utxo_indexes[i]);
    // }
    // printf("\n");
}

static void display_utxo() {
    char str[10000] = {0};
    uint64_t balance = utxo_balances[current_utxo_index];
    format_utxo_info(str, current_utxo_index, txids[current_utxo_index], balance);
    if (is_duplicate(utxo_indexes, num_utxos, current_utxo_index)) {
        display_draw_string(STARTING_X, STARTING_Y, str, DEFAULT_FONT, SELECTED_BACKGROUND_COLOR, SELECTED_FONT_COLOR);
    } else {
        display_draw_string(STARTING_X, STARTING_Y, str, DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    }
}

static void next_utxo() {
    current_utxo_index++;
    if (current_utxo_index >= num_utxos) {
        current_utxo_index = 0;
    }
}

static void previous_utxo() {
    current_utxo_index--;
    if (current_utxo_index < 0) {
        current_utxo_index = num_utxos - 1;
    }
}

static void toggle_utxo() {
    if (is_duplicate(utxo_indexes, num_utxos, current_utxo_index)) {
        for (int32_t i = 0; i < num_utxos; i++) {
            if (utxo_indexes[i] == current_utxo_index) {
                utxo_indexes[i] = -1;
                num_utxo_indexes--;
                printf("removed UTXO index\n");
            }
        }
    } else {
        for (int32_t i = 0; i < num_utxos; i++) {
            if (utxo_indexes[i] == -1) {
                utxo_indexes[i] = current_utxo_index;
                num_utxo_indexes++;
                printf("added UTXO index\n");
                break;
            }
        }
    }
}

static void display_utxo_confirm() {
    char str[10000] = {0};
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        format_utxo_info(str, utxo_indexes[i], txids[utxo_indexes[i]], utxo_balances[utxo_indexes[i]]);
        strcat(str, "     ");
    }
    display_draw_string(STARTING_X, STARTING_Y, str, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, SCREEN_HEIGHT - 40, "Press the center or right button to confirm the UTXOs.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, SCREEN_HEIGHT - 20, "Press the left button to re-select the UTXOs.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_getaddr() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the address to send the transaction to.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    keyboard_input(target_address);
}

static void display_addr_confirm() {
    display_draw_string(STARTING_X, STARTING_Y, "Address: ", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, target_address, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "Press the center or right button to confirm.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 3, "Press the left button to re-enter the address.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_getamount() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the amount to send.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    char str[10000] = {0};
    keyboard_input(str);
    amount = atoi(str);
    // scanf("%lu", &amount);
}

static void display_amount_confirm() {
    display_draw_string(STARTING_X, STARTING_Y, "Amount: ", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    char str[100] = {0};
    sprintf(str, "%lu", amount);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, str, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "Press the center or right button to confirm.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 3, "Press the left button to re-enter the amount.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_getfee() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the fee to send the transaction with.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    char str[100] = {0};
    keyboard_input(str);
    txFee = atoi(str);
    // scanf("%lu", &txFee);
}

static void display_fee_confirm() {
    uint64_t total_balance = 0;
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        total_balance += utxo_balances[utxo_indexes[i]];
    }
    change = total_balance - amount - txFee;
    if (change < 0) {
        display_draw_string(STARTING_X, STARTING_Y, "Insufficient funds!", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
        display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, "Press the left button to re-enter the fee.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
        return;
    }
    display_draw_string(STARTING_X, STARTING_Y, "Fee: ", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    estimated_transaction_size = 148 * num_utxo_indexes + 34 * 2 + 10;
    float approximate_sats_per_byte = txFee / estimated_transaction_size;
    char str[10000] = {0};
    sprintf(str, "%lu, ~%.2f sats/byte", txFee, approximate_sats_per_byte);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT, str, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 2, "Change: ", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    sprintf(str, "%lu", change);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 3, str, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 4, "Press the center or right button to confirm.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_DEFAULT_FONT * 5, "Press the left button to re-enter the fee.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

// Function to write the serialized transaction to a file
static void write_tx_to_file(const char *filename, const char *tx_hex) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s\n", tx_hex);
    fclose(file);
}

static void display_fetching() {
    display_draw_string(STARTING_X, STARTING_Y, "Creating Transaction...", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    uint8_t** prev_txs = (uint8_t**)malloc(num_utxo_indexes * sizeof(uint8_t*));
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        prev_txs[i] = (uint8_t*)malloc(32 * sizeof(uint8_t));
        hex_string_to_byte_array(txids[utxo_indexes[i]], prev_txs[i]);
    }
    // for (int32_t i = 0; i < num_utxo_indexes; i++) {
    //     printf("Prev tx: ");
    //     for (int32_t j = 0; j < 32; j++) {
    //         printf("%02x", prev_txs[i][j]);
    //     }
    //     printf("\n");
    // }

    Script** script_sigs = (Script**)malloc(num_utxo_indexes * sizeof(Script*));
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        script_sigs[i] = Script_init();
    }
    TxIn** tx_ins = (TxIn**)malloc(num_utxo_indexes * sizeof(TxIn*));
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        printf("Previous index: %d\n", vouts[utxo_indexes[i]]);
        tx_ins[i] = TxIn_init(prev_txs[i], vouts[utxo_indexes[i]], script_sigs[i], 0xffffffff);
    }
    printf("txin: ");
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        TxIn_toString(tx_ins[i]);
    }
    uint8_t target_h160[20] = {0};
    decode_base58((const char*)target_address, (unsigned char*)target_h160);
    printf("Target h160: ");
    for (int32_t i = 0; i < 20; i++) {
        printf("%02x", target_h160[i]);
    }
    printf("\n");
    Script* target_script = p2pkh_script(target_h160);
    printf("Target script: ");
    Script_toString(target_script);
    TxOut* tx_out = TxOut_init(amount, target_script);
    printf("Tx out: ");
    TxOut_toString(tx_out);
    uint8_t change_h160[20] = {0};
    decode_base58((const char*)public_key, (unsigned char*)change_h160);
    printf("Change h160: ");
    for (int32_t i = 0; i < 20; i++) {
        printf("%02x", change_h160[i]);
    }
    printf("\n");
    Script* change_script = p2pkh_script(change_h160);
    printf("Change script: ");
    Script_toString(change_script);
    TxOut* change_tx_out = TxOut_init(change, change_script);
    printf("Change tx out: ");
    TxOut_toString(change_tx_out);
    TxOut* outputs[2] = {tx_out, change_tx_out};
    Tx* tx = Tx_init(1, num_utxo_indexes, tx_ins, 2, outputs, 0, false, false);
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        sign_input(tx, utxo_indexes[i], key);
    }
    uint8_t tx_serialized[10000] = {0};
    Tx_serialize(tx, tx_serialized);
    printf("Tx serialized: ");
    for (int32_t i = 0; i < Tx_length(tx); i++) {
        printf("%02x", tx_serialized[i]);
    }
    printf("\n");
    display_clear(BACKGROUND_COLOR);
    display_draw_string(STARTING_X, STARTING_Y, "Transaction created! Writing and verifying transaction...", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    byte_array_to_hex_string(tx_serialized, Tx_length(tx), tx_hex);
    write_tx_to_file("tx.txt", tx_hex);
    printf("Verified?: %d\n", Tx_verify(tx));
    display_clear(BACKGROUND_COLOR);
    // Tx_free(tx);
    // for (int32_t i = 0; i < num_utxo_indexes; i++) {
    //     free(prev_txs[i]);
    // }
    // free(prev_txs);
    // for (int32_t i = 0; i < num_utxo_indexes; i++) {
    //     Script_free(script_sigs[i]);
    // }
    // free(script_sigs);
}

static void display_transaction_confirm() {
    display_draw_string(STARTING_X, STARTING_Y, "To: ", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_SMALL_FONT, target_address, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    char str[10000] = {0};
    sprintf(str, "Amount: %lu", amount);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_SMALL_FONT * 2, str, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    sprintf(str, "Fee: %lu", txFee);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_SMALL_FONT * 3, str, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    sprintf(str, "Change: %lu", change);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_SMALL_FONT * 4, str, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_SMALL_FONT * 5, "Press the center or right button to confirm the transaction.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    display_draw_string(STARTING_X, STARTING_Y + SPACE_BETWEEN_SMALL_FONT * 6, "Press the left button to terminate transaction.", SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

static void display_transaction() {
    broadcast_transaction(tx_hex);
    display_draw_string(STARTING_X, STARTING_Y, tx_hex, SMALL_FONT, BACKGROUND_COLOR, FONT_COLOR);
    // PrivateKey_free(key);
    // for (int32_t i = 0; i < num_utxos; i++) {
    //     free(txids[i]);
    // }
    // free(txids);
    // free(vouts);
    // free(utxo_indexes);
    // free(utxo_balances);
}

void send_transaction_init() {
    send_transaction_state = SEND_TRANSACTION_WAITING;
}

void send_transaction_tick() {
    //Transitions
    switch (send_transaction_state) {
        case SEND_TRANSACTION_WAITING:
            if (in_send_transaction) {
                send_transaction_state = SEND_TRANSACTION_GETKEY_COMPRESS_YES;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_GETKEY_COMPRESS_YES:
            if (center_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETKEY_TESTNET_YES;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETKEY_COMPRESS_NO;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_GETKEY_COMPRESS_NO:
            if (center_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETKEY_TESTNET_YES;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETKEY_COMPRESS_YES;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_GETKEY_TESTNET_YES:
            if (center_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETKEY;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETKEY_TESTNET_NO;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_GETKEY_TESTNET_NO:
            if (center_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETKEY;
                display_clear(BACKGROUND_COLOR);
            } else if (down_button_pressed() || up_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETKEY_TESTNET_YES;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_GETKEY:
            send_transaction_state = SEND_TRANSACTION_GETKEY_CONFIRM;
            display_clear(BACKGROUND_COLOR);
            break;
        case SEND_TRANSACTION_GETKEY_CONFIRM:
            if (center_button_pressed() || right_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETUTXOS;
                display_clear(BACKGROUND_COLOR);
            } else if (left_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETKEY;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_GETUTXOS:
            if (center_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_DISPLAY_UTXO;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            } else if (no_utxos) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_DISPLAY_UTXO:
            if (right_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_NEXT_UTXO;
                display_clear(BACKGROUND_COLOR);
            } else if (left_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_PREVIOUS_UTXO;
                display_clear(BACKGROUND_COLOR);
            } else if (key1_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_UTXO_CONFIRM;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            } else if (center_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_TOGGLE_UTXO;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_NEXT_UTXO:
            send_transaction_state = SEND_TRANSACTION_DISPLAY_UTXO;
            break;
        case SEND_TRANSACTION_PREVIOUS_UTXO:
            send_transaction_state = SEND_TRANSACTION_DISPLAY_UTXO;
            break;
        case SEND_TRANSACTION_TOGGLE_UTXO:
            send_transaction_state = SEND_TRANSACTION_DISPLAY_UTXO;
            break;
        case SEND_TRANSACTION_UTXO_CONFIRM:
            if (center_button_pressed() || right_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETADDR;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            } else if (left_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_DISPLAY_UTXO;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_GETADDR:
            send_transaction_state = SEND_TRANSACTION_ADDR_CONFIRM;
            display_clear(BACKGROUND_COLOR);
            break;
        case SEND_TRANSACTION_ADDR_CONFIRM:
            if (center_button_pressed() || right_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETAMOUNT;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            } else if (left_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETADDR;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_GETAMOUNT:
            send_transaction_state = SEND_TRANSACTION_AMOUNT_CONFIRM;
            display_clear(BACKGROUND_COLOR);
            break;
        case SEND_TRANSACTION_AMOUNT_CONFIRM:
            if (center_button_pressed() || right_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETFEE;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            } else if (left_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETAMOUNT;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_GETFEE:
            send_transaction_state = SEND_TRANSACTION_FEE_CONFIRM;
            display_clear(BACKGROUND_COLOR);
            break;
        case SEND_TRANSACTION_FEE_CONFIRM:
            if (center_button_pressed() || right_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_FETCHING;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            } else if (left_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_GETFEE;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_FETCHING:
            send_transaction_state = SEND_TRANSACTION_CONFIRM;
            break;
        case SEND_TRANSACTION_CONFIRM:
            if (center_button_pressed() || right_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_DISPLAYING;
                display_clear(BACKGROUND_COLOR);
            } else if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
                display_clear(BACKGROUND_COLOR);
            } else if (left_button_pressed()) {
                send_transaction_state = SEND_TRANSACTION_TERMINATED;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_BROADCAST:
            send_transaction_state = SEND_TRANSACTION_DISPLAYING;
            break;
        case SEND_TRANSACTION_DISPLAYING:
            if (return_to_menu()) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
            }
            break;
        case SEND_TRANSACTION_TERMINATED:
            send_transaction_state = SEND_TRANSACTION_WAITING;
            break;
    }
    //Actions
    switch (send_transaction_state) {
        case SEND_TRANSACTION_WAITING:
            break;
        case SEND_TRANSACTION_GETKEY_COMPRESS_YES:
            display_compress_yes();
            break;
        case SEND_TRANSACTION_GETKEY_COMPRESS_NO:
            display_compress_no();
            break;
        case SEND_TRANSACTION_GETKEY_TESTNET_YES:
            display_testnet_yes();
            break;
        case SEND_TRANSACTION_GETKEY_TESTNET_NO:
            display_testnet_no();
            break;
        case SEND_TRANSACTION_GETKEY:
            display_getkey();
            break;
        case SEND_TRANSACTION_GETKEY_CONFIRM:
            display_getkey_confirm();
            break;
        case SEND_TRANSACTION_GETUTXOS:
            display_getutxos();
            break;
        case SEND_TRANSACTION_DISPLAY_UTXO:
            display_utxo();
            break;
        case SEND_TRANSACTION_NEXT_UTXO:
            next_utxo();
            break;
        case SEND_TRANSACTION_PREVIOUS_UTXO:
            previous_utxo();
            break;
        case SEND_TRANSACTION_TOGGLE_UTXO:
            toggle_utxo();
            break;
        case SEND_TRANSACTION_UTXO_CONFIRM:
            display_utxo_confirm();
            break;
        case SEND_TRANSACTION_GETADDR:
            display_getaddr();
            break;
        case SEND_TRANSACTION_ADDR_CONFIRM:
            display_addr_confirm();
            break;
        case SEND_TRANSACTION_GETAMOUNT:
            display_getamount();
            break;
        case SEND_TRANSACTION_AMOUNT_CONFIRM:
            display_amount_confirm();
            break;
        case SEND_TRANSACTION_GETFEE:
            display_getfee();
            break;
        case SEND_TRANSACTION_FEE_CONFIRM:
            display_fee_confirm();
            break;
        case SEND_TRANSACTION_FETCHING:
            display_fetching();
            break;
        case SEND_TRANSACTION_CONFIRM:
            display_transaction_confirm();
            break;
        case SEND_TRANSACTION_BROADCAST:
            display_transaction();
            break;
        case SEND_TRANSACTION_DISPLAYING:
            break;
        case SEND_TRANSACTION_TERMINATED:
            break;
    }
}