#ifndef TXOUT_H
#define TXOUT_H

#include <stdio.h>

#include "../helper/helper.h"

typedef struct {
    unsigned long long amount;
    unsigned char* script_pubkey;
} TxOut;

TxOut* TxOut_init(unsigned long long amount, unsigned char* script_pubkey);

void TxOut_toString(TxOut* tx_out);

void TxOut_free(TxOut* tx_out);

TxOut* TxOut_parse(unsigned char* s);

#endif //TXOUT_H