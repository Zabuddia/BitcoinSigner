#include "tx.h"

Tx* Tx_init(int version, uint64_t num_inputs, TxIn** tx_ins, uint64_t num_outputs, TxOut** tx_outs, uint64_t locktime, bool testnet, bool segwit) {
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
    tx->segwit = segwit;
    return tx;
}

Tx* Tx_deep_copy(Tx* src) {
    Tx* new_tx = (Tx*)malloc(sizeof(Tx));
    if (!new_tx) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    new_tx->version = src->version;
    new_tx->num_inputs = src->num_inputs;
    new_tx->tx_ins = (TxIn**)malloc(src->num_inputs * sizeof(TxIn*));
    for (uint64_t i = 0; i < src->num_inputs; i++) {
        new_tx->tx_ins[i] = TxIn_deep_copy(src->tx_ins[i]);
    }
    new_tx->num_outputs = src->num_outputs;
    new_tx->tx_outs = (TxOut**)malloc(src->num_outputs * sizeof(TxOut*));
    for (uint64_t i = 0; i < src->num_outputs; i++) {
        new_tx->tx_outs[i] = TxOut_deep_copy(src->tx_outs[i]);
    }
    new_tx->locktime = src->locktime;
    new_tx->testnet = src->testnet;
    new_tx->segwit = src->segwit;
    return new_tx;
}

uint64_t Tx_length(Tx* tx) {
    uint64_t length = 8;
    for (uint64_t i = 0; i < tx->num_inputs; i++) {
        length += TxIn_length(tx->tx_ins[i]);
    }
    for (uint64_t i = 0; i < tx->num_outputs; i++) {
        length += TxOut_length(tx->tx_outs[i]);
    }
    return length;
}

void Tx_free(Tx* tx) {
    for (uint64_t i = 0; i < tx->num_inputs; i++) {
        TxIn_free(tx->tx_ins[i]);
    }
    for (uint64_t i = 0; i < tx->num_outputs; i++) {
        TxOut_free(tx->tx_outs[i]);
    }
    free(tx->tx_ins);
    free(tx->tx_outs);
    free(tx);
}

void Tx_id(Tx* tx, uint8_t* result) {
    uint64_t serial_length = 10;
    for (uint64_t i = 0; i < tx->num_inputs; i++) {
        uint64_t tx_in_length = TxIn_length(tx->tx_ins[i]);
        serial_length += tx_in_length;
    }
    for (uint64_t i = 0; i < tx->num_outputs; i++) {
        uint64_t tx_out_length = TxOut_length(tx->tx_outs[i]);
        serial_length += tx_out_length;
    }

    uint8_t serial[serial_length];

    Tx_serialize_legacy(tx, serial);

    hash256(serial, serial_length, result);
    little_endian_to_big_endian(result, HASH256_LEN);
}

Tx* Tx_parse(uint8_t* s, bool testnet) {
    if (s[4] == 0) {
        return Tx_parse_segwit(s, testnet);
    } else {
        return Tx_parse_legacy(s, testnet);
    }
}

Tx* Tx_parse_legacy(uint8_t* s, bool testnet) {
    uint8_t version_raw[4] = {s[0], s[1], s[2], s[3]};
    int32_t version = little_endian_to_int(version_raw, 4);
    uint64_t num_inputs = read_varint(s + 4);
    s += 4;
    s += read_varint_size(s);
    TxIn** tx_ins = (TxIn**)malloc(num_inputs * sizeof(TxIn*));
    for (uint64_t i = 0; i < num_inputs; i++) {
        TxIn* tx_in = TxIn_parse(s);
        tx_ins[i] = tx_in;
        uint64_t tx_in_len = read_varint(s + 36);
        tx_in_len += add_varint_size(tx_in_len);
        s += 40 + tx_in_len;
    }
    uint64_t num_outputs = read_varint(s);
    s += read_varint_size(s);
    TxOut** tx_outs = (TxOut**)malloc(num_outputs * sizeof(TxOut*));
    for (uint64_t i = 0; i < num_outputs; i++) {
        TxOut* tx_out = TxOut_parse(s);
        tx_outs[i] = tx_out;
        uint64_t tx_out_len = read_varint(s + 8);
        tx_out_len += add_varint_size(tx_out_len);
        s += 8 + tx_out_len;
    }
    uint64_t locktime = little_endian_to_long(s, 4);
    Tx* tx = Tx_init(version, num_inputs, tx_ins, num_outputs, tx_outs, locktime, testnet, false);
    return tx;
}

