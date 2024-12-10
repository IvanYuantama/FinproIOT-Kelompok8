// #define BLYNK_TEMPLATE_ID "TMPL6HxEZ5GGi"
// #define BLYNK_TEMPLATE_NAME "FinproIOT"
// #define BLYNK_AUTH_TOKEN "Tg9T1j4TtzA3wprD9awXRjW3ix_Lvqai"


#define BLYNK_TEMPLATE_ID "TMPL6LA2UCLob"
#define BLYNK_TEMPLATE_NAME "FinproIOT8"
#define BLYNK_AUTH_TOKEN "TFvadeQaJIuMbuLccqYBe1A6DX9a9F_E"


#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <BlynkSimpleEsp32.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

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

const char ssid[] = "GJ";
const char password[] = "sumbawa8";
const char auth[] = BLYNK_AUTH_TOKEN;

// // Konfigurasi Keypad 3x4
// const byte ROWS = 4; // Jumlah baris pada keypad
// const byte COLS = 3; // Jumlah kolom pada keypad

// char keys[ROWS][COLS] = {
//     {'1', '2', '3'},
//     {'4', '5', '6'},
//     {'7', '8', '9'},
//     {'*', '0', '#'}
// };

// byte rowPins[ROWS] = {25, 33, 32, 35}; // Pin baris
// byte colPins[COLS] = {12, 14, 5};    // Pin kolom

// Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variabel untuk menangkap PIN
// String pin;
String enteredPin;
bool pinEntered;
String nama;
String uid;
String pinKaryawan;
int countKaryawan = 0;

// Sertifikat SSL
// openssl s_client -connect mocki.io:443 -showcerts
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


// openssl req -newkey rsa:2048 -nodes -keyout client_key.pem -x509 -days 365 -out client_cert.pem
const char *client_cert = R"(-----BEGIN CERTIFICATE-----
MIIDazCCAlOgAwIBAgIULFfEDbJUaYNZHkU3TDAxImmj9j8wDQYJKoZIhvcNAQEL
BQAwRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM
GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0yNDEyMDkwNDIwNTBaFw0yNTEy
MDkwNDIwNTBaMEUxCzAJBgNVBAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEw
HwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwggEiMA0GCSqGSIb3DQEB
AQUAA4IBDwAwggEKAoIBAQCdgHOFK3fiNAVcmi2fOLuWdxshbIKo9Ib54s3Nt0ZV
MkOkoq6LmwCnbMitIfmceHNNjonFbzpeXqJneRwm+sXpLPIbvne4qUs4C+duLDBJ
cpcm/bJEe2E5uJpOVB4X9CBgfrvHtJqERQ0U8/hk4PBNz89bS30yM5a+zyoCkrWz
Hsw66pdfk0nieghJF6A2CNRRn8iKwKAtql3kXVLkGdKwzTZ0pUht2kM2BYTvVgf+
GZcKpPzfykYQEibeaoltvCakhyATvsLxgdKPVGqtdXqZfg0O4BkrULxO4XLwKp8w
qV9PkvY9u2z/wxs0qgUTjYeRvt1ncpbzmLzEx1xOpYMhAgMBAAGjUzBRMB0GA1Ud
DgQWBBQzqfuFgHXwHH9TS7lg7hm+4U76eDAfBgNVHSMEGDAWgBQzqfuFgHXwHH9T
S7lg7hm+4U76eDAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQA7
uIsUPvvqX3Ki4JWXMRuSKhoHGjH4TwXMxNrgJM1CYnxkbpMNo7yb+DgUKSLZnj4A
KpKruSnF0A64WRtXvXs0ayXBazJx1TgXkiDC9HxdjYEHZzMO+JIhF9ce1QrtsgVI
/RK4aqrzI0K3mFRT+enZa27viXY2sysdCv0KseuMSJIP4qxAOYHLraLtEtvtI8zm
x4VbmxjeNdOLixqD4Kl0xY/OhJbUBeT1VyRgtk6H7BpbCaVStIyKCjrpXzd2kgrr
MroVaf9SeXDQIL/YBvepu6knRjlWFORfAFBW9NEdItmnY/J7loQ2qCxpIrN4ZWk/
5vBkUYnTxy52wKYSCYnI
-----END CERTIFICATE-----)";

