#include "tx.h"

Tx* Tx_init(int version, unsigned long long num_inputs, TxIn** tx_ins, unsigned long long num_outputs, TxOut** tx_outs, unsigned long long locktime, __uint8_t testnet) {
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

void Tx_id(Tx* tx, unsigned char* result) {
    unsigned char* serial = (unsigned char*)malloc(10000);
    if (serial == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    Tx_serialize(tx, serial);
    hash256(serial, 148 * tx->num_inputs + 34 * tx->num_outputs + 10, result);
    free(serial);
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
    unsigned long long locktime = little_endian_to_long(s, 4);
    Tx* tx = Tx_init(version, num_inputs, tx_ins, num_outputs, tx_outs, locktime, testnet);
    return tx;
}

void Tx_serialize(Tx* tx, unsigned char* result) {
    int_to_little_endian(tx->version, result, 4);
    encode_varint(result + 4, tx->num_inputs);
    if (tx->num_inputs < 253) {
        result += 5;
    } else if (tx->num_inputs < 65535) {
        result += 7;
    } else if (tx->num_inputs < 4294967295) {
        result += 9;
    } else {
        result += 17;
    }
    for (unsigned long long i = 0; i < tx->num_inputs; i++) {
        TxIn_serialize(tx->tx_ins[i], result + i * 148);
    }
    result += tx->num_inputs * 148;
    encode_varint(result, tx->num_outputs);
    if (tx->num_outputs < 253) {
        result += 1;
    } else if (tx->num_outputs < 65535) {
        result += 3;
    } else if (tx->num_outputs < 4294967295) {
        result += 5;
    } else {
        result += 9;
    }
    for (unsigned long long i = 0; i < tx->num_outputs; i++) {
        TxOut_serialize(tx->tx_outs[i], result + i * 34);
    }
    result += tx->num_outputs * 34;
    long_to_little_endian(tx->locktime, result, 4);
}