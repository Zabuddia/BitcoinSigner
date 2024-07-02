#ifndef TX_H
#define TX_H

#include <curl/curl.h>
#include <cjson/cJSON.h>

#include "../helper/helper.h"
#include "../script/script.h"
#include "../ecc/privatekey.h"

#define MAX_URL_LENGTH 256
#define PREV_TX_LEN 32

struct memory_struct {
    char* response;
    uint64_t size;
};

typedef struct {
    uint8_t prev_tx[PREV_TX_LEN];
    int32_t prev_index;
    Script* script_sig;
    int32_t sequence;
    uint8_t** witness;
    int32_t witness_count;
} TxIn;

typedef struct {
    uint64_t amount;
    Script* script_pubkey;
} TxOut;

typedef struct {
    int32_t version;
    uint64_t num_inputs;
    TxIn** tx_ins;
    uint64_t num_outputs;
    TxOut** tx_outs;
    uint64_t locktime;
    bool testnet;
    bool segwit;
} Tx;

Tx* Tx_init(int version, uint64_t num_inputs, TxIn** tx_ins, uint64_t num_outputs, TxOut** tx_outs, uint64_t locktime, bool testnet, bool segwit);

Tx* Tx_deep_copy(Tx* src);

uint64_t Tx_length(Tx* tx);

void Tx_free(Tx* tx);

void Tx_id(Tx* tx, uint8_t* result);

Tx* Tx_parse(uint8_t* s, bool testnet);

Tx* Tx_parse_legacy(uint8_t* s, bool testnet);

Tx* Tx_parse_segwit(uint8_t* s, bool testnet);

void Tx_serialize(Tx* tx, uint8_t* result);

void Tx_serialize_legacy(Tx* tx, uint8_t* result);

void Tx_serialize_segwit(Tx* tx, uint8_t* result);

uint64_t fee(Tx* tx, bool testnet);

void sig_hash(Tx* tx, uint64_t input_index, uint8_t* result, Script* redeem_script);

bool verify_input(Tx* tx, uint64_t input_index);

bool Tx_verify(Tx* tx);

bool sign_input(Tx* tx, uint64_t input_index, PrivateKey* private_key);

//Txin
TxIn* TxIn_init(uint8_t* prev_tx, int32_t prev_index, Script* script_sig, int32_t sequence);

TxIn* TxIn_deep_copy(TxIn* src);

uint64_t TxIn_length(TxIn* tx_in);

void TxIn_toString(TxIn* tx_in);

void TxIn_free(TxIn* tx_in);

TxIn* TxIn_parse(uint8_t* s);

void TxIn_serialize(TxIn* tx_in, uint8_t* result);

Tx* fetch_tx(TxIn* txin, bool testnet);

uint64_t value(TxIn* txin, bool testnet);

Script* TxIn_script_pubkey(TxIn* txin, bool testnet);

//Txout
TxOut* TxOut_init(uint64_t amount, Script* script_pubkey);

TxOut* TxOut_deep_copy(TxOut* src);

uint64_t TxOut_length(TxOut* tx_out);

void TxOut_toString(TxOut* tx_out);

void TxOut_free(TxOut* tx_out);

TxOut* TxOut_parse(uint8_t* s);

void TxOut_serialize(TxOut* tx_out, uint8_t* result);

//Txfetcher
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);

uint64_t http_get(const char* url, char* response);

const char* get_url(bool testnet);

uint64_t get_balance(const char* address);

uint64_t get_utxo_balance(const char* tx_id, const char* address);

void get_utxos(const char *address, char *response);

void extract_all_utxo_info(const char *response, char*** txid, int32_t** vout, int32_t* num_utxos);

void free_all_utxo_info(char** txid, int32_t* vout, int32_t num_utxos);

Tx* fetch(uint8_t* tx_id, bool testnet);

void broadcast_transaction(const char *tx_hex);

#endif //TX_H