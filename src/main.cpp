#include <Arduino.h>
#include <jled.h>
#include "Button2.h"


#define GPIO_OUT_RELAY 19
#define GPIO_OUT_RELAY_2 22
#define GPIO_IN_BUTTON 0

Button2 button;

int pinsToTry[] = {
    //1,
    //2,
    //3,
    //4,
    //5,
    //12,
    13, // Green strange symbol (21s)
    //14,
    15, // blue wifi
    16, // red on/off
    //17,
    //18,
    //19,
    //21,
    //22,
    //23,
    //25,
    //26,
    //27,
    //32,
    //33,
};
int currentIdx = 0;
int currentGPIO = pinsToTry[currentIdx];

auto led_breathe = JLed(currentGPIO).Blink(500, 500).Repeat(5).DelayAfter(500);

void flashLed(int gpio)
{
  led_breathe = JLed(gpio).Blink(300, 200).Repeat(5).DelayAfter(500);
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
  led_breathe.Update();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Setup called!");

  // relay
  pinMode(GPIO_OUT_RELAY_2, OUTPUT); // Setzt den Digitalpin 13 als Outputpin
  pinMode(GPIO_OUT_RELAY, OUTPUT);   // Setzt den Digitalpin 13 als Outputpin

  // button
  button.begin(GPIO_IN_BUTTON);
  button.setClickHandler(click);
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