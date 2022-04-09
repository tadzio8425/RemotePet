#include <Arduino.h>
#include <WiFi.h>
#include <HX711.h>
#include "BH1750FVI.h"

// ### Configuración del WIFI ### //
#define WIFI_NETWORK "CityU-WIFI-24G-T1-P26"
#define WIFI_PASSWORD "CityU2018*"
#define WIFI_TIMEOUT_MS 20000

void connectToWiFi(){
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();

  while(WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS){
    Serial.print(".");
    delay(100);
  }

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Failed!");
  
  }
  else{
    Serial.print("Connected!");
    Serial.println(WiFi.localIP());
  }
  }


// ### Lista de pines ### //

// HX711 (Galga) circuit wiring
const int LOADCELL_DOUT_PIN = 19;
const int LOADCELL_SCK_PIN = 18;
const float calibration_factor = 20780.0;
float units;

// GY30 (Light) circuit wiring
const int GY30_SDA = 21;
const int GY30_SCL = 22;

// Water Sensor Circuit Wiring
const int WS_S = 2;

// ### Lista de clases ### //
class Galga : public HX711
{
public:
  void setUp(int LOADCELL_DOUT_PIN, int LOADCELL_SCK_PIN, float calibration_factor)
  {
    begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    set_scale(calibration_factor);
    tare();
  };

  float getWeight()
  {
    units = get_units(), 10;
    if (units < 0)
    {
      units = 0.00;
    }

    return units;
  }

private:
};

class GY30 : public BH1750FVI
{
public:
  GY30(int i):BH1750FVI(i){};
  void setUp(){
    powerOn();
    setContLowRes();
  }
  float getLight(){
    return getLux();
  }
private:

};

class WaterSensor{
  public:
    void setUp(int analogPin){
      _analogPin = analogPin;
      pinMode(_analogPin, INPUT);
    }
    float getWaterLevel(){
      return analogRead(_analogPin);
    }
  private:
    int _analogPin;
};

// ### Instanciación de sensores y actuadores ### //
Galga weightSensor; 
GY30 lightSensor(0x23);
WaterSensor waterSensor;

void setup()
{
  Serial.begin(9600);

  // Ajustes de la Galga de Peso (Pines, scala y offset)
  weightSensor.setUp(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, calibration_factor);

  // Ajustes del sensor de luz en el ambiente (GY30)
  lightSensor.setUp();

  //Ajustes del sensor de agua (Water Sensor)
  waterSensor.setUp(WS_S);

  connectToWiFi();
}

void loop()
{

}