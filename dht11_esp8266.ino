// Include library yang diperlukan
#include <DHT.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

//Deklarasi variabel  dht
#define DHTPIN D7
#define DHTTYPE 11

// Gunakan serial sebagai monitor
#define USE_SERIAL Serial

// Buat object Wifi
ESP8266WiFiMulti WiFiMulti;

// Buat object http
HTTPClient http;
String payload;
String stat;
// Buat object dht
DHT dht (DHTPIN, DHTTYPE, 15);

// Ini adalah alamat script (URL) yang kita pasang di web server
// Silahkan sesuaikan alamat IP dengan ip komputer anda atau alamat domain (bila di web hosting)

String url = "http://192.168.43.108/dht11/konek.php";

//===============================
// SETUP
//===============================

void setup() {

  Serial.begin(9600);
  dht.begin();
  delay(1000);


  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(false);

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] Tunggu %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("OPPO A11k", "xxxxxx"); // Sesuaikan SSID dan password ini

}

//===============================
// LOOP
//===============================

void loop() {

  //Baca suhu dan kelembapan ruangan
  float humidity = dht.readHumidity();
  delay(50);
  float temperature = dht.readTemperature();
  delay(50);


  //Cek sensor dht 22
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Sensor DHT 11 Tidak terditeksi");
    delay(100);
  } else {
    Serial.println(temperature);
  }

  // Cek apakah statusnya sudah terhubung
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    // Tambahkan nilai, suhu, dan kelembapan pada URL yang sudah kita buat
    USE_SERIAL.print("[HTTP] Memulai...\n");
    String requestURL = url + "?temperature=" + (String)temperature + "&humidity=" + (String)humidity;
    http.begin(requestURL);

    // Mulai koneksi dengan metode GET
    USE_SERIAL.print("[HTTP] Melakukan GET ke server...\n");
    int httpCode = http.GET();

    // Periksa httpCode, akan bernilai negatif kalau error
    if (httpCode > 0) {
      // Tampilkan response http
      USE_SERIAL.printf("[HTTP] kode response GET: %d\n", httpCode);
      
      // Bila koneksi berhasil, baca data response dari server
      if (httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        USE_SERIAL.println(payload);
      }
      
    } else {
      USE_SERIAL.printf("[HTTP] GET gagal, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    delay(10000);
  } else {
    USE_SERIAL.println("WiFi tidak terkoneksi, reconnect...");
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("OPPO A11k", "xxxxxx"); // Sesuaikan SSID dan password ini
  }
  delay(4000);
}
