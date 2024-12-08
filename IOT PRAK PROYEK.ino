#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         22           // Configurable, see typical pin layout above
#define SS_PIN          21           // Configurable, see typical pin layout above
#define BUZZER_PIN      2            // Pin untuk buzzer
#define SERVO_PIN       4            // Pin untuk buzzer

LiquidCrystal_I2C lcd(0x27, 16, 2);

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
String UID;

DynamicJsonDocument doc(1024);

Servo servo1;

TaskHandle_t httpTask;
TaskHandle_t rfidTask;
TaskHandle_t keypadTask;

const char ssid[] = "Studio Room";
const char password[] = "johncena12";

// Sertifikat SSL
const char *server_cert = R"(-----BEGIN CERTIFICATE-----
MIIEdTCCA12gAwIBAgIJAKcOSkw0grd/MA0GCSqGSIb3DQEBCwUAMGgxCzAJBgNV
BAYTAlVTMSUwIwYDVQQKExxTdGFyZmllbGQgVGVjaG5vbG9naWVzLCBJbmMuMTIw
MAYDVQQLEylTdGFyZmllbGQgQ2xhc3MgMiBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0
eTAeFw0wOTA5MDIwMDAwMDBaFw0zNDA2MjgxNzM5MTZaMIGYMQswCQYDVQQGEwJV
UzEQMA4GA1UECBMHQXJpem9uYTETMBEGA1UEBxMKU2NvdHRzZGFsZTElMCMGA1UE
ChMcU3RhcmZpZWxkIFRlY2hub2xvZ2llcywgSW5jLjE7MDkGA1UEAxMyU3RhcmZp
ZWxkIFNlcnZpY2VzIFJvb3QgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IC0gRzIwggEi
MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDVDDrEKvlO4vW+GZdfjohTsR8/
y8+fIBNtKTrID30892t2OGPZNmCom15cAICyL1l/9of5JUOG52kbUpqQ4XHj2C0N
Tm/2yEnZtvMaVq4rtnQU68/7JuMauh2WLmo7WJSJR1b/JaCTcFOD2oR0FMNnngRo
Ot+OQFodSk7PQ5E751bWAHDLUu57fa4657wx+UX2wmDPE1kCK4DMNEffud6QZW0C
zyyRpqbn3oUYSXxmTqM6bam17jQuug0DuDPfR+uxa40l2ZvOgdFFRjKWcIfeAg5J
Q4W2bHO7ZOphQazJ1FTfhy/HIrImzJ9ZVGif/L4qL8RVHHVAYBeFAlU5i38FAgMB
AAGjgfAwge0wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0O
BBYEFJxfAN+qAdcwKziIorhtSpzyEZGDMB8GA1UdIwQYMBaAFL9ft9HO3R+G9FtV
rNzXEMIOqYjnME8GCCsGAQUFBwEBBEMwQTAcBggrBgEFBQcwAYYQaHR0cDovL28u
c3MyLnVzLzAhBggrBgEFBQcwAoYVaHR0cDovL3guc3MyLnVzL3guY2VyMCYGA1Ud
HwQfMB0wG6AZoBeGFWh0dHA6Ly9zLnNzMi51cy9yLmNybDARBgNVHSAECjAIMAYG
BFUdIAAwDQYJKoZIhvcNAQELBQADggEBACMd44pXyn3pF3lM8R5V/cxTbj5HD9/G
VfKyBDbtgB9TxF00KGu+x1X8Z+rLP3+QsjPNG1gQggL4+C/1E2DUBc7xgQjB3ad1
l08YuW3e95ORCLp+QCztweq7dp4zBncdDQh/U90bZKuCJ/Fp1U1ervShw3WnWEQt
8jxwmKy6abaVd38PMV4s/KCHOkdp8Hlf9BRUpJVeEXgSYCfOn8J3/yNTd126/+pZ
59vPr5KW7ySaNRB6nJHGDn2Z9j8Z3/VyVOEVqQdZe4O/Ui5GjLIAZHYcSNPYeehu
VsyuLAOQ1xk4meTKCRlb/weWsKh/NEnfVqn3sF/tM+2MR7cwA130A4w=
-----END CERTIFICATE-----)";

