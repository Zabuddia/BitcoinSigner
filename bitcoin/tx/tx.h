#ifndef TX_H
#define TX_H

#include <stdio.h>

#include "../helper/helper.h"
#include "txin.h"
#include "txout.h"

typedef struct {
    int version;
    unsigned long long num_inputs;
    TxIn** tx_ins;
    unsigned long long num_outputs;
    TxOut** tx_outs;
    unsigned long long locktime;
    __uint8_t testnet;
} Tx;

Tx* Tx_init(int version, unsigned long long num_inputs, TxIn** tx_ins, unsigned long long num_outputs, TxOut** tx_outs, unsigned long long locktime, __uint8_t testnet);

void Tx_free(Tx* tx);

Tx* Tx_parse(unsigned char* s, uint8_t testnet);

void Tx_serialize(Tx* tx, unsigned char* result);

#endif //TX_H