Tx* Tx_parse_segwit(uint8_t* s, bool testnet) {
    uint8_t version_raw[4] = {s[0], s[1], s[2], s[3]};
    int version = little_endian_to_int(version_raw, 4);
    uint8_t marker_raw[2] = {s[4], s[5]};
    if (marker_raw[0] != 0 || marker_raw[1] != 1) {
        printf("Not a segwit transaction\n");
        exit(1);
    }
    uint64_t num_inputs = read_varint(s + 6);
    s += 6;
    s += read_varint_size(s);
    TxIn** tx_ins = (TxIn**)malloc(num_inputs * sizeof(TxIn*));
    for (uint64_t i = 0; i < num_inputs; i++) {
        TxIn* tx_in = TxIn_parse(s);
        tx_ins[i] = tx_in;
        uint64_t tx_in_len = read_varint(s + 36);
        tx_in_len += add_varint_size(tx_in_len);
        s += 40 + tx_in_len;
    }
    uint64_t num_outputs = read_varint(s);
    s += read_varint_size(s);
    TxOut** tx_outs = (TxOut**)malloc(num_outputs * sizeof(TxOut*));
    for (uint64_t i = 0; i < num_outputs; i++) {
        TxOut* tx_out = TxOut_parse(s);
        tx_outs[i] = tx_out;
        uint64_t tx_out_len = read_varint(s + 8);
        tx_out_len += add_varint_size(tx_out_len);
        s += 8 + tx_out_len;
    }

    for (uint64_t i = 0; i < num_inputs; i++) {
        uint64_t num_items = read_varint(s);
        s += read_varint_size(s);

        uint8_t** witness = (uint8_t**)malloc(num_items * sizeof(uint8_t*));
        for (uint64_t j = 0; j < num_items; j++) {
            uint64_t item_len = read_varint(s);
            uint8_t* item = (uint8_t*)malloc(item_len);
            memcpy(item, s, item_len);
            witness[j] = item;
            s += item_len;
        }
        tx_ins[i]->witness = witness;
        tx_ins[i]->witness_count = num_items;
    }

    uint64_t locktime = little_endian_to_long(s, 4);

    Tx* tx = Tx_init(version, num_inputs, tx_ins, num_outputs, tx_outs, locktime, testnet, true);

    return tx;
}

void Tx_serialize(Tx* tx, uint8_t* result) {
    if (tx->segwit) {
        Tx_serialize_segwit(tx, result);
    } else {
        Tx_serialize_legacy(tx, result);
    }
}

void Tx_serialize_legacy(Tx* tx, uint8_t* result) {
    int_to_little_endian(tx->version, result, 4);
    encode_varint(result + 4, tx->num_inputs);
    result += 4;
    result += add_varint_size(tx->num_inputs);
    for (uint64_t i = 0; i < tx->num_inputs; i++) {
        uint64_t tx_in_length = TxIn_length(tx->tx_ins[i]);
        TxIn_serialize(tx->tx_ins[i], result);
        result += tx_in_length;
    }
    encode_varint(result, tx->num_outputs);
    result += add_varint_size(tx->num_outputs);
    for (uint64_t i = 0; i < tx->num_outputs; i++) {
        uint64_t tx_out_length = TxOut_length(tx->tx_outs[i]);
        TxOut_serialize(tx->tx_outs[i], result);
        result += tx_out_length;
    }
    long_to_little_endian(tx->locktime, result, 4);
}