const char *client_key = R"(-----BEGIN PRIVATE KEY-----
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCdgHOFK3fiNAVc
mi2fOLuWdxshbIKo9Ib54s3Nt0ZVMkOkoq6LmwCnbMitIfmceHNNjonFbzpeXqJn
eRwm+sXpLPIbvne4qUs4C+duLDBJcpcm/bJEe2E5uJpOVB4X9CBgfrvHtJqERQ0U
8/hk4PBNz89bS30yM5a+zyoCkrWzHsw66pdfk0nieghJF6A2CNRRn8iKwKAtql3k
XVLkGdKwzTZ0pUht2kM2BYTvVgf+GZcKpPzfykYQEibeaoltvCakhyATvsLxgdKP
VGqtdXqZfg0O4BkrULxO4XLwKp8wqV9PkvY9u2z/wxs0qgUTjYeRvt1ncpbzmLzE
x1xOpYMhAgMBAAECggEABAiDTMx3ajM/VOb81pg3E4wSxYkYEngKkdEZSJSSKEHc
ghWJZzlNq7vpKqfVUsix2TFWfVgTFFITkhdDnYrr8QPt4kB/I9sv+iwx3bwjc2oM
DVtPSponO/lqeCmGyMG/Z4EvUVaiY1M6WchCmuC4vqFc879L4jJ4gIPhFUZJAWB0
8Wn5Tyb6Ovx0YOlv3r8CWVGnRDYjuvZnZq6k8WEuLBzm4lD5HljOoLILzD0MVbwd
o2XylclGk4YNfD9Viz/YPZZd4L6X8EXzALwqNQlB+ZZeKZtlSXonL1UdL1kK+J8z
QoenNwEQ7p7kMSL+0XGEOVFpgByYBAXgn4OS4xN4zQKBgQDcrvSJdT3Z1KQVhIHB
7LyJuAqUTyL4s1P8oxJ6TmIJQWq4ivq3/YseQqlL9DMnYRvQUxmRmNli3S8WtanB
917Yxgink7YDh9PYyxnK2BhqdQGdh9iVBetTGzVWybrLjcPSZCom78rLZv5GvVN2
piwcdV3eTcXfzbXti/gGErP8ZQKBgQC2tQuRCtGzBkuAOxlQjIKeheejiAO2Vb7a
NL0xlpODONclrF32BQI64LlPoL3ig9RrTiojqYDA6G2HoN4ZP8WuXGY438HdcgKX
2JFGmuhx1G04RhOfAzK+NPYglGIgL6oxPwO2/Q0NL2QIqyxPSPyj60dId+RL+I9P
S7HfplLKDQKBgFeub9yZp5x//z3JayDY5cQ2SoXIt5Vm8uzDAhl0QUF6K2PtXFZ0
N60rQUa7XQO9cqagDd2qhFziks4MGcnGdnncnR2v87aNZh+R+sp8d578pEqp6eSz
+F8JxXSZLE2qIu7Z+2GDDukoH8mNebb/qTEZdNVZw0/71NfwL/iBv999AoGAFZeF
u2lc6NxIlenmDvfcA+HtQw0y6xxtBpqO9UpHOo3AF0qsfAORhOXD4J4tcpSDyP6e
ykIg3itZrlqbhL5dnUJ8LiJ7ZbEwj0Nfv8lUyQcCtVDKxtL47zORFr6Sbh1T7qf0
x25WWYWNO90GyoPkn3aRoQXSLAw+228loTmltQ0CgYAHuxFvzsmBbjaZP3Id5iNN
lkNITZPExHAyWXrACUfAqF8Z5ZTpHqz13LlM9PRIBVItnIyh/RIzWTInZu86+iXg
VkQ3eykBMU6UuJfdtfz5l64Y5NHvG97FuFS6NEj6ZSWNBoD1rDy/Is1UIQe+AXcz
JDRzfKL6LqaZvPIpoSSWgQ==
-----END PRIVATE KEY-----)";

