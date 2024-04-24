#include "txfetcher.h"

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t real_size = size * nmemb;
    char **response_ptr = (char **)userdata;
    char *old_response = *response_ptr;
    size_t old_len = old_response ? strlen(old_response) : 0;

    char *new_response = realloc(old_response, old_len + real_size + 1);
    if (new_response == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 0;
    }

    memcpy(new_response + old_len, ptr, real_size); // append new data
    new_response[old_len + real_size] = '\0'; // null-terminate

    *response_ptr = new_response;
    free(new_response);
    return real_size;
}

char *http_get(const char *url) {
    CURL *curl;
    CURLcode res;
    char *response = calloc(10000 ,sizeof(char)); // Allocate response buffer
    if (response == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return NULL;
    }
    memset(response, 0, 10000 * sizeof(char)); // Initialize response buffer
    response[0] = '\0';
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
    }
    return response;
}

const char *get_url(int testnet) {
    if (testnet)
        return "https://blockstream.info/testnet/api/";
    else
        return "https://blockstream.info/api/";
}

Tx *fetch(const char *tx_id, size_t testnet) {
    char url[MAX_URL_LENGTH];
    snprintf(url, sizeof(url), "%s/tx/%s/hex", get_url(testnet), tx_id);
    
    char *response = http_get(url);
    unsigned char* raw = (unsigned char*)response;
    // Assuming not a segwit transaction
    Tx* tx = Tx_parse(raw, testnet);

    unsigned char tx_id_result[32];
    Tx_id(tx, tx_id_result);
    if (memcmp(tx_id_result, tx_id, 32) != 0) {
        fprintf(stderr, "Tx ID mismatch\n");
        return NULL;
    }

    free(response);
    free(raw);

    return tx;
}