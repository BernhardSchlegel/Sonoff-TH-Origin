#include <Arduino.h>
#include <jled.h>
#include "Button2.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define GPIO_OUT_RELAY 19
#define GPIO_OUT_RELAY_2 22
#define GPIO_IN_BUTTON 0
#define GPIO_ONE_WIRE_BUS 25 // tried 1, 2, 3, 4, 5, 14, 17, 18, 21, 23, 25, 26, 27, 32, 33
#define GPIO_OUT_POWER_RJ11 27 // tried 1, 2, 3, 4, 5, 14, 17, 18, 21, 23, 25, 26, 27, 32, 33

// Data wire is plugged into GPIO4 on ESP32
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(GPIO_ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
// arrays to hold device address
DeviceAddress insideThermometer;

Button2 button;

int pinsToTry[] = {
    // 0, // Button
    // 1, // TX
    // 2,
    // 3, // RX
    // 4,
    // 5,
    // 12,
    13, // Green strange symbol (21s)
    // 14,
    15, // blue wifi
    16, // red on/off
    // 17,
    // 18,
    // 19, // Relay 1
    // 21,
    // 22, // Relay 2
    // 23,
    // 25,
    // 26,
    // 27,
    // 32,
    // 33,
};
int currentIdx = 0;
int currentGPIO = pinsToTry[currentIdx];

auto led_breathe = JLed(currentGPIO).Blink(500, 500).Repeat(5).DelayAfter(500);

void flashLed(int gpio)
{
  led_breathe = JLed(gpio).Blink(300, 200).Repeat(5).DelayAfter(500);
}

// function to print the temperature for a device
float celsius = -42;
void printTemperature(DeviceAddress deviceAddress)
{
  celsius = sensors.getTempC(deviceAddress);
  if (celsius == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(celsius);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(celsius)); // Converts celsius to Fahrenheit
}

bool manualState = false;
void click(Button2 &btn)
{
  Serial.print("clicked: ");
  if (manualState) {
    digitalWrite(GPIO_OUT_RELAY_2, 0);
    digitalWrite(GPIO_OUT_RELAY, 1);
    manualState = false;
    Serial.println(" now OFF");
  } else
  {
    digitalWrite(GPIO_OUT_RELAY_2, 1);
    digitalWrite(GPIO_OUT_RELAY, 0);
    manualState = true;
    Serial.println(" now ON");
  }

  sensors.requestTemperatures(); // Send the command to get temperatures
  printTemperature(insideThermometer);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16)
      Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void setup()
{
  delay(2000);
  Serial.begin(115200);
  Serial.println("Setup called!");

  // relay
  pinMode(GPIO_OUT_RELAY_2, OUTPUT); // Setzt den Digitalpin 13 als Outputpin
  pinMode(GPIO_OUT_RELAY, OUTPUT);   // Setzt den Digitalpin 13 als Outputpin

  // button
  button.begin(GPIO_IN_BUTTON);
  button.setClickHandler(click);

  // DS18B20
  // enable power on RJ 11
  pinMode(GPIO_OUT_POWER_RJ11, OUTPUT); // Setzt den Digitalpin 13 als Outputpin
  digitalWrite(GPIO_OUT_POWER_RJ11, 1);
  // locate devices on the bus
  // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
  Serial.print("Locating devices...");
  sensors.begin();
  delay(750);
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");
  if (!sensors.getAddress(insideThermometer, 0))
    Serial.println("Unable to find address for Device 0");

  Serial.print("2nd try Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);
}

void loop()
{
  button.loop();
  led_breathe.Update();

  if (!led_breathe.IsRunning())
  {
    led_breathe.Off().Update();
    currentIdx += 1;
    size_t n = sizeof(pinsToTry) / sizeof(pinsToTry[0]);
    if (currentIdx >= n)
    {
      Serial.print("starting from the beginning...");
      currentIdx = 0;
    }
    currentGPIO = pinsToTry[currentIdx];

    // effect is done, try next one
    Serial.print("trying GPIO");
    Serial.print(currentGPIO);
    Serial.println("...");
    flashLed(currentGPIO);
  }
}