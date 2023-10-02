#include "rsa_pki.h"

#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <memory>

// "Infrastructure" to verify RSA public keys
// Construct this object from public key (*pub_key pointer and keyLen)
// It has methods to verify_signature using segmentation wise (from data_stream) or from data_byes
RSA_PKI::RSA_PKI(const unsigned char *pub_key, const size_t keylen)
{
    mbedtls_pk_init(&rsa);
    if (!mbedtls_pk_parse_public_key(&rsa, pub_key, keylen) && mbedtls_pk_can_do(&rsa, MBEDTLS_PK_RSA))
    {
        key_valid = true;
    }
    else
    {
        log_i("Not a valid RSA public key!");
        key_valid = false;
    }

    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    mbedtls_md_init(&sha);
    mbedtls_md_setup(&sha, md_info, 0);
    mbedtls_md_starts(&sha);
}

RSA_PKI::~RSA_PKI()
{
    mbedtls_md_free(&sha);
    mbedtls_pk_free(&rsa);
}

bool RSA_PKI::is_key_valid()
{
    return key_valid;
}

// Segment-wise signature verification of a data stream
bool RSA_PKI::verify_signature(Stream &data_stream, const int dataLen, const String &signature)
{
    return verify_signature(data_stream, dataLen, (uint8_t *)signature.c_str());
}

// Segment-wise signature verification of a data stream
bool RSA_PKI::verify_signature(Stream &data_stream, const int dataLen, const uint8_t *signature)
{
    if (!key_valid)
    {
        log_i("Invalid RSA public key!");
        return false;
    }

    constexpr const int bufferSize = SPI_FLASH_SEC_SIZE;

    int remainBytes = dataLen;
    std::unique_ptr<byte[]> _buffer{new byte[bufferSize]};

    if (_buffer.get() == nullptr)
    {
        log_e("Heap allocation failed");
        return false;
    }

    // step-wise hashing data (SHA256) from data_stream:
    while (remainBytes > 0)
    {
        int bytesToRead = (remainBytes < bufferSize) ? remainBytes : bufferSize;
        if (data_stream.readBytes(_buffer.get(), bytesToRead))
        {
            mbedtls_md_update(&sha, _buffer.get(), bytesToRead); // hashing "message --> digest" using SHA256; step-wise update
            remainBytes -= bytesToRead;
        }
        else
        {
            log_i("Failed to read the data_stream");
            return false;
        }
    }
    byte hash[32];
    mbedtls_md_finish(&sha, hash);

    return !mbedtls_pk_verify(&rsa, MBEDTLS_MD_SHA256,
                              hash, 32,
                              signature, 512);
}

// Segment-wise signature verification of a esp partition (OTA data)
bool RSA_PKI::verify_signature(const esp_partition_t *partition, const int dataLen, const uint8_t *signature)
{
    if (!key_valid)
    {
        log_i("Invalid RSA public key!");
        return false;
    }

    constexpr const int bufferSize = SPI_FLASH_SEC_SIZE;

    int remainBytes = dataLen;
    std::unique_ptr<byte[]> _buffer{new byte[bufferSize]};

    if (_buffer.get() == nullptr)
    {
        log_e("Heap allocation failed");
        return false;
    }

    // step-wise hashing data (SHA256) from data_stream:
    uint32_t offsetPos = 0; // offset position for partitionRead(...)
    while (remainBytes > 0)
    {
        int bytesToRead = (remainBytes < bufferSize) ? remainBytes : bufferSize;
        if (ESP.partitionRead(partition, offsetPos, (uint32_t *)_buffer.get(), bytesToRead))
        {
            mbedtls_md_update(&sha, _buffer.get(), bytesToRead); // hashing "message --> digest" using SHA256; step-wise update
            remainBytes -= bytesToRead;
            offsetPos += bytesToRead;
        }
        else
        {
            log_i("ESP.partitionRead failed");
            return false;
        }
    }
    byte output_hash[32];
    mbedtls_md_finish(&sha, output_hash);

    return !mbedtls_pk_verify(&rsa, MBEDTLS_MD_SHA256,
                              output_hash, 32,
                              signature, 512);
}

bool RSA_PKI::verify_signature(const String &data, const String &signature)
{
    return verify_signature((uint8_t *)data.c_str(), data.length(), (uint8_t *)signature.c_str());
}

bool RSA_PKI::verify_signature(const uint8_t *data, const size_t dataLen, const uint8_t *signature)
{
    if (!key_valid)
    {
        log_i("Invalid RSA public key!");
        return false;
    }

    mbedtls_md_update(&sha, data, dataLen); // hashing "message --> digest" using SHA256; step-wise update
    byte hash[32];
    mbedtls_md_finish(&sha, hash);

    return !mbedtls_pk_verify(&rsa, MBEDTLS_MD_SHA256,
                              hash, 32,
                              signature, 512);
}
