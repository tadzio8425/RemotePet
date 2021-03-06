#include <Arduino.h>
#include <WiFi.h>
#include <HX711.h>
#include "BH1750FVI.h"
#include <WebSocketsServer.h>
#include <Servo.h>
#include <ArduinoJson.h>
#include <sstream>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Time.h>
#include <Adafruit_MLX90614.h>


//== CHANGE THIS ============
double new_emissivity = 0.95;
//===========================

Adafruit_MLX90614 mlx = Adafruit_MLX90614();


//Configuración del reloj interno
ESP32Time rtc(0); 

// # Configuración del JSON con la información de los sensores //
StaticJsonDocument<200> doc;

//Variables de ayuda
bool inInterval = false;
int intervalCount = 0;
bool timeSet = false;
long long start_time;
int hour = 50;
int minute = 80;
int intervalo = 100;

// # Configuración pantalla oled
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ### Configuración del WIFI ### //
const char* ssid     = "RemotePet-Server";
const char* password = "eusebio8425";


// ### Configuración del socket webserver ### //
WebSocketsServer webSocket = WebSocketsServer(80); //Puerto de salida

//Función auxiliar
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
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

//Sensor de movimiento PIR
const int PIR_S = 12;

//Servo motores
const int food_servo_pwm = 25;
const int bombaAguaPin = 16;
int minUs = 1000;
int maxUs = 2000;
int pos = 0;
bool once = true;
int last_pos = 0;

//Tiempo para motores
unsigned long previousMillis = 0;  
long timed_interval = 0;  
long auto_interval = 0;
bool only_pass = true;
int timed_kibble_amount;
int timed_water_amount;
int auto_kibble_amount;
int auto_water_amount;


//Variable para activar feed por proximidad
int autoFeed = 0;


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
    int light = getLux();
    
    if (light > 2200){
      light = 2200;
    }

    return light/22;
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

      float waterLevel;

      if(analogRead(_analogPin) > 1000){
        waterLevel = (analogRead(_analogPin)-650)/4.0909;
      }
      else{
        waterLevel = 0;
      }
      return waterLevel;
    }
  private:
    int _analogPin;
};

class ServoM : public Servo{
  public:
    void setPosition(int angle){   


      if (angle > last_pos){
      for (pos = last_pos; pos <= angle; pos += 1) { // goes from 0 degrees to 180 degrees
		  // in steps of 1 degree
		  write(pos);    // tell servo to go to position in variable 'pos'
		  delay(4);             // waits 15ms for the servo to reach the position
	    }

    }
    
      else{
      for (pos = last_pos; pos >= angle; pos -= 1) { // goes from 0 degrees to 180 degrees
		  // in steps of 1 degree
		  write(pos);    // tell servo to go to position in variable 'pos'
		  delay(4);             // waits 15ms for the servo to reach the position
	    }
      }
    last_pos = angle;
    }

};

class MotoBomba{
  public:
    void setUp(int pin){
      _pin = pin;
      pinMode(pin, OUTPUT);
    }
    void turnOn(){
      digitalWrite(_pin, HIGH);
    }

    void turnOff(){
      digitalWrite(_pin, LOW);
    }

  private:
    int _pin;

};

class PIR{
  public:
    void setUp(int digitalPin){
      _digitalPin = digitalPin;
      pinMode(_digitalPin, INPUT);
    }
    int isMovement(){
      return digitalRead(_digitalPin);
    }
  private:
    int _digitalPin;
};

// ### Instanciación de sensores y actuadores ### //
Galga weightSensor; 
GY30 lightSensor(0x23);
WaterSensor waterSensor;
ServoM foodServo;
PIR movementSensor;
MotoBomba bombaAgua;


//Funciones para obtener los intervalos según la porción deseada
int foodAmountToInterval(int amount){
  return amount;
}

