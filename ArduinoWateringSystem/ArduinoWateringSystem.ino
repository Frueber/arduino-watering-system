// The Capacitive Soil Moisture Sensor v1.2 is active low.  
// We define RELAY_ON and RELAY_OFF for clarity.  
#define RELAY_ON LOW
#define RELAY_OFF HIGH

const int _airValue = 617;   // Replace the value with value when placed in air using calibration code.
const int _waterValue = 349; // Replace the value with value when placed in water using calibration code.
const int _waterPumpPin = 2;
const int _soilMoistureSensorPin = A0;

// Pins for potentiometers that allow for adjusting maximum and minimum soil moisture percentages.
const int _maximumSoilMoisturePercentageSensorPin = A1;
const int _minimumSoilMoisturePercentageSensorPin = A2;
const int _maximumSoilMoisturePercentageSensorReadingValue = 1023;
int maximumSoilMoisturePercentage = 0;
int minimumSoilMoisturePercentage = 0;

// Time variables for throttling current soil moisture percentage serial printing.
unsigned long timeOfLastSoilMoisturePercentagePrint = 0;
unsigned long timePassedSinceLastSoilMoisturePercentagePrint = 0;

// Time variables for throttling maximum and minimum soil moisture sensor serial printing.
unsigned long timeOfLastSoilMoisturePercentageControlPrint = 0;
unsigned long timePassedSinceLastSoilMoisturePercentageControlPrint = 0;

int getSensorPercentage(
  int sensorPin,
  int maximumSensorReadingValue
)
{
  int sensorReading = analogRead(sensorPin);
  int percentage = map(
    sensorReading,
    0,
    maximumSensorReadingValue,
    0,
    100
  );

  return percentage;
}

void handleSoilMoisturePercentageControlPrint()
{
  timePassedSinceLastSoilMoisturePercentageControlPrint = millis() - timeOfLastSoilMoisturePercentageControlPrint;
  bool isSoilMoisturePercentageControlPrintAllowed = timePassedSinceLastSoilMoisturePercentageControlPrint > 1000;
  
  if(isSoilMoisturePercentageControlPrintAllowed)
  {
    Serial.print("Maximum Soil Moisture: ");
    Serial.print(maximumSoilMoisturePercentage);
    Serial.println("%");
    Serial.print("Minimum Soil Moisture: ");
    Serial.print(minimumSoilMoisturePercentage);
    Serial.println("%");

    timeOfLastSoilMoisturePercentageControlPrint = millis();
  }
}

void handleSoilMoisturePercentageControl()
{
  maximumSoilMoisturePercentage = getSensorPercentage(
    _maximumSoilMoisturePercentageSensorPin,
    _maximumSoilMoisturePercentageSensorReadingValue
  );
  minimumSoilMoisturePercentage = getSensorPercentage(
    _minimumSoilMoisturePercentageSensorPin,
    _maximumSoilMoisturePercentageSensorReadingValue
  );

  if(maximumSoilMoisturePercentage < minimumSoilMoisturePercentage)
  {
    maximumSoilMoisturePercentage = minimumSoilMoisturePercentage;
  }
  
  if(minimumSoilMoisturePercentage > maximumSoilMoisturePercentage)
  {
    minimumSoilMoisturePercentage = maximumSoilMoisturePercentage;
  }

  handleSoilMoisturePercentageControlPrint();
}

void handleSoilMoisturePercentagePrint(
  int soilMoistureSensorValue,
  int soilMoisturePercentage
)
{
  timePassedSinceLastSoilMoisturePercentagePrint = millis() - timeOfLastSoilMoisturePercentagePrint;
  bool isSoilMoisturePercentagePrintAllowed = timePassedSinceLastSoilMoisturePercentagePrint > 1000;
  
  if(isSoilMoisturePercentagePrintAllowed)
  {
    Serial.print("Moisture Sensor Value: ");
    Serial.println(soilMoistureSensorValue);
    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisturePercentage);
    Serial.println("%");

    timeOfLastSoilMoisturePercentagePrint = millis();
  }
}

int getCurrentSoilMoisturePercentage()
{
  int soilMoistureSensorValue = analogRead(_soilMoistureSensorPin);

  int soilMoisturePercentage = map(
    soilMoistureSensorValue,
    _airValue,
    _waterValue,
    0,
    100
  );
  
  if (soilMoisturePercentage > 100)
  {
    soilMoisturePercentage = 100;
  }
  else if (soilMoisturePercentage < 0)
  {
    soilMoisturePercentage = 0;
  }

  handleSoilMoisturePercentagePrint(
    soilMoistureSensorValue,
    soilMoisturePercentage
  );
  
  return soilMoisturePercentage;
}

void handleWaterPump(int soilMoisturePercentage)
{
  // Turn on/off water pump.
  if(soilMoisturePercentage >= maximumSoilMoisturePercentage)
  {
    digitalWrite(_waterPumpPin, RELAY_OFF);
  }
  else if(soilMoisturePercentage < minimumSoilMoisturePercentage)
  {
    digitalWrite(_waterPumpPin, RELAY_ON);
  }
}

void setup()
{
  Serial.begin(9600);

  // Ensure the digital pin for the water pump is off by default.
  digitalWrite(_waterPumpPin, RELAY_OFF);
  pinMode(_waterPumpPin, OUTPUT);
}

void loop()
{
  handleSoilMoisturePercentageControl();
  
  int soilMoisturePercentage = getCurrentSoilMoisturePercentage();
  
  handleWaterPump(soilMoisturePercentage);
}