void Tx_serialize_segwit(Tx* tx, uint8_t* result) {
    int_to_little_endian(tx->version, result, 4);
    result += 4;
    result[0] = 0x00;
    result[1] = 0x01;
    result += 2;
    encode_varint(result, tx->num_inputs);
    result += add_varint_size(tx->num_inputs);
    for (uint64_t i = 0; i < tx->num_inputs; i++) {
        uint64_t script_sig_length = 0;
        for (uint64_t j = 0; j < tx->tx_ins[i]->script_sig->cmds_len; j++) {
            script_sig_length += tx->tx_ins[i]->script_sig->cmds[j].data_len;
            if (tx->tx_ins[i]->script_sig->cmds[j].data_len > 1) {
                script_sig_length++;
            }
        }
        script_sig_length += add_varint_size(script_sig_length);
        uint64_t tx_in_length = 40 + script_sig_length;
        TxIn_serialize(tx->tx_ins[i], result);
        result += tx_in_length;
    }
    encode_varint(result, tx->num_outputs);
    result += add_varint_size(tx->num_outputs);
    for (uint64_t i = 0; i < tx->num_outputs; i++) {
        uint64_t script_pubkey_length = 0;
        for (uint64_t j = 0; j < tx->tx_outs[i]->script_pubkey->cmds_len; j++) {
            script_pubkey_length += tx->tx_outs[i]->script_pubkey->cmds[j].data_len;
            if (tx->tx_outs[i]->script_pubkey->cmds[j].data_len > 1) {
                script_pubkey_length++;
            }
        }
        script_pubkey_length += add_varint_size(script_pubkey_length);
        uint64_t tx_out_length = 8 + script_pubkey_length;
        TxOut_serialize(tx->tx_outs[i], result);
        result += tx_out_length;
    }

    for (uint64_t i = 0; i < tx->num_inputs; i++) {
        encode_varint(result, tx->tx_ins[i]->witness_count);
        result += add_varint_size(tx->tx_ins[i]->witness_count);
        
        for (uint64_t j = 0; j < tx->tx_ins[i]->witness_count; j++) {
            uint8_t* item = tx->tx_ins[i]->witness[j];
            uint64_t item_len = strlen((char*)item); // Assume witness item is a string, adjust accordingly
            encode_varint(result, item_len);
            result += add_varint_size(item_len);
            memcpy(result, item, item_len);
            result += item_len;
        }
    }
    
    int_to_little_endian(tx->locktime, result, 4);
    result += 4;
}

uint64_t fee(Tx* tx, bool testnet) {
    uint64_t input_sum = 0;
    for (uint64_t i = 0; i < tx->num_inputs; i++) {
        input_sum += value(tx->tx_ins[i], testnet);
    }
    uint64_t output_sum = 0;
    for (uint64_t i = 0; i < tx->num_outputs; i++) {
        output_sum += tx->tx_outs[i]->amount;
    }
    return input_sum - output_sum;
}

