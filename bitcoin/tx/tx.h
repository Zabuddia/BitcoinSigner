#ifndef TX_H
#define TX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

#include "../helper/helper.h"
#include "../script/script.h"

#define MAX_URL_LENGTH 256

struct memory_struct {
    char *response;
    unsigned long long size;
};

typedef struct {
    unsigned char prev_tx[32];
    int prev_index;
    Script* script_sig;
    int sequence;
} TxIn;

typedef struct {
    unsigned long long amount;
    Script* script_pubkey;
} TxOut;

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

void Tx_id(Tx* tx, unsigned char* result);

Tx* Tx_parse(unsigned char* s, uint8_t testnet);

void Tx_serialize(Tx* tx, unsigned char* result);

unsigned long long fee(Tx* tx, size_t testnet);

//Txin
TxIn* TxIn_init(unsigned char prev_tx[32], int prev_index, Script* script_sig, int sequence);

void TxIn_toString(TxIn* tx_in);

void TxIn_free(TxIn* tx_in);

TxIn* TxIn_parse(unsigned char* s);

void TxIn_serialize(TxIn* tx_in, unsigned char* result);

Tx* fetch_tx(TxIn* txin, size_t testnet);

unsigned long long value(TxIn* txin, size_t testnet);

Script* script_pubkey(TxIn* txin, size_t testnet);

//Txout
TxOut* TxOut_init(unsigned long long amount, Script* script_pubkey);

void TxOut_toString(TxOut* tx_out);

void TxOut_free(TxOut* tx_out);

TxOut* TxOut_parse(unsigned char* s);

void TxOut_serialize(TxOut* tx_out, unsigned char* result);

//Txfetcher
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

unsigned long long http_get(const char *url, char* response);

const char *get_url(int testnet);

Tx *fetch(unsigned char *tx_id, size_t testnet);

#endif //TX_H