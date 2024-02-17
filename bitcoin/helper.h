#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#include "ecc/s256field.h"

S256Field* hash_to_s256field(const unsigned char* data, size_t data_len);

#endif //HELPER_H