void sig_hash(Tx* tx, uint64_t input_index, uint8_t* result, Script* redeem_script) {
    int total_length = 0;
    uint8_t* s = (uint8_t*)malloc(10000);
    int_to_little_endian(tx->version, s, 4);
    s += 4;
    total_length += 4;
    encode_varint(s, tx->num_inputs);
    s += add_varint_size(tx->num_inputs);
    total_length += add_varint_size(tx->num_inputs);
    for (uint64_t i = 0; i < tx->num_inputs; i++) {
        if (i == input_index) {
            if (redeem_script != NULL) {
                Script_deep_copy(tx->tx_ins[i]->script_sig, redeem_script);
            } else {
                Script* script_sig = TxIn_script_pubkey(tx->tx_ins[i], tx->testnet);
                Script_deep_copy(tx->tx_ins[i]->script_sig, script_sig);
                Script_free(script_sig);
            }
            uint64_t script_sig_length = Script_length(tx->tx_ins[i]->script_sig);
            uint64_t tx_in_length = 40 + script_sig_length;
            TxIn_serialize(tx->tx_ins[i], s);
            s += tx_in_length;
            total_length += tx_in_length;
        } else {
            uint8_t prev_tx[32] = {0};
            memcpy(prev_tx, tx->tx_ins[i]->prev_tx, 32);
            little_endian_to_big_endian(prev_tx, 32);
            memcpy(s, prev_tx, 32);
            s += 32;
            total_length += 32;
            int_to_little_endian(tx->tx_ins[i]->prev_index, s, 4);
            s += 4;
            total_length += 4;
            //Replaces the script_sig with a 0
            s[0] = 0x00;
            s++;
            total_length++;
            int_to_little_endian(tx->tx_ins[i]->sequence, s, 4);
            s += 4;
            total_length += 4;
        }
    }
    encode_varint(s, tx->num_outputs);
    s += add_varint_size(tx->num_outputs);
    total_length += add_varint_size(tx->num_outputs);
    for (uint64_t i = 0; i < tx->num_outputs; i++) {
        uint64_t script_pubkey_length = Script_length(tx->tx_outs[i]->script_pubkey);
        uint64_t tx_out_length = 8 + script_pubkey_length;
        TxOut_serialize(tx->tx_outs[i], s);
        s += tx_out_length;
        total_length += tx_out_length;
    }
    long_to_little_endian(tx->locktime, s, 4);
    s += 4;
    total_length += 4;
    int sighash_all = SIGHASH_ALL;
    int_to_little_endian(sighash_all, s, 4);
    s += 4;
    total_length += 4;
    if (total_length > 10000) {
        printf("Total length too long in sig_hash()\n");
        exit(1);
    }
    s -= total_length;
    hash256(s, total_length, result);
    free(s);
}

bool verify_input(Tx* tx, uint64_t input_index) {
    Tx* tx_copy = Tx_deep_copy(tx);
    TxIn* tx_in = tx_copy->tx_ins[input_index];
    Script* script_pubkey = TxIn_script_pubkey(tx_in, tx_copy->testnet);
    Script* script_sig = Script_init();
    Script_deep_copy(script_sig, tx_in->script_sig);
    uint8_t z_raw[32] = {0};
    if (is_p2sh_script_pubkey(script_pubkey)) {
        Command cmd = script_sig->cmds[script_sig->cmds_len - 1];
        int cmd_length = cmd.data_len;
        // Not accounting for if the varint is bigger than 1 byte
        uint8_t raw_redeem[cmd_length + 1];
        encode_varint(raw_redeem, cmd_length);
        memcpy(raw_redeem + 1, cmd.data, cmd_length);
        Script* redeem_script = Script_parse(raw_redeem);
        if (is_p2wpkh_script_pubkey(redeem_script)) {
            //Finish here
            //sig_hash_bip143(tx_copy, input_index, z_raw, redeem_script);
            Script_free(redeem_script);
        } else {
            sig_hash(tx_copy, input_index, z_raw, redeem_script);
            Script_free(redeem_script);
        }
    } else {
        sig_hash(tx_copy, input_index, z_raw, NULL);
    }
    char z_str[65] = {0};
    byte_array_to_hex_string(z_raw, 32, z_str);
    mpz_t z_mpz;
    mpz_init_set_str(z_mpz, z_str, HEX);
    S256Field* z = S256Field_init(z_mpz);
    Script* combined = Script_add(script_sig, script_pubkey);
    uint64_t combined_length = Script_length(combined);
    uint8_t combined_serialized[combined_length];
    Script_serialize(combined, combined_serialized);
    bool result = Script_evaluate(combined, z);
    Script_free(script_sig);
    Script_free(script_pubkey);
    Script_free(combined);
    S256Field_free(z);
    Tx_free(tx_copy);
    return result;
}

