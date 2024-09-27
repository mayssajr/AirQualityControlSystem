#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

const char* ssid = "***********";
const char* password = "*************";
const char* apiKey = "SYKXQFJBA7BXN3OF";
const char* server = "http://api.thingspeak.com/update";

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define MQ135_PIN 35
#define RED_PIN 25
#define GREEN_PIN 26
#define BLUE_PIN 27
#define BUZZER_PIN 23

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MQ135_PIN, INPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int mq135Value = analogRead(MQ135_PIN);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C, Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Air Quality: ");
  Serial.println(mq135Value);

  if (mq135Value > 2100) {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(BUZZER_PIN, HIGH);
  } else if (mq135Value <= 2100 && mq135Value > 1000) {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = server + String("?api_key=") + apiKey +
                 "&field2=" + String(mq135Value) +
                 "&field1=" + String(temperature);
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.print("Data sent to ThingSpeak. HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending data. HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Wi-Fi not connected.");
  }

  delay(1000);
}
