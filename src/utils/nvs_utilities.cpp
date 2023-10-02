#include "nvs_utilities.h"

#include <Preferences.h>
Preferences nvs_kv;

namespace NVS
{
    constexpr bool RW_MODE = false;
    constexpr bool RO_MODE = true;

    // init a new key-value OR get the value if existed
    int init_string(const char *nvs_namespace, const char *key, String &value)
    {
        int result;
        nvs_kv.begin(nvs_namespace, RW_MODE);
        if (!nvs_kv.isKey(key)) // non-existence key --> write default (the pre-existing value)
        {
            nvs_kv.putString(key, value);
            result = 0;
        }
        else
        {
            value = nvs_kv.getString(key);
            result = 1;
        }
        nvs_kv.end();
        return result;
    }

    // init a new key-value OR get the value if existed
    int init_string(const char *nvs_namespace, const char *key, char *value, const size_t max_val_len)
    {
        int result;
        nvs_kv.begin(nvs_namespace, RW_MODE);
        if (!nvs_kv.isKey(key)) // non-existence key --> write default (the pre-existing value)
        {
            nvs_kv.putString(key, value);
            result = 0;
        }
        else
        {
            nvs_kv.getString(key, value, max_val_len);
            result = 1;
        }
        nvs_kv.end();
        return result;
    }

    // init a new key-value OR get the value if existed
    size_t init_bytes(const char *nvs_namespace, const char *key, byte *buf, const size_t len, const size_t max_size)
    {
        size_t result;
        nvs_kv.begin(nvs_namespace, RW_MODE);
        if (!nvs_kv.isKey(key)) // non-existence key --> write default (the pre-existing buf's content!)
        {
            result = nvs_kv.putBytes(key, buf, len);
        }
        else
        {
            result = nvs_kv.getBytes(key, buf, max_size);
        }
        nvs_kv.end();
        return result;
    }

    // init a new key-value OR get the value if existed
    int init_int(const char *nvs_namespace, const char *key, int &value)
    {
        int result;
        nvs_kv.begin(nvs_namespace, RW_MODE);
        if (!nvs_kv.isKey(key)) // non-existence key --> write default (the pre-existing value)
        {
            nvs_kv.putInt(key, value);
            result = 0;
        }
        else
        {
            value = nvs_kv.getInt(key);
            result = 1;
        }
        nvs_kv.end();
        return result;
    }

    // init a new key-value OR get the value if existed
    int init_float(const char *nvs_namespace, const char *key, float &value)
    {
        int result;
        nvs_kv.begin(nvs_namespace, RW_MODE);
        if (!nvs_kv.isKey(key)) // non-existence key --> write default (the pre-existing value)
        {
            nvs_kv.putFloat(key, value);
            result = 0;
        }
        else
        {
            value = nvs_kv.getFloat(key);
            result = 1;
        }
        nvs_kv.end();
        return result;
    }

    // update with checking change
    int update_float_if_change(const char *nvs_namespace, const char *key, const float &value)
    {
        int result;
        nvs_kv.begin(nvs_namespace, RW_MODE);
        if (value == nvs_kv.getFloat(key)) // the same value --> do-nothing
        {
            result = 0;
        }
        else
        {
            nvs_kv.putFloat(key, value);
            result = 1; // updated
        }
        nvs_kv.end();
        return result;
    }

    // update with checking change
    int update_int_if_change(const char *nvs_namespace, const char *key, const int &value)
    {
        int result;
        nvs_kv.begin(nvs_namespace, RW_MODE);
        if (value == nvs_kv.getInt(key)) // the same value
        {
            result = 0; // do nothing
        }
        else
        {
            nvs_kv.putInt(key, value);
            result = 1; // updated
        }
        nvs_kv.end();
        return result;
    }

    // update without checking-change (you should check `is_change?` before calling this function)
    void update_float(const char *nvs_namespace, const char *key, const float &value)
    {
        nvs_kv.begin(nvs_namespace, RW_MODE);
        nvs_kv.putFloat(key, value);
        nvs_kv.end();
    }

    // update without checking-change (you should check `is_change?` before calling this function)
    void update_int(const char *nvs_namespace, const char *key, const int &value)
    {
        nvs_kv.begin(nvs_namespace, RW_MODE);
        nvs_kv.putInt(key, value);
        nvs_kv.end();
    }

    // update without checking-change (you should check `is_change?` before calling this function)
    void update_string(const char *nvs_namespace, const char *key, const String &value)
    { // always update (check is_change? before calling this function)
        nvs_kv.begin(nvs_namespace, RW_MODE);
        nvs_kv.putString(key, value);
        nvs_kv.end();
    }

    // update without checking-change (you should check `is_change?` before calling this function)
    void update_string(const char *nvs_namespace, const char *key, const char *value)
    {
        nvs_kv.begin(nvs_namespace, RW_MODE);
        nvs_kv.putString(key, value);
        nvs_kv.end();
    }

    // update without checking-change (you should check `is_change?` before calling this function)
    void update_bytes(const char *nvs_namespace, const char *key, const byte *buf, const size_t len)
    { // always update (check is_change? before calling this function)
        nvs_kv.begin(nvs_namespace, RW_MODE);
        nvs_kv.putBytes(key, buf, len);
        nvs_kv.end();
    }
}