bool Tx_verify(Tx* tx) {
    uint64_t tx_fee = fee(tx, tx->testnet);
    if (tx_fee < 0) {
        return false;
    }
    for (uint64_t i = 0; i < tx->num_inputs; i++) {
        if (!verify_input(tx, i)) {
            return false;
        }
    }
    return true;
}

bool sign_input(Tx* tx, uint64_t input_index, PrivateKey* private_key) {
    uint8_t z_raw[32] = {0};
    sig_hash(tx, input_index, z_raw, NULL);
    char z_str[65] = {0};
    byte_array_to_hex_string(z_raw, 32, z_str);
    mpz_t z_mpz;
    mpz_init_set_str(z_mpz, z_str, 16);
    S256Field* z = S256Field_init(z_mpz);
    Signature* sig = PrivateKey_sign(private_key, z);
    int32_t der_length = Signature_der_length(sig);
    uint8_t der[der_length + 1];
    Signature_der(sig, der);
    uint8_t sighash_all[1] = {0};
    int_to_little_endian(SIGHASH_ALL, sighash_all, 1);
    der[der_length] = sighash_all[0];
    Command sig_cmd;
    memcpy(sig_cmd.data, der, der_length + 1);
    sig_cmd.data_len = der_length + 1;
    uint8_t sec[33] = {0};
    S256Point_sec_compressed(private_key->point, sec);
    Command sec_cmd;
    memcpy(sec_cmd.data, sec, 33);
    sec_cmd.data_len = 33;
    Command cmds[2] = {sig_cmd, sec_cmd};
    Script* script_sig = Script_init();
    Script_set_cmds(script_sig, cmds, 2);
    Script_deep_copy(tx->tx_ins[input_index]->script_sig, script_sig);
    Script_free(script_sig);
    S256Field_free(z);
    Signature_free(sig);
    bool result = verify_input(tx, input_index);
    return result;
}

