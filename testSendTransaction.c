#include "testSendTransaction.h"
#include "globals.h"
#include "config.h"
#include "bitcoin/ecc/privatekey.h"
#include "bitcoin/tx/tx.h"
#include "bitcoin/helper/helper.h"

enum test_send_transaction_state {
    SEND_TRANSACTION_WAITING,
    SEND_TRANSACTION_GETKEY,
    SEND_TRANSACTION_GETUTXOS,
    SEND_TRANSACTION_GETADDR,
    SEND_TRANSACTION_GETAMOUNT,
    SEND_TRANSACTION_GETFEE,
    SEND_TRANSACTION_FETCHING,
    SEND_TRANSACTION_DISPLAYING
} test_send_transaction_state;

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
    printf("Enter the private key: \n");
    scanf("%s", private_key);
    mpz_t secret_num;
    hash_to_mpz_t((const uint8_t*)private_key, strlen(private_key), secret_num);
    key = PrivateKey_init(secret_num);
    S256Point_address(key->point, (uint8_t*)public_key, true, false);
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
    char utxo_response[10000] = {0};
    get_utxos(public_key, utxo_response);
    extract_all_utxo_info(utxo_response, &txids, &vouts, &num_utxos);
    char str[10000] = {0};
    for (int32_t i = 0; i < num_utxos; i++) {
        uint64_t balance = get_utxo_balance(txids[i], public_key);
        format_utxo_info(str, i, txids[i], balance);
        strcat(str, "     ");
        printf("Txid: %s\n", txids[i]);
        printf("Vout: %d\n", vouts[i]);
    }
    printf("UTXO info: %s\n", str);
    utxo_indexes = (int32_t*)malloc(num_utxos * sizeof(int32_t));
    printf("Enter the number of utxos you would like to use: \n");
    scanf("%d", &num_utxo_indexes);
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        printf("Enter the index of the utxo you would like to use: \n");
        scanf("%d", &utxo_indexes[i]);
        while (is_duplicate(utxo_indexes, i, utxo_indexes[i])) {
            printf("You have already selected that utxo. Please select a different one.\n");
            scanf("%d", &utxo_indexes[i]);
        }
    }
    printf("UTXO indexes: ");
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        printf("%d ", utxo_indexes[i]);
    }
    printf("\n");

}

static void display_getaddr() {
    printf("Enter the target address: \n");
    scanf("%s", target_address);
    printf("Target address: %s\n", target_address);
}

static void display_getamount() {
    printf("Enter the amount to send: \n");
    scanf("%lu", &amount);
    printf("Amount: %lu\n", amount);
}

static void display_getfee() {
    printf("Enter the transaction fee: \n");
    scanf("%lu", &txFee);
    uint64_t total_balance = 0;
    for (int32_t i = 0; i < num_utxo_indexes; i++) {
        total_balance += get_utxo_balance(txids[utxo_indexes[i]], public_key);
    }
    change = total_balance - amount - txFee;
    printf("Fee: %lu\n", txFee);
    printf("Change: %lu\n", change);
    if (change < 0) {
        printf("Insufficient funds\n");
        return;
    }
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
    char tx_hex[10000] = {0};
    byte_array_to_hex_string(tx_serialized, Tx_length(tx), tx_hex);
    write_tx_to_file("tx.txt", tx_hex);
    printf("Tx hex: %s\n", tx_hex);
    printf("Verified?: %d\n", Tx_verify(tx));
    broadcast_transaction(tx_hex);
}

void test_send_transaction_init() {
    test_send_transaction_state = SEND_TRANSACTION_WAITING;
}

void test_send_transaction_tick() {
    //Transitions
    switch (test_send_transaction_state) {
        case SEND_TRANSACTION_WAITING:
            test_send_transaction_state = SEND_TRANSACTION_GETKEY;
            break;
        case SEND_TRANSACTION_GETKEY:
            test_send_transaction_state = SEND_TRANSACTION_GETUTXOS;
            break;
        case SEND_TRANSACTION_GETUTXOS:
            test_send_transaction_state = SEND_TRANSACTION_GETADDR;
            break;
        case SEND_TRANSACTION_GETADDR:
            test_send_transaction_state = SEND_TRANSACTION_GETAMOUNT;
            break;
        case SEND_TRANSACTION_GETAMOUNT:
            test_send_transaction_state = SEND_TRANSACTION_GETFEE;
            break;
        case SEND_TRANSACTION_GETFEE:
            test_send_transaction_state = SEND_TRANSACTION_FETCHING;
            break;
        case SEND_TRANSACTION_FETCHING:
            test_send_transaction_state = SEND_TRANSACTION_DISPLAYING;
            break;
        case SEND_TRANSACTION_DISPLAYING:
            test_send_transaction_state = SEND_TRANSACTION_WAITING;
            break;
    }
    //Actions
    switch (test_send_transaction_state) {
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