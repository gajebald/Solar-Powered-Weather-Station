// Project: https://www.pcbway.com/project/shareproject/Solar_Powered_Weather_Station_V_1_0.html
// Build your own Weather Station
//
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

// Air pressure on sea level:
#define SEALEVELPRESSURE_HPA 1031

Adafruit_BME680 bme; //  I2C

double humidity = 0.0; // Humidity
double temperature = 0.0; // temperature
double pressure = 0.0; // air pressure
double gas = 0.0; // gas / air quality
double est_altitude = 0.0; // Impeccable height
int sensorPin = A0;    // select the input pin for the potentiometer 
float rawRange = 1024; // 3.3v 
float logRange = 5.0; // 3.3v = 10^5 lux 

// replace with your channel’s thingspeak API key and your SSID and password
String apiKey = "xxxxxxxxxxxxxxx";
const char* ssid = "xxxxxx";
const char* password = "xxxxxxxxxxx";
const char* server = "api.thingspeak.com";
 
const int sleepSeconds = 180; // If you use DEEPSLEEP Bridge the "Sleep" Pad on the PCB
float t  ;
float h  ;
float p  ;
float g  ;
float c  ;
WiFiClient client;

void setup() 
{
//analogReference(EXTERNAL); // 
Serial.begin(9600);
pinMode(D0, WAKEUP_PULLUP);
delay(10);


  
  if (!bme.begin()) {
    Serial.println("No BME680 sensor found!");
    while (1);
  }
  
  // Initialisierung von  Oversampling und Filter
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms


  
WiFi.begin(ssid, password);
 
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
 
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) 
{
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");


}
 
void loop() 
{
int rawValue = analogRead(sensorPin);  
float voltage = rawValue / 200.7092;
       t = voltage ; 
 Serial.print("Raw = "); 
 Serial.print(rawValue); 
Serial.print("Voltage: ");
 Serial.print(voltage);
c = rawValue;

// Erst den bme680 auslesen
  if (! bme.performReading()) {
    Serial.println("Fehler beim Messen ");
    return;
  }
        
   
  // Werte ermitteln:
  temperature   = bme.temperature;
  pressure      = bme.pressure / 100.0;
  humidity      = bme.humidity;
  gas           = bme.gas_resistance / 1000.0;
  est_altitude  = bme.readAltitude(SEALEVELPRESSURE_HPA);


  Serial.print("Temperatur = ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Luftdruck = ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Feuchtigkeit = ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(gas);
  Serial.println(" KOhms");

  Serial.print("Ungefaehre Hoehe = ");
  Serial.print(est_altitude);
  Serial.println(" m");

  Serial.println();



  
    Serial.println(WiFi.localIP());  

;
 
if (client.connect(server,80)) {
String postStr = apiKey;
postStr +="&field1=";
postStr += String(temperature);
postStr +="&field2=";                           
postStr += String(humidity); 
postStr +="&field3=";                           
postStr += String(pressure);    
postStr +="&field5=";                           
postStr += String(voltage);  
postStr += "\r\n\r\n";
 
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);

Serial.println("Sending data to Thingspeak");
}
client.stop();
// thingspeak needs at least a 15 sec delay between updates
// 20 seconds to be safe
   Serial.printf("Sleep for %d seconds\n\n", sleepSeconds);
 ESP.deepSleep(sleepSeconds * 1000000);
//   delay(180000);
}

