#include "configOTASecure.h"

bool Config::is_signature_valid(const uint8_t *pub_key, const size_t pk_len, const uint8_t *content, const size_t contentLen, const uint8_t *signature)
{
    RSA_PKI rsa(pub_key, pk_len);
    return rsa.verify_signature(content, contentLen, signature);
}

int Config::get_img(uint8_t *signature, uint8_t *content, const char *config_url)
{
    int result;

    HTTPClient http;
    int imageLength = HTTP::get_length(http, config_url);

    int contentLength = imageLength - SIGN_LEN;
    if (contentLength > 0 && contentLength <= max_content_size)
    {
        http.getStream().readBytes(signature, SIGN_LEN);
        http.getStream().readBytes(content, contentLength);
        result = contentLength; // OK
    }
    else if (contentLength > (int)max_content_size)
    {
        log_i("config.json's size Error: Content Length > %d", max_content_size);
        result = 0; // Exceeded Content Length error
    }
    else
    {
        log_i("config.img's size Error: the image's length must > %d", SIGN_LEN);
        result = -1; // HTTP GET error or imageLength <= SIGN_LEN
    }

    http.end();
    return result;
}

const char *Config::translate_err(ConfigErr errCode)
{
    const char *errMsg[]{
        "No Error",
        "Invalid Signature",
        "Invalid Sematic Versioning",
        "Not a JSON Object",
        "HTTP GET config.img Failed",
        "\"version\" Not Found"
        "JSON Deserialization Failed",
    };
    return errMsg[(int)errCode];
}

// check the remote repository & perform updates if needed
ConfigErr Config::check_update(Device_Params &device)
{
    uint8_t signature[SIGN_LEN];
    uint8_t content[max_content_size];
    Config_Params configParams;

    int contentLength = get_img(signature, content, configParams.url);
    if (contentLength <= 0)
    {
        return ConfigErr::HttpGetErr;
    }
    if (!is_signature_valid(configParams.public_key, configParams.pubkey_size, content, contentLength, signature))
    {
        return ConfigErr::InvalidSign;
    }

    DynamicJsonDocument doc(json_doc_capacity);
    DeserializationError jsonError = deserializeJson(doc, content, max_content_size);
    if (jsonError)
    {
        log_i("deserializeJson() failed: %s", jsonError.c_str());
        return ConfigErr::DeserializeErr; // deserialize error
    }
    if (!doc.is<JsonObject>())
    {
        log_i("Invalid JSON format: config.json's root is not an object");
        return ConfigErr::InvalidJsonFormat;
    }

    if (!doc["device"].is<JsonObject>() || !doc["config"].is<JsonObject>() || !doc["firmware"].is<JsonObject>())
    {
        log_i("Invalid JSON format: There must be \"config\" and \"device\" and \"firmware\" objects in config.json file.");
        return ConfigErr::InvalidJsonFormat;
    }

    const char *json_cf_ver = doc["config"]["version"];
    const char *json_fw_ver = doc["firmware"]["version"];
    if (json_cf_ver == nullptr || json_fw_ver == nullptr)
    {
        return ConfigErr::NoVersion;
    }

    Firmware_Params firmwareParams;
    Semver configSemver(configParams.version, json_cf_ver);
    if (configSemver.is_newer_version())
    {
        log_i("Found a new config version: %s --> Update params.", json_cf_ver);
        configParams.update(doc["config"]);
        device.update(doc["device"]);
        firmwareParams.update_pubkey(doc["firmware"]);
    }
    else
    {
        log_i("No newer config version");
    }

    Semver firmwareSemver(firmwareParams.version, json_fw_ver);
    if (firmwareSemver.is_newer_version())
    {
        if (update_firmware(doc["firmware"]["url"], firmwareParams))
        { // true --> succeeded --> :
            log_i("FW Update successfully completed. Rebooting.");
            firmwareParams.update_version(doc["firmware"]);
            ESP.restart();
        }
    }

    return ConfigErr::NoErr;
}

bool Config::is_fw_signature_valid(const Firmware_Params &fw_params, const int fw_len, const uint8_t *signature)
{
    RSA_PKI rsa(fw_params.public_key, fw_params.pubkey_size);
    const esp_partition_t *next_partition = esp_ota_get_next_update_partition(NULL);
    return rsa.verify_signature(next_partition, fw_len, signature);
}

bool Config::update_firmware(const char *url, const Firmware_Params &fw_params)
{
    bool success = true;
    uint8_t signature[SIGN_LEN];

    HTTPClient http;
    int img_len = HTTP::get_length(http, url);
    int fw_len = img_len - SIGN_LEN;

    if (fw_len <= 0)
    {
        log_i("firmware.img's size Error: Content Length must > %d", SIGN_LEN);
        http.end();
        return false;
    }

    if (!Update.begin(fw_len))
    { // Not enough space to begin OTA
        log_i("Firmware's length, %d bytes, exceeds the OTA space!", fw_len);
        http.end();
        return false;
    }

    http.getStream().readBytes(signature, SIGN_LEN);
    log_i("Writting a newer firmware version into Flash ... (wait 2 - 5 mins)");
    size_t written = Update.writeStream(http.getStream());

    if (written == fw_len)
    {
        log_i("Written: %d successfully.", written);
    }
    else
    {
        log_i("Written only: %d/%d. Retry?", written, fw_len);
    }

    if (Update.end())
    {
        log_i("Signature checking ...");
        if (!is_fw_signature_valid(fw_params, fw_len, signature))
        {
            log_i("... failed!");
            // --> keep this_partition for next boot AND disable next_partition by erasing some bytes:
            const esp_partition_t *this_partition = esp_ota_get_running_partition();
            esp_ota_set_boot_partition(this_partition);
            const esp_partition_t *next_partition = esp_ota_get_next_update_partition(NULL);
            ESP.partitionEraseRange(next_partition, 0, ENCRYPTED_BLOCK_SIZE);
            success = false;
        }
        else
        {
            log_i("... succeeded!");
        }
    }
    else
    {
        log_e("Update Error: #%d", Update.getError());
        success = false;
    }

    http.end();
    return success;
}
