#include <Arduino.h>
#include <WiFi.h>
#include <HX711.h>

// ### Lista de pines ### //

// HX711 (Galga) circuit wiring
const int LOADCELL_DOUT_PIN = 19;
const int LOADCELL_SCK_PIN = 18;
const float calibration_factor = 20780.0;

float units;

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

// ### Instanciación de sensores y actuadores ### //
Galga galga; // Galga de peso

void setup()
{
  Serial.begin(9600);

  // Ajustes de la Galga de Peso (Pines, scala y offset)
  galga.setUp(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, calibration_factor);
}

void loop()
{
    Serial.print(galga.getWeight());
    Serial.println();
    delay(100);
}