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