// Mutex untuk melindungi buffer PIN
SemaphoreHandle_t xMutex;

// Fungsi untuk menangani input dari tombol angka (1 - 9)
BLYNK_WRITE(V100) { handleKeypadInput(0, param.asInt()); }
BLYNK_WRITE(V101) { handleKeypadInput(1, param.asInt()); }
BLYNK_WRITE(V102) { handleKeypadInput(2, param.asInt()); }
BLYNK_WRITE(V103) { handleKeypadInput(3, param.asInt()); }
BLYNK_WRITE(V104) { handleKeypadInput(4, param.asInt()); }
BLYNK_WRITE(V105) { handleKeypadInput(5, param.asInt()); }
BLYNK_WRITE(V106) { handleKeypadInput(6, param.asInt()); }
BLYNK_WRITE(V107) { handleKeypadInput(7, param.asInt()); }
BLYNK_WRITE(V108) { handleKeypadInput(8, param.asInt()); }
BLYNK_WRITE(V109) { handleKeypadInput(9, param.asInt()); }

// Fungsi untuk menangani tombol ENTER
BLYNK_WRITE(V110) {
  if (param.asInt() == 1) { // Saat tombol ENTER ditekan
    pinEntered = true;
    Blynk.virtualWrite(V111, "0");
    keypadHandle(); 
  }
}

BLYNK_WRITE(V1) {
  if (param.asInt() == 1) { // Saat tombol ENTER ditekan
    countKaryawan = 0;
    Blynk.virtualWrite(V0, countKaryawan);    
  }
}

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

// void keypadHandle(void *parameter) {
//     pin = ""; // Pastikan PIN kosong saat task dimulai
//     lcd.clear();
//     Serial.println(pinKaryawan);

//     for (;;) { // Loop tak terbatas agar terus membaca keypad
//         char key = keypad.getKey(); // Baca tombol dari keypad

//         if (key) { // Jika ada tombol yang ditekan
//             if (key == '#') { // Jika tombol '#' ditekan, konfirmasi PIN
//                 Serial.printf("PIN Entered: %s\n", pin.c_str());
                
//                 if (strcmp(pin.c_str(), pinKaryawan) == 0) { // Validasi PIN
//                     Serial.println("PIN Benar!");

//                     // Menampilkan pesan di LCD
//                     lcd.clear();
//                     lcd.setCursor(0, 0);
//                     lcd.print("Welcome !!!");
//                     lcd.setCursor(0, 1);
//                     lcd.print(nama);

//                     // Aksi servo dan buzzer
//                     servo1.write(90); // Buka servo
//                     buzzerBeep(1);    // Bunyikan buzzer
//                     vTaskDelay(3000 / portTICK_PERIOD_MS); // Delay 3 detik
//                     servo1.write(180); // Tutup servo

//                 } else { // Jika PIN salah
//                     Serial.println("Invalid PIN");
//                     lcd.clear();
//                     lcd.setCursor(0, 0);
//                     lcd.print("Invalid PIN");
//                     vTaskDelay(2000 / portTICK_PERIOD_MS); // Tampilkan pesan selama 2 detik
//                 }
//                 pin = ""; // Reset PIN setelah validasi
//             } 
//             else if (key == '*') { // Tombol '*' untuk reset PIN
//                 char abah;
//             }
//             else { // Tambahkan angka ke PIN
//                 pin += key; // Tambahkan angka yang ditekan ke variabel PIN
//                 Serial.printf("Current PIN: %s\n", pin.c_str());
                
//                 // Tampilkan PIN di LCD
//                 lcd.setCursor(0, 0);
//                 lcd.print("PIN : ");
//                 lcd.setCursor(0, 1);
//                 lcd.print(pin.c_str());
//             }
//         }
//         vTaskDelay(50 / portTICK_PERIOD_MS); // Delay tambahan untuk efisiensi CPU
//     }
// }


