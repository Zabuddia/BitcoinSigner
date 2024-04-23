#ifndef TXOUT_H
#define TXOUT_H

#include <stdio.h>

#include "../helper/helper.h"

typedef struct {
    unsigned long long amount;
    unsigned char script_pubkey[26];
} TxOut;

TxOut* TxOut_init(unsigned long long amount, unsigned char* script_pubkey);

void TxOut_toString(TxOut* tx_out);

void TxOut_free(TxOut* tx_out);

TxOut* TxOut_parse(unsigned char* s);

void TxOut_serialize(TxOut* tx_out, unsigned char* result);

#endif //TXOUT_H