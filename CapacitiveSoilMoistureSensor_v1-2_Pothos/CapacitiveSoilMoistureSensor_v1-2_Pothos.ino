// The Capacitive Soil Moisture Sensor v1.2 is active low.  
// We define RELAY_ON and RELAY_OFF for clarity.  
#define RELAY_ON LOW
#define RELAY_OFF HIGH

const int _airValue = 617;   // Replace the value with value when placed in air using calibration code.
const int _waterValue = 349; // Replace the value with value when placed in water using calibration code.
const int _waterPumpPin = 2;
int soilMoistureSensorValue = 0;
int soilMoisturePercent = 0;

void setup() {
  Serial.begin(9600);

  // Ensure the digital pin for the water pump is off by default.
  digitalWrite(_waterPumpPin, RELAY_OFF);
  pinMode(_waterPumpPin, OUTPUT);
}

void loop() {
  soilMoistureSensorValue = analogRead(A0);

  Serial.print("Moisture Sensor Value: ");
  Serial.println(soilMoistureSensorValue);
  
  soilMoisturePercent = map(
    soilMoistureSensorValue,
    _airValue,
    _waterValue,
    0,
    100
  );
  
  if (soilMoisturePercent > 100)
  {
    soilMoisturePercent = 100;
  }
  else if (soilMoisturePercent < 0)
  {
    soilMoisturePercent = 0;
  }
  
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisturePercent);
  Serial.println("%");
  
  delay(250);

  // Turn on/off water pump.
  if(soilMoisturePercent <= 35)
  {
    digitalWrite(_waterPumpPin, RELAY_ON);
  }
  else if(soilMoisturePercent >= 55)
  {
    digitalWrite(_waterPumpPin, RELAY_OFF);
  }
}