void keypadHandle() {
    if (pinEntered) {
            Serial.println("Checking PIN...");
            Serial.println(pinKaryawan);
            Serial.println(enteredPin);
            if (enteredPin == pinKaryawan) { // Bandingkan dengan PIN yang benar
                Serial.println("PIN Correct! Access Granted.");
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Access Granted!");

                buzzerBeep(1); // Bunyikan buzzer sebagai alarm

                // Servo membuka pintu
                servo1.write(90); 
                delay(2000); 
                servo1.write(0); // Kembali ke posisi awal
            } else {
                Serial.println("Incorrect PIN! Try Again.");
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Incorrect PIN!");
                buzzerBeep(2); // Bunyikan buzzer sebagai alarm
            }

            // Reset buffer PIN
            enteredPin = "";
            pinEntered = false;
            lcd.clear();
            lcd.print("Enter PIN: ");
        
    }
}


void handleKeypadInput(int value, int state) {
    if (state == 1) { // Tombol ditekan
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            if (enteredPin.length() < 4) {
                enteredPin += String(value); // Tambahkan angka ke buffer PIN
                Serial.print("Entered PIN: ");
                Serial.println(enteredPin);
                Blynk.virtualWrite(V111, enteredPin);

                
                lcd.setCursor(enteredPin.length() - 1, 1);
                lcd.print("*"); // Gunakan '*' untuk menyembunyikan angka

            } else {
                Serial.println("PIN sudah mencapai 4 digit.");
            }
            xSemaphoreGive(xMutex); // Lepaskan mutex
        }
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
            if (https.begin(*client, "https://mocki.io/v1/b600eba7-777a-4bee-86cd-57efc2f3f9af")) {
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
            nama = obj["nama"].as<String>();  // Mengambil nilai "nama" dan menyimpannya sebagai String
            uid = obj["uid"].as<String>();    // Mengambil nilai "uid" dan menyimpannya sebagai String
            pinKaryawan = obj["pin"].as<String>();  // Mengambil nilai "pin" dan menyimpannya sebagai String

            if (UID == uid) {
                found = true;
                countKaryawan++;
                Blynk.virtualWrite(V0, countKaryawan);
                lcd.clear();
                lcd.print("Enter PIN: ");

                //xTaskCreate(keypadHandle, "Keypad Task", 4096, NULL, 1, &keypadTask);
                break;
            }
        }

        if (!found) {
            Serial.println("Card not registered!");

            // Menampilkan pesan di LCD
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Access Denied!");

            // Bunyikan buzzer berirama (kartu tidak terdaftar)
            buzzerBeep(2);
            continue;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void blynkHandle(void *parameter) {
  for(;;){
    Blynk.run();
    vTaskDelay(300 / portTICK_PERIOD_MS); // Kurangi CPU usage
  }
}  

void setup() {
    Serial.begin(115200);
    pinMode(BUZZER_PIN, OUTPUT);
    servo1.attach(SERVO_PIN);

    SPI.begin();
    mfrc522.PCD_Init();
    mfrc522.PCD_DumpVersionToSerial();

    // Inisialisasi Mutex
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
      Serial.println("Mutex gagal dibuat!");
      while (1);
    }

    Wire.begin(26, 27); // SDA = 26, SCL = 27
    lcd.begin();
    lcd.backlight();

    WiFi.begin(ssid, password);
    Serial.printf("Connecting to WiFi with SSID : %s\n", ssid);
    while (!WiFi.isConnected());
    Serial.println("Wifi Connected");
    lcd.print("Wifi Connected");

    Blynk.begin(auth, ssid, password);
    while (WiFi.status() != WL_CONNECTED);
    Serial.println("Blynk Connected");
    lcd.clear();
    lcd.print("Blynk Connected");

    // Create tasks
    xTaskCreate(httpHandle, "HTTP Task", 8192, NULL, 15, &httpTask);
    xTaskCreate(rfidHandle, "RFID Task", 4096, NULL, 10, &rfidTask);
    xTaskCreate(blynkHandle, "Blynk Task", 4096, NULL, 5, NULL);
}

void loop() {
    // Kosong, tugas berjalan sebagai task
}