//Txin
TxIn* TxIn_init(uint8_t* prev_tx, int32_t prev_index, Script* script_sig, int32_t sequence) {
    TxIn* tx_in = (TxIn*)malloc(sizeof(TxIn));
    if (tx_in == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    tx_in->script_sig = Script_init();
    memcpy(tx_in->prev_tx, prev_tx, PREV_TX_LEN);
    tx_in->prev_index = prev_index;
    Script_deep_copy(tx_in->script_sig, script_sig);
    tx_in->sequence = sequence;
    Script_free(script_sig);
    return tx_in;
}

TxIn* TxIn_deep_copy(TxIn* src) {
    TxIn* new_tx_in = (TxIn*)malloc(sizeof(TxIn));
    if (!new_tx_in) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    memcpy(new_tx_in->prev_tx, src->prev_tx, 32);
    new_tx_in->prev_index = src->prev_index;
    new_tx_in->script_sig = Script_init();
    Script_deep_copy(new_tx_in->script_sig, src->script_sig);
    new_tx_in->sequence = src->sequence;
    return new_tx_in;
}

uint64_t TxIn_length(TxIn* tx_in) {
    uint64_t script_sig_length = Script_length(tx_in->script_sig);
    return 40 + script_sig_length;
}

void TxIn_toString(TxIn* tx_in) {
    if (tx_in == NULL) {
        printf("TxIn_(NULL)\n");
    } else {
        uint64_t length = TxIn_length(tx_in);
        uint8_t serialized[length];
        TxIn_serialize(tx_in, serialized);
        for (uint64_t i = 0; i < length; i++) {
            printf("%02x", serialized[i]);
        }
        printf("\n");
    }
}

void TxIn_free(TxIn* tx_in) {
    Script_free(tx_in->script_sig);
    free(tx_in);
}

TxIn* TxIn_parse(uint8_t* s) {
    uint8_t prev_tx[PREV_TX_LEN];
    uint8_t prev_index_raw[4];
    Script* script_sig = Script_parse(s + 36);
    uint64_t script_sig_len = read_varint(s + 36);
    script_sig_len += add_varint_size(script_sig_len);
    uint8_t sequence_raw[4];
    memcpy(prev_tx, s, 32);
    little_endian_to_big_endian(prev_tx, 32);
    memcpy(prev_index_raw, s + 32, 4);
    memcpy(sequence_raw, s + 36 + script_sig_len, 4);
    int32_t prev_index = little_endian_to_int(prev_index_raw, 4);
    int32_t sequence = little_endian_to_int(sequence_raw, 4);
    return TxIn_init(prev_tx, prev_index, script_sig, sequence);
}

void TxIn_serialize(TxIn* tx_in, uint8_t* result) {
    uint64_t script_sig_len = Script_length(tx_in->script_sig);
    uint8_t prev_tx_copy[32] = {0};
    memcpy(prev_tx_copy, tx_in->prev_tx, 32);
    little_endian_to_big_endian(prev_tx_copy, 32);
    memcpy(result, prev_tx_copy, 32);
    int_to_little_endian(tx_in->prev_index, result + 32, 4);
    Script_serialize(tx_in->script_sig, result + 36);
    int_to_little_endian(tx_in->sequence, result + 36 + script_sig_len, 4);
}

Tx* fetch_tx(TxIn* txin, bool testnet) {
    Tx* tx = fetch(txin->prev_tx, testnet);
    return tx;
}

uint64_t value(TxIn* txin, bool testnet) {
    Tx* tx = fetch_tx(txin, testnet);
    uint64_t value = tx->tx_outs[txin->prev_index]->amount;
    Tx_free(tx);
    return value;
}

Script* TxIn_script_pubkey(TxIn* txin, bool testnet) {
    Tx* tx = fetch_tx(txin, testnet);
    Script* result = Script_init();
    Script_deep_copy(result, tx->tx_outs[txin->prev_index]->script_pubkey);
    Tx_free(tx);
    return result;
}

//Txout
TxOut* TxOut_init(uint64_t amount, Script* script_pubkey) {
    TxOut* tx_out = (TxOut*)malloc(sizeof(TxOut));
    if (tx_out == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    tx_out->script_pubkey = Script_init();
    tx_out->amount = amount;
    Script_deep_copy(tx_out->script_pubkey, script_pubkey);
    Script_free(script_pubkey);
    return tx_out;
}

TxOut* TxOut_deep_copy(TxOut* src) {
    TxOut* new_tx_out = (TxOut*)malloc(sizeof(TxOut));
    if (!new_tx_out) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    new_tx_out->amount = src->amount;
    new_tx_out->script_pubkey = Script_init();
    Script_deep_copy(new_tx_out->script_pubkey, src->script_pubkey);
    return new_tx_out;
}

uint64_t TxOut_length(TxOut* tx_out) {
    uint64_t script_pubkey_length = Script_length(tx_out->script_pubkey);
    return 8 + script_pubkey_length;
}

void TxOut_toString(TxOut* tx_out) {
    if (tx_out == NULL) {
        printf("TxOut_(NULL)\n");
    } else {
        uint64_t length = TxOut_length(tx_out);
        uint8_t serialized[length];
        TxOut_serialize(tx_out, serialized);
        for (uint64_t i = 0; i < length; i++) {
            printf("%02x", serialized[i]);
        }
        printf("\n");
    }
}

void TxOut_free(TxOut* tx_out) {
    Script_free(tx_out->script_pubkey);
    free(tx_out);
}

TxOut* TxOut_parse(uint8_t* s) {
    uint64_t amount = little_endian_to_long(s, 8);
    //uint64_t script_pubkey_len = read_varint(s + 8);
    // Maybe this is right? I don't think I would need to do this
    // if (script_pubkey_len == 0xfd) {
    //     script_pubkey_len += 3;
    // } else if (script_pubkey_len == 0xfe) {
    //     script_pubkey_len += 5;
    // } else if (script_pubkey_len == 0xff) {
    //     script_pubkey_len += 9;
    // } else {
    //     script_pubkey_len++;
    // }
    Script* script_pubkey = Script_parse(s + 8);
    // uint8_t script_pubkey_serialized[script_pubkey_len];
    // Script_serialize(script_pubkey, script_pubkey_serialized);
    // printf("Script_pubkey: ");
    // for (int i = 0; i < script_pubkey_len; i++) {
    //     printf("%02x", script_pubkey_serialized[i]);
    // }
    // printf("\n");
    return TxOut_init(amount, script_pubkey);
}

void TxOut_serialize(TxOut* tx_out, uint8_t* result) {
    long_to_little_endian(tx_out->amount, result, 8);
    Script_serialize(tx_out->script_pubkey, result + 8);
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

uint64_t http_get(const char* url, char* response) {
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

const char* get_url(bool testnet) {
    if (testnet)
        return "https://blockstream.info/testnet/api/";
    else
        return "https://blockstream.info/api/";
}

uint64_t get_balance(const char* address) {
    char url[256] = {0};
    char response[10000] = {0};
    snprintf(url, sizeof(url), "https://api.blockcypher.com/v1/btc/main/addrs/%s/balance", address);

    uint64_t response_size = http_get(url, response);

    if (response_size == 0) {
        fprintf(stderr, "Failed to fetch balance\n");
        return 0;
    }

    // Print the raw response for debugging
    printf("HTTP response: %s\n", response);

    cJSON *json = cJSON_Parse(response);
    if (json == NULL) {
        fprintf(stderr, "Failed to parse JSON\n");
        return 0;
    }

    // Print the parsed JSON object for debugging
    char *json_string = cJSON_Print(json);
    if (json_string == NULL) {
        fprintf(stderr, "Failed to print JSON\n");
    } else {
        printf("Parsed JSON: %s\n", json_string);
        free(json_string); // Remember to free the allocated string
    }

    uint64_t balance = 0;
    cJSON *balance_json = cJSON_GetObjectItemCaseSensitive(json, "final_balance");
    if (balance_json == NULL) {
        fprintf(stderr, "JSON does not contain 'final_balance' field\n");
    } else if (!cJSON_IsNumber(balance_json)) {
        fprintf(stderr, "'final_balance' is not a number\n");
    } else {
        balance = balance_json->valueint;
    }

    cJSON_Delete(json);
    return balance;
}

uint64_t get_utxo_balance(const char* tx_id, const char* address) {
    char url[MAX_URL_LENGTH];
    snprintf(url, sizeof(url), "%stx/", get_url(false));
    int32_t start = strlen(url);
    snprintf(url + start, 65, "%s", tx_id);
    //printf("URL: %s\n", url);
    char response[10000] = {0};
    http_get(url, response);
    cJSON *json = cJSON_Parse(response);
    if (json == NULL) {
        fprintf(stderr, "Error parsing JSON response\n");
        return 0;
    }
    cJSON *vout = cJSON_GetObjectItemCaseSensitive(json, "vout");
    if (!cJSON_IsArray(vout)) {
        fprintf(stderr, "No vout found\n");
        cJSON_Delete(json);
        return 0;
    }
    cJSON *output;
    cJSON_ArrayForEach(output, vout) {
        cJSON * scriptpubkey_address = cJSON_GetObjectItemCaseSensitive(output, "scriptpubkey_address");
        if (cJSON_IsString(scriptpubkey_address) && strcmp(scriptpubkey_address->valuestring, address) == 0) {
            cJSON *value = cJSON_GetObjectItemCaseSensitive(output, "value");
            if (cJSON_IsNumber(value)) {
                cJSON_Delete(json);
                return value->valueint;
            }
        }
    }
    cJSON_Delete(json);
    return 0;
}

void get_utxos(const char *address, char *response) {
    char url[256];
    snprintf(url, sizeof(url), "https://api.blockcypher.com/v1/btc/main/addrs/%s?unspentOnly=true", address);
    http_get(url, response);
}

void extract_all_utxo_info(const char *response, char*** txid, int32_t** vout, int32_t* num_utxos) {
    cJSON *json = cJSON_Parse(response);
    if (json == NULL) {
        fprintf(stderr, "Error parsing JSON response\n");
        return;
    }

    cJSON *txrefs = cJSON_GetObjectItemCaseSensitive(json, "txrefs");
    if (!cJSON_IsArray(txrefs)) {
        fprintf(stderr, "No UTXOs found\n");
        cJSON_Delete(json);
        return;
    }

    *num_utxos = cJSON_GetArraySize(txrefs);
    *txid = (char**)malloc(*num_utxos * sizeof(char*));
    *vout = (int32_t*)malloc(*num_utxos * sizeof(int32_t));
    int32_t i = 0;
    cJSON *utxo;
    cJSON_ArrayForEach(utxo, txrefs) {
        (*txid)[i] = (char*)malloc(65 * sizeof(char)); // Allocate memory for each txid string
        strcpy((*txid)[i], cJSON_GetObjectItemCaseSensitive(utxo, "tx_hash")->valuestring);
        (*vout)[i] = cJSON_GetObjectItemCaseSensitive(utxo, "tx_output_n")->valueint;
        // printf("txid: %s\n", cJSON_GetObjectItemCaseSensitive(utxo, "tx_hash")->valuestring);
        // printf("vout: %d\n", cJSON_GetObjectItemCaseSensitive(utxo, "tx_output_n")->valueint);
        i++;
    }

    cJSON_Delete(json);
}

void free_all_utxo_info(char** txid, int32_t* vout, int32_t num_utxos) {
    for (int32_t i = 0; i < num_utxos; i++) {
        free(txid[i]);
    }
    free(txid);
    free(vout);
}

Tx *fetch(uint8_t *tx_id, bool testnet) {
    char url[MAX_URL_LENGTH];
    snprintf(url, sizeof(url), "%stx/", get_url(testnet));
    int32_t start = strlen(url);
    for (int32_t i = 0; i < 32; i++) {
        snprintf(url + start + 2 * i, 3, "%02x", tx_id[i]);
    }
    snprintf(url + start + 64, 5, "%s", "/hex");
    //printf("URL: %s\n", url);
    char response[100000] = {0};
    http_get(url, response);
    //printf("Response: %s\n", response);
    uint64_t raw_length = strlen(response) / 2;
    uint8_t raw[raw_length];
    memset(raw, 0, raw_length);
    hex_string_to_byte_array(response, raw);

    // Assuming not a segwit transaction
    Tx* tx = Tx_parse(raw, testnet);

    uint8_t tx_id_result[32];
    Tx_id(tx, tx_id_result);
    // for (int i = 0; i < 32; i++) {
    //     printf("%02x", tx_id_result[i]);
    // }
    // printf("\n");
    // for (int i = 0; i < 32; i++) {
    //     printf("%02x", tx_id[i]);
    // }
    // printf("\n");
    if (memcmp(tx_id_result, tx_id, 32) != 0) {
        fprintf(stderr, "Tx ID mismatch\n");
        return NULL;
    }

    return tx;
}

void broadcast_transaction(const char *tx_hex) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    char post_fields[1024];

    // Create the POST fields with the transaction hex
    snprintf(post_fields, sizeof(post_fields), "{\"tx\":\"%s\"}", tx_hex);

    // Initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set the URL for BlockCypher API
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.blockcypher.com/v1/btc/main/txs/push");

        // Set the headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the POST fields
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);

        // Enable verbose output for debugging
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("Transaction broadcasted successfully.\n");
        }

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    } else {
        fprintf(stderr, "curl_easy_init() failed\n");
    }

    curl_global_cleanup();
}