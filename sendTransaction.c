#include "sendTransaction.h"
#include "lib/all_libs.h"
#include "globals.h"
#include "config.h"
#include "bitcoin/ecc/privatekey.h"
#include "bitcoin/tx/tx.h"
#include "bitcoin/helper/helper.h"

enum send_transaction_state {
    SEND_TRANSACTION_WAITING,
    SEND_TRANSACTION_GETKEY,
    SEND_TRANSACTION_GETUTXOS,
    SEND_TRANSACTION_GETADDR,
    SEND_TRANSACTION_GETAMOUNT,
    SEND_TRANSACTION_GETFEE,
    SEND_TRANSACTION_FETCHING,
    SEND_TRANSACTION_DISPLAYING
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

static void display_getkey() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the private key to send the transaction with.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%s", private_key);
    mpz_t secret_num;
    hash_to_mpz_t((const uint8_t*)private_key, 6, secret_num);
    key = PrivateKey_init(secret_num);
    S256Point_address(key->point, (uint8_t*)public_key, false, false);
    printf("Private key: %s\n", private_key);
    printf("Public key: %s\n", public_key);
}

static bool is_duplicate(int32_t* array, int32_t size, int32_t value) {
    for (int32_t i = 0; i < size; i++) {
        if (array[i] == value) {
            return true;
        }
    }
    return false;

}

static void display_getutxos() {
    char utxo_response[10000] = {0};
    get_utxos(public_key, utxo_response);
    extract_all_utxo_info(utxo_response, &txids, &vouts, &num_utxos);
    for (int32_t i = 0; i < num_utxos; i++) {
        char str[100];
        sprintf(str, "%d. ", i);
        sprintf(str, "%s: ", txids[i]);
        sprintf(str, "%d", get_utxo_balance(txids[i], public_key));
        printf("Txid: %s\n", txids[i]);
        printf("Vout: %d\n", vouts[i]);
        display_draw_string(STARTING_X, STARTING_Y + i * 30, str, DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    }
    display_draw_string(STARTING_X, STARTING_Y + num_utxos * 30, "Enter the indexes of the UTXOs to spend.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    utxo_indexes = (int32_t*)malloc(num_utxos * sizeof(int32_t));
    int32_t index = 0;
    int32_t count = 0;
    while (scanf("%d", &index) == 1) {
        if (is_duplicate(utxo_indexes, num_utxos, index)) {
            printf("Duplicate index!\n");
            continue;
        }
        utxo_indexes[count++] = index;
    }
    num_utxo_indexes = count;
    printf("UTXO indexes: ");
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        printf("%d ", utxo_indexes[i]);
    }
    printf("\n");

}

static void display_getaddr() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the address to send the transaction to.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%s", target_address);
    printf("Target address: %s\n", target_address);
}

static void display_getamount() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the amount to send.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%lu", &amount);
    printf("Amount: %lu\n", amount);
}

static void display_getfee() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the fee to send the transaction with.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%lu", &txFee);
    uint64_t total_balance = 0;
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        total_balance += get_utxo_balance(txids[utxo_indexes[i]], public_key);
    }
    change = total_balance - amount - txFee;
    printf("Fee: %lu\n", txFee);
    printf("Change: %lu\n", change);
    if (change < 0) {
        display_draw_string(STARTING_X, STARTING_Y, "Insufficient funds!", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
        return;
    }
}

static void display_fetching() {
    uint8_t** prev_txs = (uint8_t**)malloc(num_utxo_indexes * sizeof(uint8_t*));
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        prev_txs[i] = (uint8_t*)malloc(32 * sizeof(uint8_t));
        hex_string_to_byte_array(txids[utxo_indexes[i]], prev_txs[i]);
    }
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        printf("Prev tx: ");
        for (int32_t j = 0; j < 32; j++) {
            printf("%02x", prev_txs[i][j]);
        }
        printf("\n");
    }

    Script** script_sigs = (Script**)malloc(num_utxo_indexes * sizeof(Script*));
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        script_sigs[i] = Script_init();
    }
    TxIn** tx_ins = (TxIn**)malloc(num_utxo_indexes * sizeof(TxIn*));
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        tx_ins[i] = TxIn_init(prev_txs[i], vouts[utxo_indexes[i]], script_sigs[i], 0xffffffff);
    }
    uint8_t target_h160[20] = {0};
    decode_base58((uint8_t*)target_address, strlen(target_address), target_h160);
    Script* target_script = p2pkh_script(target_h160);
    TxOut* tx_out = TxOut_init(amount, target_script);
    uint8_t change_h160[20] = {0};
    decode_base58((uint8_t*)public_key, strlen(public_key), change_h160);
    Script* change_script = p2pkh_script(change_h160);
    TxOut* change_tx_out = TxOut_init(change, change_script);
    TxOut* outputs[2] = {tx_out, change_tx_out};
    Tx* tx = Tx_init(1, num_utxo_indexes, tx_ins, 2, outputs, 0, false, false);
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        sign_input(tx, i, key);
    }
    uint8_t tx_serialized[10000] = {0};
    Tx_serialize(tx, tx_serialized);
    char tx_hex[10000] = {0};
    byte_array_to_hex_string(tx_serialized, Tx_length(tx), tx_hex);
    display_draw_string(STARTING_X, STARTING_Y, tx_hex, DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
}

void send_transaction_init() {
    send_transaction_state = SEND_TRANSACTION_WAITING;
}

void send_transaction_tick() {
    //Transitions
    switch (send_transaction_state) {
        case SEND_TRANSACTION_WAITING:
            if (in_send_transaction) {
                send_transaction_state = SEND_TRANSACTION_GETKEY;
                display_clear(BACKGROUND_COLOR);
            }
            break;
        case SEND_TRANSACTION_GETKEY:
            send_transaction_state = SEND_TRANSACTION_GETUTXOS;
            display_clear(BACKGROUND_COLOR);
            break;
        case SEND_TRANSACTION_GETUTXOS:
            send_transaction_state = SEND_TRANSACTION_GETADDR;
            display_clear(BACKGROUND_COLOR);
            break;
        case SEND_TRANSACTION_GETADDR:
            send_transaction_state = SEND_TRANSACTION_GETAMOUNT;
            display_clear(BACKGROUND_COLOR);
            break;
        case SEND_TRANSACTION_GETAMOUNT:
            send_transaction_state = SEND_TRANSACTION_GETFEE;
            display_clear(BACKGROUND_COLOR);
            break;
        case SEND_TRANSACTION_GETFEE:
            send_transaction_state = SEND_TRANSACTION_FETCHING;
            display_clear(BACKGROUND_COLOR);
            break;
        case SEND_TRANSACTION_FETCHING:
            send_transaction_state = SEND_TRANSACTION_DISPLAYING;
            break;
        case SEND_TRANSACTION_DISPLAYING:
            if (button_left() == 0) {
                send_transaction_state = SEND_TRANSACTION_WAITING;
            }
            break;
    }
    //Actions
    switch (send_transaction_state) {
        case SEND_TRANSACTION_WAITING:
            break;
        case SEND_TRANSACTION_GETKEY:
            display_getkey();
            break;
        case SEND_TRANSACTION_GETUTXOS:
            display_getutxos();
            break;
        case SEND_TRANSACTION_GETADDR:
            display_getaddr();
            break;
        case SEND_TRANSACTION_GETAMOUNT:
            display_getamount();
            break;
        case SEND_TRANSACTION_GETFEE:
            display_getfee();
            break;
        case SEND_TRANSACTION_FETCHING:
            display_fetching();
            break;
        case SEND_TRANSACTION_DISPLAYING:
            break;
    }
}