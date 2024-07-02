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

static void display_getkey() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the private key to send the transaction with.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%s", private_key);
    mpz_t secret_num;
    hash_to_mpz_t((const uint8_t*)private_key, 6, secret_num);
    key = PrivateKey_init(secret_num);
    S256Point_address(key->point, (uint8_t*)public_key, false, false);
}

static void display_getaddr() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the address to send the transaction to.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%s", target_address);
}

static void display_getamount() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the amount to send.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%lu", &amount);
}

static void display_getfee() {
    display_draw_string(STARTING_X, STARTING_Y, "Enter the fee to send the transaction with.", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
    scanf("%lu", &txFee);
    change = get_balance(public_key) - amount - txFee;
    if (change < 0) {
        display_draw_string(STARTING_X, STARTING_Y, "Insufficient funds!", DEFAULT_FONT, BACKGROUND_COLOR, FONT_COLOR);
        return;
    }
}

static void display_fetching() {
    char utxo_response[10000] = {0};
    char txid[65] = {0};
    int32_t vout = 0;
    get_utxos(public_key, utxo_response);
    extract_all_utxo_info(utxo_response, txid, &vout);
    uint8_t prev_tx[32] = {0};
    hex_string_to_byte_array(txid, prev_tx);
    Script* script_sig = Script_init();
    TxIn* tx_in = TxIn_init(prev_tx, vout, script_sig, 0xffffffff);
    uint8_t target_h160[20] = {0};
    decode_base58((uint8_t*)target_address, strlen(target_address), target_h160);
    Script* target_script = p2pkh_script(target_h160);
    TxOut* tx_out = TxOut_init(amount, target_script);
    uint8_t change_h160[20] = {0};
    decode_base58((uint8_t*)public_key, strlen(public_key), change_h160);
    Script* change_script = p2pkh_script(change_h160);
    TxOut* change_tx_out = TxOut_init(change, change_script);
    TxIn* inputs[1] = {tx_in};
    TxOut* outputs[2] = {tx_out, change_tx_out};
    Tx* tx = Tx_init(1, 1, inputs, 2, outputs, 0, false, false);
    sign_input(tx, 0, key);
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