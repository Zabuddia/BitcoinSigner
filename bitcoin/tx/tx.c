#include "tx.h"

Tx* Tx_init(int version, unsigned char* tx_ins, unsigned char* tx_outs, unsigned char* locktime, __uint8_t testnet) {
    Tx* tx = (Tx*)malloc(sizeof(Tx));
    if (tx == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    tx->version = version;
    tx->tx_ins = tx_ins;
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
        printf("  tx_ins: %s\n", tx->tx_ins);
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
    free(tx);
}

Tx* Tx_parse(unsigned char* s, uint8_t testnet) {
    unsigned char version_raw[4] = {s[0], s[1], s[2], s[3]};
    int version = little_endian_to_int(version_raw, 4);
    
    Tx* tx = Tx_init(version, NULL, NULL, NULL, testnet);
    return tx;
}