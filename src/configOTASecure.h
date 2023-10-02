/*
Structs and Classes for secured OTA configuration and firmware update using RSA public key signatures.
Note:
- place this header in the very fist of your main.cpp
- config default device's parameters in this file before compile.
*/

#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <nvs_flash.h>
#include <Update.h>
#include <esp_ota_ops.h>

#include "rsa_pub_key.h"

#include "utils/http_utilities.h"
#include "utils/Semver.hpp"
#include "utils/rsa_pki.h"
#include "utils/nvs_utilities.h"

namespace
{
    constexpr const char *device_type = "ch4_generator";

    constexpr const char *default_conf_version = "0.0.1";
    constexpr const char *default_firm_version = "0.0.5";
    constexpr const char *default_conf_url = "https://raw.githubusercontent.com/tuan-karma/fota_firmware_test/main/config.img";
    constexpr const char *default_firm_url = "https://raw.githubusercontent.com/tuan-karma/fota_firmware_test/main/m5stack/firmware.img";

    constexpr const size_t max_version_size = 64;
    constexpr const size_t max_url_size = 256;
    constexpr const size_t max_content_size = 1024U; // the config.json size limit (in bytes)
    constexpr const size_t json_doc_capacity = 1024;
    constexpr const size_t max_pubkey_size = 832U;
    constexpr const uint8_t max_catalog_size = 32U;
    constexpr const size_t SIGN_LEN = 512U;
}

// The device's parameters: should be a global object, e.g. `Device_Params device;`
struct Device_Params
{
    float ch4_factor = 25.5;
    float power_factor = 12.25;
    int checking_interval = 5; // seconds
    Device_Params()
    {
        nvs_flash_init(); // it must be called before any Pereferences::begin()
        NVS::init_float("device", "ch4_factor", ch4_factor);
        NVS::init_float("device", "power_factor", power_factor);
        NVS::init_int("device", "checking_interv", checking_interval);
    }

    void update(const JsonObject &device_obj)
    {
        float new_ch4_factor = device_obj["ch4_factor"];
        if (new_ch4_factor != 0.0 && new_ch4_factor != ch4_factor)
        {
            ch4_factor = new_ch4_factor;
            NVS::update_float("device", "ch4_factor", ch4_factor);
        }

        float new_power_factor = device_obj["power_factor"];
        if (new_power_factor != 0.0 && new_power_factor != power_factor)
        {
            power_factor = new_power_factor;
            NVS::update_float("device", "power_factor", power_factor);
        }

        int new_checking_interval = device_obj["checking_interval"];
        if (new_checking_interval != 0 && new_checking_interval != checking_interval)
        {
            checking_interval = new_checking_interval;
            NVS::update_int("device", "checking_interv", checking_interval);
        }
    }
};

// This struct hold config's params (corresponding to the "config" obj in config.json)
struct Config_Params
{
    char version[max_version_size];
    char url[max_url_size];
    uint8_t public_key[max_pubkey_size];
    size_t pubkey_size{max_pubkey_size}; // include the null-terminator

    // Initialize the config's parameters from default constants or get them from NVS if existed.
    Config_Params()
    {
        strlcpy(version, default_conf_version, max_version_size);
        strlcpy(url, default_conf_url, max_url_size);

        if(NVS::init_string("config", "version", version, max_version_size) == 0)
        { // no "version" key --> empty NVS --> init using the default rsa_pub_key
            pubkey_size = strlcpy((char *)public_key, (char *)rsa_pub_key, max_pubkey_size) + 1;
        }
        pubkey_size = NVS::init_bytes("config", "public_key", public_key, pubkey_size, max_pubkey_size);

        NVS::init_string("config", "url", url, max_url_size);
    }

    // Need to check is_newer_version()? before this update
    void update(const JsonObject &config_obj)
    {
        strlcpy(version, config_obj["version"], max_version_size);
        NVS::update_string("config", "version", version);

        if (config_obj["url_change?"])
        {
            strlcpy(url, config_obj["url"], max_url_size);
            NVS::update_string("config", "url", url);
        }

        if (config_obj["public_key_change?"])
        {
            HTTPClient http;
            int content_length = HTTP::get_length(http, config_obj["public_key_url"]);
            if (content_length < 0)
            {
                log_e("HTTP GET error code: %d", -content_length);
                return;
            }
            if (content_length >= max_pubkey_size)
            {
                log_e("The %s_key.pub's length > %d", "config", max_pubkey_size - 1);
                return;
            }
            http.getStream().readBytes(public_key, content_length);
            http.end();

            public_key[content_length] = '\0'; // null terminated
            NVS::update_bytes("config", "public_key", public_key, content_length + 1);
            log_i("The %s's pk updated!", "config");
        }
    }
};

// This struct hold firmware's params (corresponding to the "firmware" obj in config.json)
struct Firmware_Params
{
    char version[max_version_size];
    uint8_t public_key[max_pubkey_size]; // the null-terminator included
    size_t pubkey_size{max_pubkey_size}; // include the null-terminator

    // Initialize config's or firmware's parameters from default constants or get them from NVS if existed.
    Firmware_Params()
    {
        strlcpy(version, default_firm_version, max_version_size);

        if (NVS::init_string("firmware", "version", version, max_version_size) == 0)
        { // no "version" key --> empty NVS --> copy the default rsa_pub_key
            pubkey_size = strlcpy((char *)public_key, (char *)rsa_pub_key, max_pubkey_size) + 1;
        }
        pubkey_size = NVS::init_bytes("firmware", "public_key", public_key, pubkey_size, max_pubkey_size);
    }

    // Check is_newer_config_version()? before this update --> prevent perpetual pk update, when `"public_key_change?": true`
    void update_pubkey(const JsonObject &firmware_obj)
    {
        if (firmware_obj["public_key_change?"])
        {
            HTTPClient http;
            int content_length = HTTP::get_length(http, firmware_obj["public_key_url"]);
            if (content_length < 0)
            {
                log_e("HTTP GET error code: %d", -content_length);
                return;
            }
            if (content_length >= max_pubkey_size)
            {
                log_e("The %s_key.pub's length > %d", "firmware", max_pubkey_size - 1);
                return;
            }
            http.getStream().readBytes(public_key, content_length);
            http.end();

            public_key[content_length] = '\0'; // null terminated
            NVS::update_bytes("firmware", "public_key", public_key, content_length + 1);
            log_i("The %s's pk updated!", "firmware");
        }
    }

    // Need to check is_newer_version()? before this update
    void update_version(const JsonObject &firmware_obj)
    {
        strlcpy(version, firmware_obj["version"], max_version_size);
        NVS::update_string("firmware", "version", version);
    }
};

enum class ConfigErr
{
    NoErr = 0,
    InvalidSign,
    InvalidSemver,
    InvalidJsonFormat,
    HttpGetErr,
    NoVersion,
    DeserializeErr,
};

/*  - Check is_newer_config_version? --> update config's & device's params
    - Check is_newer_firmware_version? --> update the "firmware's version! & the OTA firmware
    - Verify signatures before every update.
*/
class Config
{
public:
    ConfigErr check_update(Device_Params &device);
    const char *translate_err(ConfigErr errCode);

private:
    bool is_signature_valid(const uint8_t *pub_key, const size_t pk_len, const uint8_t *content, const size_t contentLen, const uint8_t *signature);
    int get_img(uint8_t *signature, uint8_t *content, const char *config_url);
    bool is_fw_signature_valid(const Firmware_Params &fw_params, const int fw_len, const uint8_t *signature);
    bool update_firmware(const char *url, const Firmware_Params &fw_params);
};
