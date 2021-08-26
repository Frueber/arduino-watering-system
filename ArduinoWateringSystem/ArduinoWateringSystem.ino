#include <LiquidCrystal.h>

// Initialize the LiquidCrystal library by associating any needed LCD interface pin with the arduino pin number it is connected to.
const byte rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

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
int soilMoisturePercentage = 0;
int soilMoistureSensorValue = 0;

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

void handleSoilMoisturePercentagePrint()
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

void setCurrentSoilMoisturePercentage()
{
  soilMoistureSensorValue = analogRead(_soilMoistureSensorPin);

  soilMoisturePercentage = map(
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

  handleSoilMoisturePercentagePrint();
}

void handleWaterPump()
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

/*
 * The LCD screen has 2 rows with 16 characters each.  
 * The first row will display the current soil moisture sensor reading.  
 * The second row will display the currently set minimum and maximum soil moisture values.  
 */
void handleLcd()
{
  lcd.home();
  lcd.print("Current:");
  lcd.print(soilMoisturePercentage);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Min:");
  lcd.print(minimumSoilMoisturePercentage + "%");
  lcd.print("%");
  lcd.print("Max:");
  lcd.print(maximumSoilMoisturePercentage);
  lcd.print("%");
}

void setup()
{
  Serial.begin(9600);

  // Ensure the digital pin for the water pump is off by default.
  digitalWrite(_waterPumpPin, RELAY_OFF);
  pinMode(_waterPumpPin, OUTPUT);

  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
}

void loop()
{
  handleSoilMoisturePercentageControl();
  
  setCurrentSoilMoisturePercentage();
  
  handleWaterPump();

  handleLcd();
}
