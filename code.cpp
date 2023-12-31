#include <ESP8266WiFi.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h" 
#include <Adafruit_BMP085.h> 

#define lightPin A0 

String apiKey = "POPRRGVFVIW2R3R1"; 
const char *ssid =  "POCO X3"; 
const char *pass =  "123457890";
const char* server = "api.thingspeak.com";

DHT dht(14,DHT11);
Adafruit_BMP085 bmp;

WiFiClient client;

float lightVal;
int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void setup() {
  Serial.begin(115200);
  delay(10);
  
  lcd.begin();
  lcd.backlight();
  
  dht.begin();
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  pinMode(lightPin, INPUT);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Weather Update");
  delay(1000);
  lcd.clear();
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  lightVal = analogRead(lightPin);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  float bmp_pressure = bmp.readPressure();
  float bmp_altitude = bmp.readAltitude();

  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += String(lightVal);
    postStr += "&field4=";
    postStr += String(bmp_pressure);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" Cel");
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.println(" %");
    
    lcd.setCursor(0,0);
    lcd.print("Temp C =");
    lcd.setCursor(0,1);
    lcd.print("Humidity %=");
    lcd.setCursor(9,0);
    lcd.print(t);
    lcd.setCursor(12,1);
    lcd.print(h);
    delay(2000);
    lcd.clear();
    
    Serial.print("Light Intensity: ");
    Serial.println(lightVal);

    lcd.setCursor(0,0);
    lcd.print("Light =");
    lcd.setCursor(9,0);
    lcd.print(lightVal);
    delay(2000);
    lcd.clear();
    
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");
    Serial.print("\n");

    lcd.setCursor(0,0);
    lcd.print("Pressure =");
    lcd.setCursor(0,1);
    lcd.print("Altitude =");
    lcd.setCursor(11,0);
    lcd.print(bmp.readPressure());
    lcd.setCursor(11,1);
    lcd.print(bmp.readAltitude());
    delay(2000);
    lcd.clear();
  }
  client.stop();
  delay(1000);
}