const char *client_cert = R"(-----BEGIN CERTIFICATE-----
MIIDazCCAlOgAwIBAgIUKKiAFOqVEulwkqj1poHRwfgm3howDQYJKoZIhvcNAQEL
BQAwRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM
GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0yNDEyMDYxNDM4NDVaFw0yNTEy
MDYxNDM4NDVaMEUxCzAJBgNVBAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEw
HwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwggEiMA0GCSqGSIb3DQEB
AQUAA4IBDwAwggEKAoIBAQDOOFUUud8dsFYjq0aTcD7geyljtzKvz/nbUFKn9fHv
wMZvO6HOU8xLI/LJe6c/d57IXIGmtb2//ni5tL2TgDcmhwgDSvtFKA94OHzQjrcb
So0AHxb0ds1xM+ND1xaDWfyn0Nj4qWBCghvioTyNTA/lgGPjmRStejohnUKBi4/O
/05ui42nPHlLISZPF9MBctzlM3ESFGa/i2KQfWzC13x89poHhawV9PbWcx4OqwO5
uvHXHxTFmLHKaEpP/MFGNU7rq3h8QeXGm4uCb/0O7oD4pDEaU/xV9NEGIp08XzBE
rmk4i4oWrfPjfkt/a/lw+Tsh6NekK4lv9UJIqVAB2FFnAgMBAAGjUzBRMB0GA1Ud
DgQWBBSW62qJkMthWor5raTxgsGKBRIc3DAfBgNVHSMEGDAWgBSW62qJkMthWor5
raTxgsGKBRIc3DAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQBp
pNjbSjamj3nKdKdIcVcZIgXtFRB5NaVYA6ABiWmLoSSEvDUU47+ofKnSWRwABx+u
72oJavXE7srnTXOyqD5RZUrz2Ca8AcUBiZ/CluIE2CJ/r5PXgMDK7hRpZ35pmmFC
rE+RmRUlFPHQE6PoIFCllzod2H3ikW8MroEYjnaW9ivFC5tCv8vhAYp+aLcZmKBt
YhilSGMTIWVLOmGYzoh6pgxH9dlCKMIE28kHwGj2zDUuPQVoQXVpikF6c7Ffh7yE
Fim1kRjUXIO8eujnZa8aaTBqCBIBTshSquOlwlhEGHCj9rIJzne7YHDwNUIyGxNd
+L8V0Jc0S2WbR8r9xQsU
-----END CERTIFICATE-----)";

const char *client_key = R"(-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDOOFUUud8dsFYj
q0aTcD7geyljtzKvz/nbUFKn9fHvwMZvO6HOU8xLI/LJe6c/d57IXIGmtb2//ni5
tL2TgDcmhwgDSvtFKA94OHzQjrcbSo0AHxb0ds1xM+ND1xaDWfyn0Nj4qWBCghvi
oTyNTA/lgGPjmRStejohnUKBi4/O/05ui42nPHlLISZPF9MBctzlM3ESFGa/i2KQ
fWzC13x89poHhawV9PbWcx4OqwO5uvHXHxTFmLHKaEpP/MFGNU7rq3h8QeXGm4uC
b/0O7oD4pDEaU/xV9NEGIp08XzBErmk4i4oWrfPjfkt/a/lw+Tsh6NekK4lv9UJI
qVAB2FFnAgMBAAECggEAMUV3WpjgbL0Fj8cr+S0/8K2soXUTiLs11Qc3gIb8c87I
+qmyStRtwuRHbYl8V+cySmgD2DujJxzbZm9/Ph9LzgpESVkgeRENxSkl9USsDjq9
PrKu7uD1zB7cJYjeCas1+Timpp79b1ab6AWjLJVoVOrcsvQP4zUJb5+d1MruJMQg
R1+CuqYGzrBLlerJx9H4vvMrXpa/4KA+00rnS3ye1uyF2+X588La71SYptegpbVw
ISXTdATrGCuj3uVkD5kkC15PXafzzQs5IWdM8/8qxWFBDb+hDvxeAcSCrJkJgODt
VilCrHk+dKN1hTtNg3ntOf55xgsKDF+6YVXxesveYQKBgQD1Orwnesq+IAtvw7gN
+iA2ODyPga3+kAoXV0JOwBAs1DQIzPfZa9r24xdoypethgNpagVTKVCcjBijlR/F
kMKHjyXVl477g6GtoKAaqqTEOqTcaRIkhpal2/8YyxOOkw3EZDxhruSM9AH0YFZc
GJjbE3ff8tPiFmc9a9GkyE9mBwKBgQDXRv2sUwgiTVek1cWlFtNhsUpBaemj+cfI
evuPLAD6DBcekyjpVik5xADEcFHD7j+0uOUis4P5ugcozTPUXX7yvkIm9fEbcHdy
xRaL/anp6/ZIY8jjcteZvQ7rK14WgirrdJF0nLnY47tx1R3Tco9uro0oLNycKybk
02uQf5vhoQKBgQCzSW/KRWH4wqDZ43Oq9FWcjlzoZlz5IgHesrMNtCmfPL2WOZRy
5wG9QqYYvRVLjhDsftDJzgjbiylwX1RNMtqmFSgKErY8eC46LJchJ+UyedWSN1dG
UQfO1xqX29C+ihCkpsvmduEtlC/hpJU+29TVRlR77KKwRsTQCQXKvZAe/QKBgEmY
rn6sQDqh4FWs843V+NwMJUuprsNtUDyctsWikI7rdTfxrWngF+X0uSJvF7T5DXR2
RXeQkYzdaTcsuAxhDlZkbu8s44my8FMfR+8CAIXFMoRbAMGpcEXR2XjSca8WPyAR
0Oxh7DNW360x46l3H8wCBqX2eFE3e2t9T+etiJghAoGBALiLUmjpssA7tXY3yjfI
m45dvPn0iSUplrD1sXhzTieqMbw89wanl79ZdUrCmxF0ept9+4rqCAwWEOqacExJ
bNDgNSkEzdjLpG0TQaocVAfXqd5ZCs3wPh/KtjQJzc8Pn3Cilif3dhxSahquAtCz
fAf7reVO5lc7EqoYUK380J/A
-----END PRIVATE KEY-----)";

