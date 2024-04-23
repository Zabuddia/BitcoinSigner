#ifndef TXIN_H
#define TXIN_H

#include <stdio.h>

#include "../helper/helper.h"

typedef struct {
    unsigned char prev_tx[32];
    int prev_index;
    unsigned char script_sig[108];
    int sequence;
} TxIn;

TxIn* TxIn_init(unsigned char prev_tx[32], int prev_index, unsigned char* script_sig, int sequence);

void TxIn_toString(TxIn* tx_in);

void TxIn_free(TxIn* tx_in);

TxIn* TxIn_parse(unsigned char* s);

void TxIn_serialize(TxIn* tx_in, unsigned char* result);

#endif //TXIN_H