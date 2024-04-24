#ifndef TXFETCHER_H
#define TXFETCHER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

#include "../helper/helper.h"
#include "tx.h"

#define MAX_URL_LENGTH 256

typedef struct {
    char tx_id[65];
    Tx *tx;
} TxCache;

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

char *http_get(const char *url);

const char *get_url(int testnet);

Tx *fetch(const char *tx_id, size_t testnet);

#endif //TXFETCHER_H