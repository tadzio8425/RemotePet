#include <Arduino.h>
#include <WiFi.h>
#include <HX711.h>
#include "BH1750FVI.h"
#include <WebSocketsServer.h>
#include <Servo.h>
#include <ArduinoJson.h>
#include <sstream>

// # Configuración del JSON con la información de los sensores //
StaticJsonDocument<200> doc;


// ### Configuración del WIFI ### //
const char* ssid     = "RemotePet-Server";
const char* password = "eusebio8425";


// ### Configuración del socket webserver ### //
WebSocketsServer webSocket = WebSocketsServer(80); //Puerto de salida

//Función que se llama cuando se desea enviar un socket
void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  String payload_str = String((char*) payload);

  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
      }
      break;

    // Echo text message back to client
    case WStype_TEXT:

      if(strcmp((char *) payload, "Ping?") == 0){
        webSocket.sendTXT(num, "Pong.");
      }
      else{
      Serial.printf("[%u] Text: %s\n", num, payload);
      webSocket.sendTXT(num, payload);}
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}



// ### Lista de pines ### //

// HX711 (Galga) circuit wiring
const int LOADCELL_DOUT_PIN = 19;
const int LOADCELL_SCK_PIN = 18;
const float calibration_factor = 41800;
float units;

// GY30 (Light) circuit wiring
const int GY30_SDA = 21;
const int GY30_SCL = 22;

// Water Sensor Circuit Wiring
const int WS_S = 35;

//Servo motores
const int food_servo_pwm = 25;
const int water_servo_pwm = 26;
int minUs = 1000;
int maxUs = 2000;
int pos = 0;
bool once = true;

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

    return get_units();
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

class ServoM : public Servo{
  public:
    void setPosition(int angle){   

      for (pos = 0; pos <= angle; pos += 1) { // goes from 0 degrees to 180 degrees
		  // in steps of 1 degree
		  write(pos);    // tell servo to go to position in variable 'pos'
		  delay(4);             // waits 15ms for the servo to reach the position
	    }

    }

};

// ### Instanciación de sensores y actuadores ### //
Galga weightSensor; 
GY30 lightSensor(0x23);
WaterSensor waterSensor;
ServoM foodServo;
ServoM waterServo;


// ### Funciones auxiliares ### //

// ### MAIN ### //
void setup()
{
  Serial.begin(9600);

  // Ajustes de la Galga de Peso (Pines, scala y offset)
  weightSensor.setUp(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, calibration_factor);

  // Ajustes del sensor de luz en el ambiente (GY30)
  lightSensor.setUp();

  //Ajustes del sensor de agua (Water Sensor)
  waterSensor.setUp(WS_S);


  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  //Iniciar el WebSocket Server
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop()
{
  webSocket.loop();

  foodServo.attach(food_servo_pwm);
	waterServo.attach(water_servo_pwm);

  //Se asignan los valores actuales de cada sensor al JSON y se envia a todos los clientes
  doc["Sensors"]["Light"] = lightSensor.getLight();
  doc["Sensors"]["Weight"] = weightSensor.getWeight();
  doc["Sensors"]["Water"] = waterSensor.getWaterLevel();

  String str_doc;
  serializeJson(doc, str_doc);
  webSocket.broadcastTXT(str_doc);

}