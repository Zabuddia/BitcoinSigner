#include "tx.h"

Tx* Tx_init(int version, unsigned long long num_inputs, TxIn** tx_ins, unsigned long long num_outputs, TxOut** tx_outs, unsigned char* locktime, __uint8_t testnet) {
    Tx* tx = (Tx*)malloc(sizeof(Tx));
    if (tx == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    tx->version = version;
    tx->num_inputs = num_inputs;
    tx->tx_ins = tx_ins;
    tx->num_outputs = num_outputs;
    tx->tx_outs = tx_outs;
    tx->locktime = locktime;
    tx->testnet = testnet;
    return tx;
}

void Tx_toString(Tx* tx) {
    if (tx == NULL) {
        printf("Tx_(NULL)\n");
    } else {
        printf("Tx_(\n");
        printf("  tx: %s\n", Tx_id(tx));
        printf("  version: %d\n", tx->version);
        printf("  tx_ins: ");
        for (unsigned long long i = 0; i < tx->num_inputs; i++) {
            TxIn_toString(tx->tx_ins[i]);
        }
        printf("  tx_outs: %s\n", tx->tx_outs);
        printf("  locktime: %s\n", tx->locktime);
        printf("  testnet: %d\n", tx->testnet);
        printf(")\n");
    }
}

unsigned char* Tx_id(Tx* tx) {
    return Tx_hash(tx);
}

unsigned char* Tx_hash(Tx* tx) {
    unsigned char result[32];
    unsigned char* tx_str = Tx_serialize(tx);
    hash256(tx_str, strlen(tx_str), result);
    free(tx_str);
    return result;
}

unsigned char* Tx_serialize(Tx* tx) {
    unsigned char* result = (unsigned char*)malloc(1000);
    if (result == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    sprintf(result, "%d%s%s%s", tx->version, tx->tx_ins, tx->tx_outs, tx->locktime);
    return result;
}

void Tx_free(Tx* tx) {
    for (unsigned long long i = 0; i < tx->num_inputs; i++) {
        TxIn_free(tx->tx_ins[i]);
    }
    for (unsigned long long i = 0; i < tx->num_outputs; i++) {
        TxOut_free(tx->tx_outs[i]);
    }
    free(tx->tx_ins);
    free(tx->tx_outs);
    free(tx);
}

Tx* Tx_parse(unsigned char* s, uint8_t testnet) {
    unsigned char version_raw[4] = {s[0], s[1], s[2], s[3]};
    int version = little_endian_to_int(version_raw, 4);
    unsigned long long num_inputs = read_varint(s + 4);
    if (s[4] == 0xfd) {
        s += 7;
    } else if (s[4] == 0xfe) {
        s += 9;
    } else if (s[4] == 0xff) {
        s += 17;
    } else {
        s += 5;
    }
    TxIn** tx_ins = (TxIn**)malloc(num_inputs * sizeof(TxIn*));
    for (unsigned long long i = 0; i < num_inputs; i++) {
        TxIn* tx_in = TxIn_parse(s);
        tx_ins[i] = tx_in;
        s += 40 + 108;
    }
    unsigned long long num_outputs = read_varint(s);
    if (s[0] == 0xfd) {
        s += 3;
    } else if (s[0] == 0xfe) {
        s += 5;
    } else if (s[0] == 0xff) {
        s += 9;
    } else {
        s += 1;
    }
    TxOut** tx_outs = (TxOut**)malloc(num_outputs * sizeof(TxOut*));
    for (unsigned long long i = 0; i < num_outputs; i++) {
        TxOut* tx_out = TxOut_parse(s);
        tx_outs[i] = tx_out;
        s += 8 + 26;
    }
    Tx* tx = Tx_init(version, num_inputs, tx_ins, num_outputs, tx_outs, NULL, testnet);
    return tx;
}