int waterVolumeToInterval(int volume){
  return volume*0.0758*1000;
}

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
        webSocket.sendTXT(num, "Pong.");}
     
      if(payload_str.indexOf("INTERVAL START") >= 0){
      
         hour = getValue(payload_str, ':', 1).toInt();
         minute = getValue(payload_str, ':', 2).toInt();
         intervalo = getValue(payload_str, ':', 3).toInt();
         timed_kibble_amount = getValue(payload_str, ':', 4).toInt();
         timed_water_amount = getValue(payload_str, ':', 5).toInt();

         timed_interval = waterVolumeToInterval(timed_water_amount);

      }    

      if(payload_str.indexOf("WATER") >= 0){
        int waterManual = getValue(payload_str, ':', 1).toInt();
        if(waterManual == 1){
          bombaAgua.turnOn();
        }
        else{
          bombaAgua.turnOff();
        }
      }    

      if(payload_str.indexOf("KIBBLE") >= 0){
        int kibbleManual = getValue(payload_str, ':', 1).toInt();
        if(kibbleManual == 1){
          foodServo.setPosition(110);
        }
        else{
          foodServo.setPosition(60);
        }
      }   

      if(payload_str.indexOf("AUTO") >= 0){
        autoFeed = getValue(payload_str, ':', 1).toInt();
        auto_kibble_amount = getValue(payload_str, ':', 2).toInt();
        auto_water_amount = getValue(payload_str, ':', 3).toInt();
        auto_interval = waterVolumeToInterval(auto_water_amount);
        
      }



      Serial.printf("[%u] Text: %s\n", num, payload);
      webSocket.sendTXT(num, payload);
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


// ### Funciones auxiliares ### //


// ### MAIN ### //
void setup()
{
  Serial.begin(9600);

  //Hora inicial 
  rtc.setTime();

  //Ajustes pantalla OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();

  // Ajustes de la Galga de Peso (Pines, scala y offset)
  weightSensor.setUp(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, calibration_factor);

  // Ajustes del sensor de luz en el ambiente (GY30)
  lightSensor.setUp();

  //Ajustes del sensor de agua (Water Sensor)
  waterSensor.setUp(WS_S);

  //Ajustes del sensor de movimiento
  movementSensor.setUp(PIR_S);

  //Bomba de agua
  bombaAgua.setUp(bombaAguaPin);

  //Ajustes sensor temperatura
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
  };


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

  //INICIAR SERVO FOOD CERRADO
  foodServo.attach(food_servo_pwm);

  foodServo.setPosition(60);

}

void loop()
{
  webSocket.loop();

  //Se asignan los valores actuales de cada sensor al JSON y se envia a todos los clientes
  doc["Sensors"]["Light"] = lightSensor.getLight();
  doc["Sensors"]["Weight"] = weightSensor.getWeight();
  doc["Sensors"]["Water"] = waterSensor.getWaterLevel();
  doc["Sensors"]["Movement"] = movementSensor.isMovement();
  doc["Sensors"]["AmbTemp"] =mlx.readAmbientTempC();
  doc["Sensors"]["PetTemp"] =mlx.readObjectTempC();

  String str_doc;
  serializeJson(doc, str_doc);
  webSocket.broadcastTXT(str_doc);

  String formattedTime = rtc.getTime();

  //Configuración del display
  display.clearDisplay();
  display.setTextSize(2.5);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,16);             // Start at top-left corner
  display.println(formattedTime);
  display.display();

  //Configuración del timer feed
  unsigned long currentMillis = millis();

  if(rtc.getHour() == hour && rtc.getMinute() == minute){
    
    if (currentMillis - previousMillis >= timed_interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;

      if(last_pos == 60 && only_pass == true){ 
      foodServo.setPosition(110);
      bombaAgua.turnOn();
      only_pass = false;}

      else{
        foodServo.setPosition(60);
        bombaAgua.turnOff();
      }
  }}
  if(rtc.getHour() == hour && rtc.getMinute() == minute+1){
    only_pass = true;
  }

  //Configuración del autofeed
  if(movementSensor.isMovement()==1 and autoFeed == 1){
        if (currentMillis - previousMillis >= auto_interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;

      if(last_pos == 60){ 
      foodServo.setPosition(110);
      bombaAgua.turnOn();}

      else{
        foodServo.setPosition(60);
        bombaAgua.turnOff();
        currentMillis+=auto_interval;
      }
  }
  }

  
  }


    