void buzzerBeep(int pattern) {
    if (pattern == 1) { // Irama 1 untuk kartu terdaftar
        for (int i = 0; i < 4; i++) { 
            digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
            digitalWrite(BUZZER_PIN, LOW);
            delay(150);
        }
    } else if (pattern == 2) { // Irama 2 untuk kartu tidak terdaftar
        for (int i = 0; i < 2; i++) {
            digitalWrite(BUZZER_PIN, HIGH);
            delay(300);
            digitalWrite(BUZZER_PIN, LOW);
            delay(200);
        }
    }
}

void keypadHandle(void *parameter) {
    for (;;) {
      Serial.println("WOI BAH");
      vTaskDelete(NULL);
    }
}

void httpHandle(void *parameter) {
    for (;;) {
        WiFiClientSecure *client = new WiFiClientSecure;
        if (client) {
            client->setCACert(server_cert);
            client->setCertificate(client_cert);
            client->setPrivateKey(client_key);

            HTTPClient https;
            if (https.begin(*client, "https://mocki.io/v1/84130b2e-0aee-4572-9674-360cc7f0f700")) {
                int httpCode = https.GET();

                if (httpCode > 0) {
                    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                        String payload = https.getString();
                        DeserializationError error = deserializeJson(doc, payload);
                        if (error) {
                            Serial.println("Failed to parse JSON");
                        }
                    }
                } else {
                    Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
                https.end();
            } else {
                Serial.printf("[HTTPS] Unable to connect\n");
            }
            delete client;
        } else {
            Serial.println("Unable to create client");
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Delay 5 seconds
    }
}

void rfidHandle(void *parameter) {
    for (;;) {
        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
            vTaskDelay(100 / portTICK_PERIOD_MS); // Avoid busy waiting
            continue;
        }

        Serial.println(F("**CARD DETECTED**"));

        // Bunyikan buzzer selama 500 ms
        digitalWrite(BUZZER_PIN, HIGH);
        delay(500);
        digitalWrite(BUZZER_PIN, LOW);

        // Delay tambahan sebelum pengecekan UID
        vTaskDelay(500 / portTICK_PERIOD_MS);

        UID = "";
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            UID += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") +
                   String(mfrc522.uid.uidByte[i], HEX) +
                   (i != (mfrc522.uid.size - 1) ? " " : "");
        }
        UID.toUpperCase();
        Serial.println(UID);
        mfrc522.PICC_HaltA();

        bool found = false;
        // Mencari nama berdasarkan UID dari JSON
        for (JsonObject obj : doc.as<JsonArray>()) {
            const char *nama = obj["nama"];
            const char *uid = obj["uid"];

            if (strcmp(UID.c_str(), uid) == 0) {
                found = true;

                Serial.printf("Welcome to Office %s !!!\n", nama);
                Serial.println("PIN : ");

                // Menampilkan pesan di LCD
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Welcome !!!");
                lcd.setCursor(0, 1);
                lcd.print(nama);

                servo1.write(90);
                // Bunyikan buzzer (kartu terdaftar)
                buzzerBeep(1);

                vTaskDelay(3000 / portTICK_PERIOD_MS);
                servo1.write(180);

                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("PIN :");
                xTaskCreate(keypadHandle, "Keypad Task", 4096, NULL, 1, &keypadTask);
                break;
            }
        }

        if (!found) {
            Serial.println("Card not registered!");

            // Menampilkan pesan di LCD
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Access Denied!");

            // Bunyikan buzzer (kartu tidak terdaftar)
            buzzerBeep(2);
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(BUZZER_PIN, OUTPUT);
    servo1.attach(SERVO_PIN);

    SPI.begin();
    mfrc522.PCD_Init();
    mfrc522.PCD_DumpVersionToSerial();

    Wire.begin(26, 27); // SDA = 26, SCL = 27
    lcd.begin();
    lcd.backlight();

    WiFi.begin(ssid, password);
    Serial.printf("Connecting to WiFi with SSID : %s\n", ssid);
    while (!WiFi.isConnected());
    Serial.println("Connection successful");
    lcd.print("Wifi Connected");

    // Create tasks
    xTaskCreate(httpHandle, "HTTP Task", 8192, NULL, 1, &httpTask);
    xTaskCreate(rfidHandle, "RFID Task", 4096, NULL, 1, &rfidTask);
}

void loop() {
    // Kosong, tugas berjalan sebagai task
}
