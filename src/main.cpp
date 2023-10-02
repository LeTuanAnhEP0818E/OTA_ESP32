/*
  Secured Over The Air Config:
  - using semver.c
  - using arduinojson6 with the String container
  - using Periodic_Timer(duration, callback_function);
*/

/*
  Note:
  - Tách xác nhận chữ ký số thành hàm tiện ích (rsa_pki.h)
  - ... đầu vào: con trỏ chữ ký, con trỏ khóa PK, con trỏ tới dữ liệu
  - ... Sử dụng function overloading (dựa trên function signature)
  - Dự án khác: sử dụng semver.c --> so sánh ver trong thông điệp với version hiện tại của config, firmware ...
  - Main program: tải file config (size <= 2048) lưu vào mảng động/tĩnh
  - Kiểm tra chữ ký số (tải file pub.key)
  - (version OK & sign OK) ==> Next steps: Cập nhận firmware; Cập nhật thông số cấu hình thiết bị (device config) vào NVS ;
    ...
    - https://github.com/h2non/semver.c
    - https://arduinojson.org/v6/example/config/
    - https://arduinojson.org/v6/assistant/#/step4

  About github delay from new content was pushed:
    - ~1m45s for config.json
    - ~3min05s for config.sign
    - github may use client's IP address to prevent frequently updating (DDOS preventation!)
*/
#include <Arduino.h>
#include "wifi_config.h"
#include "PeriodicTimer.h"
#include "configOTASecure.h"

Device_Params device;
Config config;
void check_config();
PeriodicTimer timer(check_config);

void check_config()
{
    ConfigErr err = config.check_update(device);
    if (err != ConfigErr::NoErr)
    {
        Serial.println(config.translate_err(err));
    }
}

void setup()
{
    Serial.begin(115200);
    setup_wifi(1);

    Config_Params configParams;
    Serial.printf("Config version: %s\n", configParams.version);

    Firmware_Params firmwareParams;
    Serial.printf("Fimrwave version: %s\n", firmwareParams.version);
    Serial.printf("device's checking_interval: %d\n", device.checking_interval);
}

void loop()
{
    // put your main code here, to run repeatedly:
    delay(10); // this speeds up the simulation
    timer.loop(device.checking_interval);
}
