#include <TimeLib.h>

const int tempoPin = 12;
const int ledPin =  2; 
const int powerPin = 11;
const int analogPin = 10;

int tempoState = 0;
int lastTempoState = 0;
bool tempoStateChange = false;

int powerState = 0;
int lastPowerState = 0;
bool power = true;

int analogState = 0;
int lastAnalogState = 0;
bool analogToggle = false;

int buttonCount = 0;
int buttonCountPrev = 0;

double timePrevious;
double timeSinceOff = 0;
//double time_since_tempo_change = 0;
//double average_period = 250;

int i = 0;
int newRhythm[4] = {250, 250, 250, 250};
int oldRhythm[4] = {250, 250, 250, 250};

int tempo = 250;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(tempoPin, INPUT);
  pinMode(powerPin, INPUT);
  pinMode(analogPin, INPUT);
  Serial.begin(9600);
}

void loop() {

  //Read button state and count how many button presses there were
  tempoState = digitalRead(tempoPin);
  tempoStateChange = tempoState == HIGH && lastTempoState == LOW;
  if (tempoStateChange) {
    buttonCount++;
    }
  lastTempoState = tempoState;


  //power button
  powerState = digitalRead(powerPin);
  if (powerState == HIGH && lastPowerState == LOW) {
    power = !power;
    timeSinceOff = millis() - 3000;  // <-- turn LED on right when press on
    i = 0;                            // <-- reset the index
    }
  lastPowerState = powerState;


  //toggle analog button
  analogState = digitalRead(analogPin);
  if (analogState == HIGH && lastAnalogState == LOW) {
    analogToggle = !analogToggle;
    }
  lastAnalogState = analogState;



  //Do period calculations <-------------------- average period
//  if (!buttonCount) {
//    timePrevious = millis();
//    }
//  else if (buttonCount == 4) {
//    average_period = millis()/4 - timePrevious/4;
//    buttonCount = 0;
//    //time_since_tempo_change = millis();
//    }
//    Serial.println(average_period);
//    Serial.println(buttonCount);

  //Bounds
//  if (average_period < 250) { //240 bpm
//    average_period = 250;
//    } else if (average_period > 30000) { //20 bpm
//      average_period = 30000;
//      }

  if (Serial.available() > 0) {
    tempo = Serial.parseInt();
    Serial.println(tempo);
    if (tempo < 250) { //240 bpm
      tempo = 250;
      } else if (tempo > 3000) { //20 bpm
        tempo = 30000;
        }
    newRhythm[0] = tempo;
    newRhythm[1] = tempo;
    newRhythm[2] = tempo;
    newRhythm[3] = tempo;
    }
  else {
    buttonCount = millis() - timePrevious > 3000 ? 0 : buttonCount;
    if (!buttonCount) {
      timePrevious = millis();
      }
    else if (buttonCount < 4 && tempoStateChange) {
      oldRhythm[buttonCount - 1] = millis() - timePrevious;
      timePrevious = millis();
      }
    else if (buttonCount == 4) {
      oldRhythm[3] = millis() - timePrevious;
      buttonCount = 0;
      newRhythm[0] = oldRhythm[0]; 
      newRhythm[1] = oldRhythm[1]; 
      newRhythm[2] = oldRhythm[2]; 
      newRhythm[3] = oldRhythm[3]; 
      }

    if (buttonCount != buttonCountPrev) {
      Serial.println(buttonCount);
    }
    buttonCountPrev = buttonCount;
  }

  if (power) {
    if (analogToggle) {
      if (millis() - timeSinceOff >= newRhythm[i]) {
        timeSinceOff = millis();
        i = i == 3 ? 0 : i + 1;
        }
      analogWrite(ledPin, 255*((millis() - timeSinceOff)/newRhythm[i]));
      Serial.println(millis() - timeSinceOff);
      }
    else if (millis() - timeSinceOff >= newRhythm[i]) {
        digitalWrite(ledPin, HIGH);             
        delay (100);
        digitalWrite(ledPin, LOW);
        delay (100);
        timeSinceOff = millis();
        i = i == 3 ? 0 : i + 1;
    }
  }
}




