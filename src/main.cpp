#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "pinmap.h"
#include "pitches.h"
#include "button.h"
#include "graphics.h"
#include "settings.h"
#include "view.h"

int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4}; // notes in the melody:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};                                    // note durations: 4 = quarter note, 8 = eighth note, etc.:

// void testDash(void)
// {
//   display.clearDisplay();

//   display.setTextSize(1);
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(5, 5);
//   display.print(F("Temperature:"));

//   display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);

//   for (int i = 0; i < 100; i++)
//   {
//     display.setCursor(75, 5);
//     display.printf(F("%3d C"), i);
//     display.fillRect(5, SCREEN_HEIGHT / 2, i, 5, SSD1306_WHITE);
//     display.display();
//     delay(3);
//   }

//   for (int i = 0; i < 100; i++)
//   {
//     display.setCursor(75, 5);
//     display.printf(F("%3d C"), i);
//     display.fillRect(105 - i, SCREEN_HEIGHT / 2, i, 5, SSD1306_BLACK);
//     display.display();
//     delay(10);
//   }
// }

void playMelody(void)
{
  for (int thisNote = 0; thisNote < 8; thisNote++)
  {
    // to calculate the note duration, take one second divided by the note type.
    // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    noTone(BUZZER);
  }
}

// void test_screen_update(void)
// {

//   analogWrite(HEAT1, 5);
//   analogWriteFrequency(20);
//   analogWrite(HEAT2, 5);

//   display.setTextSize(1);
//   display.setCursor(0, 0);
//   display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);

//   int counts = analogRead(SENSE_24V);
//   float vin = 3.3f;
//   float R1 = 10e3;
//   float R2 = 1e3;
//   float v = counts * vin / 1024;
//   v = (v * (R1 + R2)) / R2;
//   display.printf(F("24V: %.2f V (%d)"), v, counts);

//   display.setCursor(0, 15);
//   counts = analogRead(NTC);
//   v = counts * vin / 1024;
//   R1 = 4700;
//   R2 = (v * R1) / (vin - v);
//   display.printf(F("NTC: %.0f ohm (%d)"), R2, counts);

//   display.display();
// }

void setup()
{
  settings_init();

  pinMode(LED_B, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(NTC, INPUT_ANALOG);
  pinMode(SENSE_24V, INPUT);
  pinMode(HEAT1, OUTPUT);
  pinMode(HEAT2, OUTPUT);

  button_init();
  view_init();
  playMelody();

  // testDash();
  // timer_screen_setup();

  // display.clearDisplay();
}

void loop()
{
  delay(20);
  digitalToggle(LED_R);
  button_update();
  view_update();
  // test_screen_update();
  // timer_screen_update();
}

// degree symbol
// display.print((char)247);