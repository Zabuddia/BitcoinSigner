#include "tx.h"

Tx* Tx_init(int version, unsigned long long num_inputs, TxIn** tx_ins, unsigned long long num_outputs, TxOut** tx_outs, unsigned long long locktime, __uint8_t testnet) {
    Tx* tx = (Tx*)malloc(sizeof(Tx));
    if (tx == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    tx->version = version;
    tx->num_inputs = num_inputs;
    tx->tx_ins = tx_ins;
    tx->num_outputs = num_outputs;
    tx->tx_outs = tx_outs;
    tx->locktime = locktime;
    tx->testnet = testnet;
    return tx;
}

void Tx_free(Tx* tx) {
    for (unsigned long long i = 0; i < tx->num_inputs; i++) {
        TxIn_free(tx->tx_ins[i]);
    }
    for (unsigned long long i = 0; i < tx->num_outputs; i++) {
        TxOut_free(tx->tx_outs[i]);
    }
    free(tx->tx_ins);
    free(tx->tx_outs);
    free(tx);
}

void Tx_id(Tx* tx, unsigned char* result) {
    unsigned char* serial = (unsigned char*)malloc(10000);
    if (serial == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    Tx_serialize(tx, serial);
    hash256(serial, 148 * tx->num_inputs + 34 * tx->num_outputs + 10, result);
    free(serial);
}

Tx* Tx_parse(unsigned char* s, uint8_t testnet) {
    unsigned char version_raw[4] = {s[0], s[1], s[2], s[3]};
    int version = little_endian_to_int(version_raw, 4);
    unsigned long long num_inputs = read_varint(s + 4);
    if (s[4] == 0xfd) {
        s += 7;
    } else if (s[4] == 0xfe) {
        s += 9;
    } else if (s[4] == 0xff) {
        s += 17;
    } else {
        s += 5;
    }
    TxIn** tx_ins = (TxIn**)malloc(num_inputs * sizeof(TxIn*));
    for (unsigned long long i = 0; i < num_inputs; i++) {
        TxIn* tx_in = TxIn_parse(s);
        tx_ins[i] = tx_in;
        s += 40 + 108;
    }
    unsigned long long num_outputs = read_varint(s);
    if (s[0] == 0xfd) {
        s += 3;
    } else if (s[0] == 0xfe) {
        s += 5;
    } else if (s[0] == 0xff) {
        s += 9;
    } else {
        s += 1;
    }
    TxOut** tx_outs = (TxOut**)malloc(num_outputs * sizeof(TxOut*));
    for (unsigned long long i = 0; i < num_outputs; i++) {
        TxOut* tx_out = TxOut_parse(s);
        tx_outs[i] = tx_out;
        s += 8 + 26;
    }
    unsigned long long locktime = little_endian_to_long(s, 4);
    Tx* tx = Tx_init(version, num_inputs, tx_ins, num_outputs, tx_outs, locktime, testnet);
    return tx;
}

void Tx_serialize(Tx* tx, unsigned char* result) {
    int_to_little_endian(tx->version, result, 4);
    encode_varint(result + 4, tx->num_inputs);
    if (tx->num_inputs < 253) {
        result += 5;
    } else if (tx->num_inputs < 65535) {
        result += 7;
    } else if (tx->num_inputs < 4294967295) {
        result += 9;
    } else {
        result += 17;
    }
    for (unsigned long long i = 0; i < tx->num_inputs; i++) {
        TxIn_serialize(tx->tx_ins[i], result + i * 148);
    }
    result += tx->num_inputs * 148;
    encode_varint(result, tx->num_outputs);
    if (tx->num_outputs < 253) {
        result += 1;
    } else if (tx->num_outputs < 65535) {
        result += 3;
    } else if (tx->num_outputs < 4294967295) {
        result += 5;
    } else {
        result += 9;
    }
    for (unsigned long long i = 0; i < tx->num_outputs; i++) {
        TxOut_serialize(tx->tx_outs[i], result + i * 34);
    }
    result += tx->num_outputs * 34;
    long_to_little_endian(tx->locktime, result, 4);
}

unsigned long long fee(Tx* tx, size_t testnet) {
    unsigned long long input_sum = 0;
    for (unsigned long long i = 0; i < tx->num_inputs; i++) {
        input_sum += value(tx->tx_ins[i], testnet);
    }
    unsigned long long output_sum = 0;
    for (unsigned long long i = 0; i < tx->num_outputs; i++) {
        output_sum += tx->tx_outs[i]->amount;
    }
    return input_sum - output_sum;
}

//Txin
TxIn* TxIn_init(unsigned char prev_tx[32], int prev_index, unsigned char* script_sig, int sequence) {
    TxIn* tx_in = (TxIn*)malloc(sizeof(TxIn));
    if (tx_in == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    memcpy(tx_in->prev_tx, prev_tx, 32);
    tx_in->prev_index = prev_index;
    memcpy(tx_in->script_sig, script_sig, 108);
    tx_in->sequence = sequence;
    return tx_in;
}

void TxIn_toString(TxIn* tx_in) {
    if (tx_in == NULL) {
        printf("TxIn_(NULL)\n");
    } else {
        printf("TxIn_(\n");
        printf("  prev_tx: %s\n", tx_in->prev_tx);
        printf("  prev_index: %d\n", tx_in->prev_index);
        printf(")\n");
    }
}

void TxIn_free(TxIn* tx_in) {
    free(tx_in);
}

TxIn* TxIn_parse(unsigned char* s) {
    int script_sig_len = 108;
    unsigned char prev_tx[32];
    unsigned char prev_index_raw[4];
    unsigned char script_sig[script_sig_len];
    unsigned char sequence_raw[4];
    memcpy(prev_tx, s, 32);
    memcpy(prev_index_raw, s + 32, 4);
    memcpy(script_sig, s + 36, script_sig_len);
    memcpy(sequence_raw, s + 36 + script_sig_len, 4);
    little_endian_to_big_endian(prev_tx, 32);
    int prev_index = little_endian_to_int(prev_index_raw, 4);
    int sequence = little_endian_to_int(sequence_raw, 4);
    return TxIn_init(prev_tx, prev_index, script_sig, sequence);
}

void TxIn_serialize(TxIn* tx_in, unsigned char* result) {
    memcpy(result, tx_in->prev_tx, 32);
    int_to_little_endian(tx_in->prev_index, result + 32, 4);
    memcpy(result + 36, tx_in->script_sig, 108);
    int_to_little_endian(tx_in->sequence, result + 36 + 108, 4);
}

Tx* fetch_tx(TxIn* txin, size_t testnet) {
    Tx* tx = fetch(txin->prev_tx, testnet);
    return tx;
}

unsigned long long value(TxIn* txin, size_t testnet) {
    Tx* tx = fetch_tx(txin, testnet);
    unsigned long long value = tx->tx_outs[txin->prev_index]->amount;
    free(tx);
    return value;
}

void script_pubkey(TxIn* txin, size_t testnet, unsigned char* result) {
    Tx* tx = fetch_tx(txin, testnet);
    result = tx->tx_outs[txin->prev_index]->script_pubkey;
    free(tx);
}

//Txout
TxOut* TxOut_init(unsigned long long amount, unsigned char* script_pubkey) {
    TxOut* tx_out = (TxOut*)malloc(sizeof(TxOut));
    if (tx_out == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    tx_out->amount = amount;
    memcpy(tx_out->script_pubkey, script_pubkey, 26);
    return tx_out;
}

void TxOut_toString(TxOut* tx_out) {
    if (tx_out == NULL) {
        printf("TxOut_(NULL)\n");
    } else {
        printf("TxOut_(\n");
        printf("  amount: %llu\n", tx_out->amount);
        printf("  script_pubkey: %s\n", tx_out->script_pubkey);
        printf(")\n");
    }
}

void TxOut_free(TxOut* tx_out) {
    free(tx_out);
}

TxOut* TxOut_parse(unsigned char* s) {
    int script_pubkey_len = 26;
    unsigned long long amount = little_endian_to_long(s, 8);
    unsigned char script_pubkey[script_pubkey_len];
    memcpy(script_pubkey, s + 8, script_pubkey_len);
    return TxOut_init(amount, script_pubkey);
}

void TxOut_serialize(TxOut* tx_out, unsigned char* result) {
    long_to_little_endian(tx_out->amount, result, 8);
    memcpy(result + 8, tx_out->script_pubkey, 26);
}

//Txfetcher
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

Tx *fetch(unsigned char *tx_id, size_t testnet) {
    char url[MAX_URL_LENGTH];
    snprintf(url, sizeof(url), "%stx/", get_url(testnet));
    int start = strlen(url);
    for (int i = 0; i < 32; i++) {
        snprintf(url + start + 2 * i, 3, "%02x", tx_id[i]);
    }
    snprintf(url + start + 64, 5, "%s", "/hex");
    printf("URL: %s\n", url);
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