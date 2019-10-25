#include "DHT.h" /* Temperature and humidity sensor lib */
#include <ESP8266WiFi.h> /* Wi-fi lib */
#include "Agentuino.h"

/* DHT11 */
#define DHTPIN 0 // pino analógico do sensor
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

/* Wi-fi */
const char *ssid =  "FADERGS";     // replace with your wifi ssid and wpa2 key
const char *pass =  "";



void setup() 
{
  Serial.begin(9600);
  Serial.println("Comunicação serial iniciada...");
  Serial.println("Iniciando DHT11...");
  dht.begin();

  Serial.println("Iniciando Wi-Fi...");
  WiFi.begin(ssid, pass); 
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
  //float h = dht.readHumidity();
  //float t = dht.readTemperature();
  // testa se retorno é valido, caso contrário algo está errado.
  //if (0) 
  //{
   // Serial.println("Failed to read from DHT");
  //} 
  //else
  //{
   // Serial.print("Umidade: ");
   // Serial.print(h);
    //Serial.print(" %t");
   // Serial.print("Temperatura: ");
   // Serial.print(t);
   // Serial.println(" *C");
  //}
}
