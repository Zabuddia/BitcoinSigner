#include "txin.h"

TxIn* TxIn_init(unsigned char prev_tx[32], int prev_index, unsigned char* script_sig, int sequence) {
    TxIn* tx_in = (TxIn*)malloc(sizeof(TxIn));
    if (tx_in == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    memcpy(tx_in->prev_tx, prev_tx, 32);
    tx_in->prev_index = prev_index;
    memcpy(tx_in->script_sig, script_sig, 108);
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

void TxIn_free(TxIn* tx_in) {
    free(tx_in);
}

TxIn* TxIn_parse(unsigned char* s) {
    int script_sig_len = 108;
    unsigned char prev_tx[32];
    unsigned char prev_index_raw[4];
    unsigned char script_sig[script_sig_len];
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

void TxIn_serialize(TxIn* tx_in, unsigned char* result) {
    memcpy(result, tx_in->prev_tx, 32);
    int_to_little_endian(tx_in->prev_index, result + 32, 4);
    memcpy(result + 36, tx_in->script_sig, 108);
    int_to_little_endian(tx_in->sequence, result + 36 + 108, 4);
}

Tx* fetch_tx(TxIn* txin, size_t testnet) {
    Tx* tx = fetch(txin->prev_tx, testnet);
    return tx;
}

unsigned long long value(TxIn* txin, size_t testnet) {
    Tx* tx = fetch_tx(txin, testnet);
    unsigned long long value = tx->tx_outs[txin->prev_index]->amount;
    free(tx);
    return value;
}

unsigned char* script_pubkey(TxIn* txin, size_t testnet) {
    Tx* tx = fetch_tx(txin, testnet);
    unsigned char script_pubkey[26] = tx->tx_outs[txin->prev_index]->script_pubkey;
    free(tx);
    return script_pubkey;
}