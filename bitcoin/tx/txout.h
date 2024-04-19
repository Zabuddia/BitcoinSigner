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

#endif //TXOUT_H