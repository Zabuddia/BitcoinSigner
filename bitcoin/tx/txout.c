#include "txout.h"

TxOut* TxOut_init(unsigned long long amount, unsigned char* script_pubkey) {
    TxOut* tx_out = (TxOut*)malloc(sizeof(TxOut));
    if (tx_out == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    tx_out->amount = amount;
    tx_out->script_pubkey = script_pubkey;
    return tx_out;
}

void TxOut_toString(TxOut* tx_out) {
    if (tx_out == NULL) {
        printf("TxOut_(NULL)\n");
    } else {
        printf("TxOut_(\n");
        printf("  amount: %llu\n", tx_out->amount);
        printf("  script_pubkey: %s\n", tx_out->script_pubkey);
        printf(")\n");
    }
}

void TxOut_free(TxOut* tx_out) {
    free(tx_out->script_pubkey);
    free(tx_out);
}

TxOut* TxOut_parse(unsigned char* s) {
    int script_pubkey_len = 26;
    unsigned long long amount = little_endian_to_long(s, 8);
    unsigned char* script_pubkey = (unsigned char*)malloc(script_pubkey_len);
    if (script_pubkey == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    memcpy(script_pubkey, s + 8, script_pubkey_len);
    return TxOut_init(amount, script_pubkey);
}