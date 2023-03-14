#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <SPI.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

unsigned long lastMsg = 0;

int ntc = A0;
int yagmur = A1;
int hava_kalite = A2;

void setup() {
  Serial.begin(115200);
  pinMode(hava_kalite, INPUT);
  pinMode(yagmur, INPUT);
  pinMode(ntc, INPUT);

  if (!bme.begin()) {
    Serial.println("BME680 baglanamadi!");
    while (1);
  }

  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop() {

  int deger = analogRead(ntc);
  String durum = "";

  StaticJsonDocument<160> doc;
  char giden_veri[255];

  long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;

    float sicaklik = Termistor(deger);
    float basinc = bme.readPressure() / 100.0;
    float nem = bme.readHumidity();
    float gazDirenci = bme.readGas() / 1000.0;
    float ppm = analogRead(hava_kalite);

    int yagmur_s = analogRead(yagmur);
    if (900 <= yagmur_s && yagmur_s < 1023) {
      durum = "HAVA KURU";
    }
    if (300 <= yagmur_s && yagmur_s < 900) {
      durum = "HAVA YAGISLI";
    }
    if (yagmur_s < 300) {
      durum = "HAVA SAGANAK YAGISLI";
    }

    doc["sicaklik"] = sicaklik;
    doc["basinc"] = basinc;
    doc["nem"] = nem;
    doc["ppm"] = ppm;
    doc["durum"] = durum;
    doc["gazDirenci"] = gazDirenci;

    serializeJson(doc, giden_veri);
    Serial.println(giden_veri);
  }
}

double Termistor(int analogOkuma) {
  double sicaklik;
  sicaklik = log(((10240000 / analogOkuma) - 10000));
  sicaklik = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * sicaklik * sicaklik)) * sicaklik);
  sicaklik = sicaklik - 273.15;
  return sicaklik;
}
