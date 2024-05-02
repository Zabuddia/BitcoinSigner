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
    unsigned long long serial_length = 10;
    for (unsigned long long i = 0; i < tx->num_inputs; i++) {
        serial_length += 40;
        unsigned long long script_sig_length = 0;
        for (unsigned long long j = 0; j < tx->tx_ins[i]->script_sig->cmds_len; j++) {
            script_sig_length += tx->tx_ins[i]->script_sig->cmds[j].data_len;
            if (tx->tx_ins[i]->script_sig->cmds[j].data_len > 1) {
                script_sig_length++;
            }
        }
        script_sig_length++;
        serial_length += script_sig_length;
    }
    for (unsigned long long i = 0; i < tx->num_outputs; i++) {
        serial_length += 8;
        unsigned long long script_pubkey_length = 0;
        for (unsigned long long j = 0; j < tx->tx_outs[i]->script_pubkey->cmds_len; j++) {
            script_pubkey_length += tx->tx_outs[i]->script_pubkey->cmds[j].data_len;
            if (tx->tx_outs[i]->script_pubkey->cmds[j].data_len > 1) {
                script_pubkey_length++;
            }
        }
        script_pubkey_length++;
        serial_length += script_pubkey_length;
    }

    unsigned char serial[serial_length];

    Tx_serialize(tx, serial);
    hash256(serial, serial_length, result);
    little_endian_to_big_endian(result, 32);
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
        unsigned long long tx_in_len = read_varint(s + 36);
        //Not sure why I have to do this
        tx_in_len++;
        s += 40 + tx_in_len;
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
        unsigned long long tx_out_len = read_varint(s + 8);
        //Not sure why I have to do this
        tx_out_len++;
        s += 8 + tx_out_len;
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
        unsigned long long script_sig_length = 0;
        for (unsigned long long j = 0; j < tx->tx_ins[i]->script_sig->cmds_len; j++) {
            script_sig_length += tx->tx_ins[i]->script_sig->cmds[j].data_len;
            if (tx->tx_ins[i]->script_sig->cmds[j].data_len > 1) {
                script_sig_length++;
            }
        }
        script_sig_length++;
        unsigned long long tx_in_length = 40 + script_sig_length;
        TxIn_serialize(tx->tx_ins[i], result);
        result += tx_in_length;
    }
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
        unsigned long long script_pubkey_length = 0;
        for (unsigned long long j = 0; j < tx->tx_outs[i]->script_pubkey->cmds_len; j++) {
            script_pubkey_length += tx->tx_outs[i]->script_pubkey->cmds[j].data_len;
            if (tx->tx_outs[i]->script_pubkey->cmds[j].data_len > 1) {
                script_pubkey_length++;
            }
        }
        script_pubkey_length++;
        unsigned long long tx_out_length = 8 + script_pubkey_length;
        TxOut_serialize(tx->tx_outs[i], result);
        result += tx_out_length;
    }
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
TxIn* TxIn_init(unsigned char prev_tx[32], int prev_index, Script* script_sig, int sequence) {
    TxIn* tx_in = (TxIn*)malloc(sizeof(TxIn));
    if (tx_in == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    tx_in->script_sig = script_init();
    memcpy(tx_in->prev_tx, prev_tx, 32);
    tx_in->prev_index = prev_index;
    script_deep_copy(tx_in->script_sig, script_sig);
    tx_in->sequence = sequence;
    script_free(script_sig);
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
    script_free(tx_in->script_sig);
    free(tx_in);
}

TxIn* TxIn_parse(unsigned char* s) {
    unsigned char prev_tx[32];
    unsigned char prev_index_raw[4];
    Script* script_sig = script_parse(s + 36);
    unsigned long long script_sig_len = read_varint(s + 36);
    //Not sure why I have to do this
    script_sig_len++;
    unsigned char sequence_raw[4];
    memcpy(prev_tx, s, 32);
    little_endian_to_big_endian(prev_tx, 32);
    memcpy(prev_index_raw, s + 32, 4);
    memcpy(sequence_raw, s + 36 + script_sig_len, 4);
    int prev_index = little_endian_to_int(prev_index_raw, 4);
    unsigned char script_sig_raw[script_sig_len];
    script_serialize(script_sig, script_sig_raw);
    int sequence = little_endian_to_int(sequence_raw, 4);
    return TxIn_init(prev_tx, prev_index, script_sig, sequence);
}

void TxIn_serialize(TxIn* tx_in, unsigned char* result) {
    unsigned long long script_sig_len = 0;
    for (int i = 0; i < tx_in->script_sig->cmds_len; i++) {
        script_sig_len += tx_in->script_sig->cmds[i].data_len;
        if (tx_in->script_sig->cmds[i].data_len > 1) {
            script_sig_len++;
        }
    }
    //Not sure why I have to do this
    script_sig_len++;
    unsigned char prev_tx_copy[32] = {0};
    memcpy(prev_tx_copy, tx_in->prev_tx, 32);
    little_endian_to_big_endian(prev_tx_copy, 32);
    memcpy(result, prev_tx_copy, 32);
    // memcpy(result, tx_in->prev_tx, 32);
    int_to_little_endian(tx_in->prev_index, result + 32, 4);
    script_serialize(tx_in->script_sig, result + 36);
    int_to_little_endian(tx_in->sequence, result + 36 + script_sig_len, 4);
}

Tx* fetch_tx(TxIn* txin, size_t testnet) {
    Tx* tx = fetch(txin->prev_tx, testnet);
    return tx;
}

unsigned long long value(TxIn* txin, size_t testnet) {
    Tx* tx = fetch_tx(txin, testnet);
    unsigned long long value = tx->tx_outs[txin->prev_index]->amount;
    Tx_free(tx);
    return value;
}

Script* script_pubkey(TxIn* txin, size_t testnet) {
    Tx* tx = fetch_tx(txin, testnet);
    Script* result = script_init();
    script_deep_copy(result, tx->tx_outs[txin->prev_index]->script_pubkey);
    free(tx);
    return result;
}

//Txout
TxOut* TxOut_init(unsigned long long amount, Script* script_pubkey) {
    TxOut* tx_out = (TxOut*)malloc(sizeof(TxOut));
    if (tx_out == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    tx_out->script_pubkey = script_init();
    tx_out->amount = amount;
    script_deep_copy(tx_out->script_pubkey, script_pubkey);
    script_free(script_pubkey);
    return tx_out;
}

void TxOut_toString(TxOut* tx_out) {
    if (tx_out == NULL) {
        printf("TxOut_(NULL)\n");
    } else {
        printf("TxOut_(\n");
        printf("  amount: %llu\n", tx_out->amount);
        printf(")\n");
    }
}

void TxOut_free(TxOut* tx_out) {
    script_free(tx_out->script_pubkey);
    free(tx_out);
}

TxOut* TxOut_parse(unsigned char* s) {
    unsigned long long amount = little_endian_to_long(s, 8);
    Script* script_pubkey = script_parse(s + 8);
    return TxOut_init(amount, script_pubkey);
}

void TxOut_serialize(TxOut* tx_out, unsigned char* result) {
    long_to_little_endian(tx_out->amount, result, 8);
    script_serialize(tx_out->script_pubkey, result + 8);
}

//Txfetcher
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct memory_struct *mem = (struct memory_struct *)userp;

    char *ptr = realloc(mem->response, mem->size + real_size + 1);
    if (ptr == NULL) {
        printf("Not enough memory\n");
        return 0;  // will cause the download to stop
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), contents, real_size);
    mem->size += real_size;
    mem->response[mem->size] = '\0';
    return real_size;
}

unsigned long long http_get(const char* url, char* response) {
    CURL *curl;
    CURLcode res;

    struct memory_struct data;
    data.response = malloc(1);  // will be grown as needed by the realloc above
    data.size = 0;    // no data at this point

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // printf("Output: %s\n", data.response);
            memcpy(response, data.response, data.size);
        }
        // always cleanup
        curl_easy_cleanup(curl);
        free(data.response);
    }
    curl_global_cleanup();
    return data.size;
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
    char response[10000] = {0};
    http_get(url, response);
    printf("Response: %s\n", response);
    unsigned long long raw_length = strlen(response) / 2;
    unsigned char raw[raw_length];
    memset(raw, 0, raw_length);
    hex_string_to_byte_array(response, raw);

    // Assuming not a segwit transaction
    Tx* tx = Tx_parse(raw, testnet);

    unsigned char tx_id_result[32];
    Tx_id(tx, tx_id_result);
    if (memcmp(tx_id_result, tx_id, 32) != 0) {
        fprintf(stderr, "Tx ID mismatch\n");
        return NULL;
    }

    return tx;
}