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

Tx* Tx_deep_copy(Tx* src) {
    Tx* new_tx = (Tx*)malloc(sizeof(Tx));
    if (!new_tx) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    new_tx->version = src->version;
    new_tx->num_inputs = src->num_inputs;
    new_tx->tx_ins = (TxIn**)malloc(src->num_inputs * sizeof(TxIn*));
    for (unsigned long long i = 0; i < src->num_inputs; i++) {
        new_tx->tx_ins[i] = TxIn_deep_copy(src->tx_ins[i]);
    }
    new_tx->num_outputs = src->num_outputs;
    new_tx->tx_outs = (TxOut**)malloc(src->num_outputs * sizeof(TxOut*));
    for (unsigned long long i = 0; i < src->num_outputs; i++) {
        new_tx->tx_outs[i] = TxOut_deep_copy(src->tx_outs[i]);
    }
    new_tx->locktime = src->locktime;
    new_tx->testnet = src->testnet;
    return new_tx;
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
        unsigned long long tx_in_length = 40;
        unsigned long long script_sig_length = 0;
        for (unsigned long long j = 0; j < tx->tx_ins[i]->script_sig->cmds_len; j++) {
            script_sig_length += tx->tx_ins[i]->script_sig->cmds[j].data_len;
            if (tx->tx_ins[i]->script_sig->cmds[j].data_len > 1) {
                script_sig_length++;
            }
        }
        tx_in_length += script_sig_length;
        if (tx_in_length < 0xfd) {
            tx_in_length++;
        } else if (tx_in_length <= 0xffff) {
            tx_in_length += 4;
        } else if (tx_in_length <= 0xffffffff) {
            tx_in_length += 6;
        } else {
            tx_in_length += 10;
        }
        serial_length += tx_in_length;
    }
    for (unsigned long long i = 0; i < tx->num_outputs; i++) {
        unsigned long long tx_out_length = 8;
        unsigned long long script_pubkey_length = 0;
        for (unsigned long long j = 0; j < tx->tx_outs[i]->script_pubkey->cmds_len; j++) {
            script_pubkey_length += tx->tx_outs[i]->script_pubkey->cmds[j].data_len;
            if (tx->tx_outs[i]->script_pubkey->cmds[j].data_len > 1) {
                script_pubkey_length++;
            }
        }
        tx_out_length += script_pubkey_length;
        if (tx_out_length < 0xfd) {
            tx_out_length++;
        } else if (tx_out_length <= 0xffff) {
            tx_out_length += 4;
        } else if (tx_out_length <= 0xffffffff) {
            tx_out_length += 6;
        } else {
            tx_out_length += 10;
        }
        serial_length += tx_out_length;
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

        // printf("Prev_tx: ");
        // for (int i = 0; i < 32; i++) {
        //     printf("%02x", tx_in->prev_tx[i]);
        // }
        // printf("\n");
        // printf("Prev_index: %d\n", tx_in->prev_index);
        // printf("Script_sig: ");
        // unsigned long long script_sig_length = 0;
        // for (unsigned long long j = 0; j < tx_in->script_sig->cmds_len; j++) {
        //     script_sig_length += tx_in->script_sig->cmds[j].data_len;
        //     if (tx_in->script_sig->cmds[j].data_len > 1) {
        //         script_sig_length++;
        //     }
        // }
        // script_sig_length += 4;
        // unsigned char script_sig_serialized[script_sig_length];
        // script_serialize(tx_in->script_sig, script_sig_serialized);
        // for (int i = 0; i < script_sig_length; i++) {
        //     printf("%02x", script_sig_serialized[i]);
        // }
        // printf("\n");
        // printf("Sequence: %d\n", tx_in->sequence);

        tx_ins[i] = tx_in;
        unsigned long long tx_in_len = read_varint(s + 36);
        //Not sure why I have to do this
        if (tx_in_len < 0xfd) {
            tx_in_len++;
        } else if (tx_in_len <= 0xffff) {
            tx_in_len += 3;
        } else if (tx_in_len <= 0xffffffff) {
            tx_in_len += 5;
        } else {
            tx_in_len += 9;
        }
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
        if (tx_out_len < 0xfd) {
            tx_out_len++;
        } else if (tx_out_len <= 0xffff) {
            tx_out_len += 3;
        } else if (tx_out_len <= 0xffffffff) {
            tx_out_len += 5;
        } else {
            tx_out_len += 9;
        }
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
        if (script_sig_length < 0xfd) {
            script_sig_length++;
        } else if (script_sig_length <= 0xffff) {
            script_sig_length += 4;
        } else if (script_sig_length <= 0xffffffff) {
            script_sig_length += 6;
        } else {
            script_sig_length += 10;
        }
        unsigned long long tx_in_length = 40 + script_sig_length;
        TxIn_serialize(tx->tx_ins[i], result);
        result += tx_in_length;
    }
    encode_varint(result, tx->num_outputs);
    if (tx->num_outputs < 253) {
        result += 1;
    } else if (tx->num_outputs < 65535) {
        result += 4;
    } else if (tx->num_outputs < 4294967295) {
        result += 6;
    } else {
        result += 10;
    }
    for (unsigned long long i = 0; i < tx->num_outputs; i++) {
        unsigned long long script_pubkey_length = 0;
        for (unsigned long long j = 0; j < tx->tx_outs[i]->script_pubkey->cmds_len; j++) {
            script_pubkey_length += tx->tx_outs[i]->script_pubkey->cmds[j].data_len;
            if (tx->tx_outs[i]->script_pubkey->cmds[j].data_len > 1) {
                script_pubkey_length++;
            }
        }
        if (script_pubkey_length < 0xfd) {
            script_pubkey_length++;
        } else if (script_pubkey_length <= 0xffff) {
            script_pubkey_length += 4;
        } else if (script_pubkey_length <= 0xffffffff) {
            script_pubkey_length += 6;
        } else {
            script_pubkey_length += 10;
        }
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

void sig_hash(Tx* tx, unsigned long long input_index, unsigned char* result) {
    printf("Sighash input index: %llu\n", input_index);
    int total_length = 0;
    unsigned char* s = (unsigned char*)malloc(10000);
    int_to_little_endian(tx->version, s, 4);
    s += 4;
    total_length += 4;
    encode_varint(s, tx->num_inputs);
    if (tx->num_inputs < 253) {
        s++;
        total_length++;
    } else if (tx->num_inputs < 65535) {
        s += 3;
        total_length += 3;
    } else if (tx->num_inputs < 4294967295) {
        s += 5;
        total_length += 5;
    } else {
        s += 9;
        total_length += 9;
    }
    for (unsigned long long i = 0; i < tx->num_inputs; i++) {
        if (i == input_index) {
            Script* script_sig = TxIn_script_pubkey(tx->tx_ins[i], tx->testnet);
            script_deep_copy(tx->tx_ins[i]->script_sig, script_sig);
            script_free(script_sig);
            unsigned long long script_sig_length = script_length(tx->tx_ins[i]->script_sig);
            unsigned long long tx_in_length = 40 + script_sig_length;
            TxIn_serialize(tx->tx_ins[i], s);
            s += tx_in_length;
            total_length += tx_in_length;
        } else {
            unsigned char prev_tx[32] = {0};
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
            printf("Prev_index: %d\n", tx->tx_ins[i]->prev_index);
            printf("Sequence: %d\n", tx->tx_ins[i]->sequence);
            int_to_little_endian(tx->tx_ins[i]->sequence, s, 4);
            s += 4;
            total_length += 4;
        }
    }
    s -= total_length;
    printf("S after inputs: ");
    for (int i = 0; i < total_length; i++) {
        printf("%02x", s[i]);
    }
    printf("\n");
    s += total_length;
    encode_varint(s, tx->num_outputs);
    if (tx->num_outputs < 253) {
        s++;
        total_length++;
    } else if (tx->num_outputs < 65535) {
        s += 3;
        total_length += 3;
    } else if (tx->num_outputs < 4294967295) {
        s += 5;
        total_length += 5;
    } else {
        s += 9;
        total_length += 9;
    }
    for (unsigned long long i = 0; i < tx->num_outputs; i++) {
        unsigned long long script_pubkey_length = script_length(tx->tx_outs[i]->script_pubkey);
        unsigned long long tx_out_length = 8 + script_pubkey_length;
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

size_t verify_input(Tx* tx, unsigned long long input_index) {
    Tx* tx_copy = Tx_deep_copy(tx);
    TxIn* tx_in = tx_copy->tx_ins[input_index];
    Script* script_pubkey = TxIn_script_pubkey(tx_in, tx_copy->testnet);
    Script* script_sig = script_init();
    script_deep_copy(script_sig, tx_in->script_sig);
    unsigned char z_raw[32];
    sig_hash(tx_copy, input_index, z_raw);
    char z_str[65] = {0};
    byte_array_to_hex_string(z_raw, 32, z_str);
    mpz_t z_mpz;
    mpz_init_set_str(z_mpz, z_str, 16);
    S256Field* z = S256Field_init(z_mpz);
    Script* combined = script_add(script_sig, script_pubkey);
    unsigned long long combined_length = script_length(combined);
    unsigned char combined_serialized[combined_length];
    script_serialize(combined, combined_serialized);
    size_t result = script_evaluate(combined, z);
    script_free(script_sig);
    script_free(script_pubkey);
    script_free(combined);
    S256Field_free(z);
    Tx_free(tx_copy);
    return result;
}

size_t Tx_verify(Tx* tx) {
    unsigned long long tx_fee = fee(tx, tx->testnet);
    if (tx_fee < 0) {
        return 0;
    }
    for (unsigned long long i = 0; i < tx->num_inputs; i++) {
        if (!verify_input(tx, i)) {
            return 0;
        }
    }
    return 1;
}

size_t sign_input(Tx* tx, unsigned long long input_index, PrivateKey* private_key) {
    unsigned char z_raw[32] = {0};
    sig_hash(tx, input_index, z_raw);
    char z_str[65] = {0};
    byte_array_to_hex_string(z_raw, 32, z_str);
    mpz_t z_mpz;
    mpz_init_set_str(z_mpz, z_str, 16);
    S256Field* z = S256Field_init(z_mpz);
    gmp_printf("Z: %Zd\n", z->num);
    Signature* sig = PrivateKey_sign(private_key, z);
    int der_length = Signature_der_length(sig);
    unsigned char der[der_length + 1];
    Signature_der(sig, der);
    printf("DER: ");
    for (int i = 0; i < der_length; i++) {
        printf("%02x", der[i]);
    }
    printf("\n");
    unsigned char sighash_all[1] = {0};
    int_to_little_endian(SIGHASH_ALL, sighash_all, 1);
    der[der_length] = sighash_all[0];
    printf("SIG: ");
    for (int i = 0; i < der_length + 1; i++) {
        printf("%02x", der[i]);
    }
    printf("\n");
    Command sig_cmd;
    memcpy(sig_cmd.data, der, der_length + 1);
    sig_cmd.data_len = der_length + 1;
    unsigned char sec[33] = {0};
    S256Point_sec_compressed(private_key->point, sec);
    printf("SEC: ");
    for (int i = 0; i < 33; i++) {
        printf("%02x", sec[i]);
    }
    printf("\n");
    Command sec_cmd;
    memcpy(sec_cmd.data, sec, 33);
    sec_cmd.data_len = 33;
    Command cmds[2] = {sig_cmd, sec_cmd};
    Script* script_sig = script_init();
    script_set_cmds(script_sig, cmds, 2);
    unsigned char script_sig_serialized[script_length(script_sig)];
    script_serialize(script_sig, script_sig_serialized);
    printf("Script: ");
    for (int i = 0; i < script_length(script_sig); i++) {
        printf("%02x", script_sig_serialized[i]);
    }
    printf("\n");
    script_deep_copy(tx->tx_ins[input_index]->script_sig, script_sig);
    script_free(script_sig);
    S256Field_free(z);
    Signature_free(sig);
    size_t result = verify_input(tx, input_index);
    return result;
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

TxIn* TxIn_deep_copy(TxIn* src) {
    TxIn* new_tx_in = (TxIn*)malloc(sizeof(TxIn));
    if (!new_tx_in) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    memcpy(new_tx_in->prev_tx, src->prev_tx, 32);
    new_tx_in->prev_index = src->prev_index;
    new_tx_in->script_sig = script_init();
    script_deep_copy(new_tx_in->script_sig, src->script_sig);
    new_tx_in->sequence = src->sequence;
    return new_tx_in;
}

unsigned long long TxIn_length(TxIn* tx_in) {
    unsigned long long script_sig_length = 0;
    for (unsigned long long i = 0; i < tx_in->script_sig->cmds_len; i++) {
        script_sig_length += tx_in->script_sig->cmds[i].data_len;
        if (tx_in->script_sig->cmds[i].data_len > 1) {
            script_sig_length++;
        }
    }
    if (script_sig_length < 0xfd) {
        script_sig_length++;
    } else if (script_sig_length <= 0xffff) {
        script_sig_length += 3;
    } else if (script_sig_length <= 0xffffffff) {
        script_sig_length += 5;
    } else {
        script_sig_length += 9;
    }
    return 40 + script_sig_length;
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
    if (script_sig_len < 0xfd) {
        script_sig_len++;
    } else if (script_sig_len <= 0xffff) {
        script_sig_len += 3;
    } else if (script_sig_len <= 0xffffffff) {
        script_sig_len += 5;
    } else {
        script_sig_len += 9;
    }
    unsigned char sequence_raw[4];
    memcpy(prev_tx, s, 32);
    little_endian_to_big_endian(prev_tx, 32);
    memcpy(prev_index_raw, s + 32, 4);
    memcpy(sequence_raw, s + 36 + script_sig_len, 4);
    int prev_index = little_endian_to_int(prev_index_raw, 4);
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
    if (script_sig_len < 0xfd) {
        script_sig_len++;
    } else if (script_sig_len <= 0xffff) {
        script_sig_len += 4;
    } else if (script_sig_len <= 0xffffffff) {
        script_sig_len += 6;
    } else {
        script_sig_len += 10;
    }
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

Script* TxIn_script_pubkey(TxIn* txin, size_t testnet) {
    Tx* tx = fetch_tx(txin, testnet);
    Script* result = script_init();
    script_deep_copy(result, tx->tx_outs[txin->prev_index]->script_pubkey);
    Tx_free(tx);
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

TxOut* TxOut_deep_copy(TxOut* src) {
    TxOut* new_tx_out = (TxOut*)malloc(sizeof(TxOut));
    if (!new_tx_out) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    new_tx_out->amount = src->amount;
    new_tx_out->script_pubkey = script_init();
    script_deep_copy(new_tx_out->script_pubkey, src->script_pubkey);
    return new_tx_out;
}

unsigned long long TxOut_length(TxOut* tx_out) {
    unsigned long long script_pubkey_length = 0;
    for (unsigned long long i = 0; i < tx_out->script_pubkey->cmds_len; i++) {
        script_pubkey_length += tx_out->script_pubkey->cmds[i].data_len;
        if (tx_out->script_pubkey->cmds[i].data_len > 1) {
            script_pubkey_length++;
        }
    }
    if (script_pubkey_length < 0xfd) {
        script_pubkey_length++;
    } else if (script_pubkey_length <= 0xffff) {
        script_pubkey_length += 3;
    } else if (script_pubkey_length <= 0xffffffff) {
        script_pubkey_length += 5;
    } else {
        script_pubkey_length += 9;
    }
    return 8 + script_pubkey_length;
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
    unsigned long long script_pubkey_len = read_varint(s + 8);
    if (script_pubkey_len == 0xfd) {
        script_pubkey_len += 3;
    } else if (script_pubkey_len == 0xfe) {
        script_pubkey_len += 5;
    } else if (script_pubkey_len == 0xff) {
        script_pubkey_len += 9;
    } else {
        script_pubkey_len++;
    }
    Script* script_pubkey = script_parse(s + 8);
    // unsigned char script_pubkey_serialized[script_pubkey_len];
    // script_serialize(script_pubkey, script_pubkey_serialized);
    // printf("Script_pubkey: ");
    // for (int i = 0; i < script_pubkey_len; i++) {
    //     printf("%02x", script_pubkey_serialized[i]);
    // }
    // printf("\n");
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
    // printf("URL: %s\n", url);
    char response[10000] = {0};
    http_get(url, response);
    // printf("Response: %s\n", response);
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