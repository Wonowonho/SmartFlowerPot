
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define WIFI_SSID "LG U+ Router_01C5AB"
#define WIFI_PASSWORD "05116139"
#define DHTPIN 14
#define DHTTYPE DHT11
#define FIREBASE_HOST "mylittlefarm-mk1.firebaseio.com"
#define FIREBASE_AUTH "5tJHKbSjrYkI1OyP4t03mvK8CyQ7edQgCDDjWEOr"
#define WATERPUMP 4
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x3F, 16, 2);
const int analogInPin = A0;
int WaterHeight = 0;
void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  pinMode(WATERPUMP, OUTPUT);
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Wire.begin(2, 0);
  lcd.init();   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight
  //lcd.print(" My Little Farm "); // Start Printing
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  digitalWrite(WATERPUMP, HIGH);
  delay(100);
}
int button = 0;
void loop() {
  delay(100);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  WaterHeight = analogRead(analogInPin);

  if (isnan(humidity) || isnan(temperature) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  //Firebase.set("Water",WaterHeight);
  Firebase.set("/data/button", button);
  Firebase.set("/data/humidity", (int)humidity);
  Firebase.set("/data/temperature", (int)temperature);
  switch (Firebase.getInt("/data/button")) {
    case 0:
      button = 0;
      Firebase.set("/data/button", 0);
      Serial.println("button is off");
      break;
    case 1:
      digitalWrite(WATERPUMP, LOW);
      delay(2000);
      digitalWrite(WATERPUMP, HIGH);
      button = 0;
      Firebase.set("/data/button", 0);
      break;
  }
  lcd.init();   // initializing the LCD
  // 온도와 습도값 시리얼 모니터에 출력
  Serial.print((int)temperature); Serial.print(" *C, ");
  Serial.print((int)humidity); Serial.println(" %");

  // LCD에 출력할 습도 메세지
  String humi = "Humi : ";
  humi += (String)humidity;
  humi += "%";
  lcd.print(humi);
  lcd.setCursor(0, 0);
  // LCD에 출력할 온도 메세지
  String temp = "temp : ";
  temp += (String)temperature;
  temp += "C";
  lcd.setCursor(0, 1);
  lcd.print(temp);
  for (int i = 0; i < 120; i++) {
    if (Firebase.getInt("button")) {
      digitalWrite(WATERPUMP, LOW);
      delay(2000);
      digitalWrite(WATERPUMP, HIGH);
      button = 0;
      Firebase.set("/data/button", 0);
    }
    delay(10);
    break;
  }
}
