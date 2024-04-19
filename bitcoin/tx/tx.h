#ifndef TX_H
#define TX_H

#include <stdio.h>

#include "../helper/helper.h"

typedef struct {
    int version;
    unsigned char* tx_ins;
    unsigned char* tx_outs;
    unsigned char* locktime;
    __uint8_t testnet;
} Tx;

Tx* Tx_init(int version, unsigned char* tx_ins, unsigned char* tx_outs, unsigned char* locktime, __uint8_t testnet);

void Tx_toString(Tx* tx);

unsigned char* Tx_id(Tx* tx);

unsigned char* Tx_hash(Tx* tx);

unsigned char* Tx_serialize(Tx* tx);

void Tx_free(Tx* tx);

Tx* Tx_parse(unsigned char* s, uint8_t testnet);

#endif //TX_H