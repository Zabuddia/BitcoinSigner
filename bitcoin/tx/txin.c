#include "txin.h"

TxIn* TxIn_init(unsigned char prev_tx[32], int prev_index, unsigned char* script_sig, int sequence) {
    TxIn* tx_in = (TxIn*)malloc(sizeof(TxIn));
    if (tx_in == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    memcpy(tx_in->prev_tx, prev_tx, 32);
    tx_in->prev_index = prev_index;
    tx_in->script_sig = script_sig;
    tx_in->sequence = sequence;
    return tx_in;
}

void TxIn_toString(TxIn* tx_in) {
    if (tx_in == NULL) {
        printf("TxIn_(NULL)\n");
    } else {
        printf("TxIn_(\n");
        printf("  prev_tx: %s\n", tx_in->prev_tx);
        printf("  prev_index: %d\n", tx_in->prev_index);
        printf(")\n");
    }
}

TxIn* TxIn_parse(unsigned char* s) {
    int script_sig_len = 108;
    unsigned char prev_tx[32];
    unsigned char prev_index_raw[4];
    unsigned char* script_sig = (unsigned char*)malloc(script_sig_len);
    unsigned char sequence_raw[4];
    memcpy(prev_tx, s, 32);
    memcpy(prev_index_raw, s + 32, 4);
    memcpy(script_sig, s + 36, script_sig_len);
    memcpy(sequence_raw, s + 36 + script_sig_len, 4);
    little_endian_to_big_endian(prev_tx, 32);
    int prev_index = little_endian_to_int(prev_index_raw, 4);
    int sequence = little_endian_to_int(sequence_raw, 4);
    return TxIn_init(prev_tx, prev_index, script_sig, sequence);
}