#pragma once
#include <Arduino.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>

// "Infrastructure" to verify RSA public keys
// Construct this object from public key (*pub_key pointer and keyLen)
// It has methods to verify_signature using segmentation wise (from data_stream) or from data_byes
class RSA_PKI
{
public:
    RSA_PKI(const unsigned char *pub_key, const size_t keylen);

    ~RSA_PKI();

    bool is_key_valid();

    bool verify_signature(Stream &data_stream, const int dataLen, const String &signature);
    bool verify_signature(Stream &data_stream, const int dataLen, const uint8_t *signature);
    bool verify_signature(const esp_partition_t *partition, const int dataLen, const uint8_t *signature);
    bool verify_signature(const String &data, const String &signature);
    bool verify_signature(const uint8_t *data, const size_t dataLen, const uint8_t *signature);

private:
    mbedtls_pk_context rsa;
    mbedtls_md_context_t sha;
    bool key_valid = false;
};