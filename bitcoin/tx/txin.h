#ifndef TXIN_H
#define TXIN_H

#include <stdio.h>

typedef struct {
    unsigned char* prev_tx;
    unsigned char* prev_index;
    unsigned char* script_sig;
    unsigned char* sequence;
} TxIn;

#endif //TXIN_H