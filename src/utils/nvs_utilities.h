#pragma once
#include <Arduino.h>

namespace NVS
{
    // init a new key-value OR get the value if existed
    int init_string(const char *nvs_namespace, const char *key, String &value);

    // init a new key-value OR get the value if existed
    int init_string(const char *nvs_namespace, const char *key, char *value, const size_t max_val_len);

    // init a new key-value OR get the value if existed
    size_t init_bytes(const char *nvs_namespace, const char *key, byte *buf, const size_t len, const size_t max_size);

    // init a new key-value OR get the value if existed
    int init_int(const char *nvs_namespace, const char *key, int &value);

    // init a new key-value OR get the value if existed
    int init_float(const char *nvs_namespace, const char *key, float &value);

    // update with checking change
    int update_float_if_change(const char *nvs_namespace, const char *key, const float &value);

    // update with checking change
    int update_int_if_change(const char *nvs_namespace, const char *key, const int &value);

    // update without checking-change (you should check `is_change?` before calling this function)
    void update_float(const char *nvs_namespace, const char *key, const float &value);

    // update without checking-change (you should check `is_change?` before calling this function)
    void update_int(const char *nvs_namespace, const char *key, const int &value);

    // update without checking-change (you should check `is_change?` before calling this function)
    void update_string(const char *nvs_namespace, const char *key, const String &value);

    // update without checking-change (you should check `is_change?` before calling this function)
    void update_string(const char *nvs_namespace, const char *key, const char *value);

    // update without checking-change (you should check `is_change?` before calling this function)
    void update_bytes(const char *nvs_namespace, const char *key, const byte *buf, const size_t len);
}
