#define BUTTON_BREATHE 2
#define BUTTON_LEFT 3
#define BUTTON_MIDDLE 4
#define BUTTON_RIGHT 5

int LEDPins[] = {14, 13, 12, 11, 10, 9, 8, 7, 6}; 
int inputPins[] = {BUTTON_BREATHE, BUTTON_LEFT, BUTTON_MIDDLE, BUTTON_RIGHT};


void setup() {
  for (int i = 1; i < 9; i++){ 
    pinMode(LEDPins[i], OUTPUT); 
    digitalWrite(LEDPins[i], HIGH); 
    delay(20);
    digitalWrite(LEDPins[i], LOW); 
    delay(20);
  }
  for (int i = 0; i < 5; i++){ 
    pinMode(inputPins[i], INPUT); 
  }
  
  //digitalWrite(13, HIGH); 
}

// Writes an articial PWM signal to a pin, dutyCycle is in percentage, and 
// msDuration is the duration this specific duty cycle will last for in ms. 
void writeDutyCycle(int pinName, int dutyCycle, int msDuration){ 
  int elapsedTime = 0; 
  int microsecondDurationOn = dutyCycle*10; 
  while (elapsedTime < msDuration){ 
    digitalWrite(pinName, HIGH); 
    delayMicroseconds(microsecondDurationOn); 
    digitalWrite(pinName, LOW); 
    delayMicroseconds(1000-microsecondDurationOn);
    elapsedTime++; 
  }
}

// Fades the specified LED from 0 to 100% duty cycle with msTimeOn between each 1%
void fadeOnLED(int ledName, int msTimeOn){ 
  for (int i = 0; i < 100; i++){ 
    writeDutyCycle(ledName, i, msTimeOn); 
  }
  digitalWrite(ledName, HIGH); 
}

// Fades the specified LED from 100 to 0% duty cycle with msTimeOn between each 1%
void fadeOffLED(int ledName, int msTimeOff){ 
  for (int i = 100; i > -1; i--){ 
      writeDutyCycle(ledName, i, msTimeOff); 
    }
  digitalWrite(ledName, LOW); 
}

void fadePulseLED(int ledName, int msTimeOn, int msTimeOff){ 
  fadeOnLED(ledName, msTimeOn);
  fadeOffLED(ledName, msTimeOff);  
}

// Lights up LEDs one by one, from left to right, with specified on 
// and off fade speed
void sweepRightOnLED(int length, int msTimeOn, int msTimeOff){ 
  for (int i = 0; i < length; i++){ 
    if (i < length-1){
      fadePulseLED(LEDPins[i], msTimeOn, msTimeOff); 
    } else { 
      fadeOnLED(LEDPins[i], msTimeOn); 
    }
  }
}

// Turns off LEDs one by one, from right to left, with specified on
// and off fade speed
void sweepLeftOffLED(int length, int msTimeOn, int msTimeOff){ 
  for (int i = length; i > -1; i--){ 
    if (i >= length-1){ 
      fadeOffLED(LEDPins[i], msTimeOff); 
    } else {
      fadePulseLED(LEDPins[i], msTimeOn, msTimeOff); 
    }
  }
}

// Activated when the "breathe" button is pressed, will loop until 
// interrupted by the middle button. 
void breathe_mode(){ 
  while (1){
    for (int i = 1; i < 9; i++){ 
      fadeOnLED(LEDPins[i], 5);
    }
    delay(1000); 
    for (int i = 8; i > 0; i--){ 
      fadeOffLED(LEDPins[i], 5);
    }
    delay(1000);
  }
}

// Turns off all LEDs
void clearLED(){ 
  for (int i = 0; i < 9; i++){ 
    digitalWrite(LEDPins[i], LOW); 
  }
}

// In this mode, the left button lights up the LED to the left
// of the one currently on, and the right does the opposite. 
// This can probably be better implemented using a circular 
// doubly linked list. 
void scrollingLED(int startingLED){ 
  digitalWrite(LEDPins[startingLED], HIGH); 
  int lastOn = startingLED; 
  delay(300); 
  while(1){ 
    if (digitalRead(BUTTON_LEFT)){ 
      lastOn = moveLightLeft(lastOn); 
      delay(300); 
    } else if (digitalRead(BUTTON_RIGHT)){ 
      lastOn = moveLightRight(lastOn); 
      delay(300); 
    }
  }
}

// Turns on the LED to the left of the currently on LED, and 
// returns the index of the LED that was turned on. 
int moveLightLeft(int lastOn){ 
  digitalWrite(LEDPins[lastOn], LOW); 
  int nextToTurnOn = lastOn-1 < 1 ? 8 : lastOn-1; 
  digitalWrite(LEDPins[nextToTurnOn], HIGH); 
  return nextToTurnOn; 
}

// Turns on the LED to the right of the currently on LED, and 
// returns the index of the LED that was turned on. 
int moveLightRight(int lastOn){ 
  digitalWrite(LEDPins[lastOn], LOW); 
  int nextToTurnOn = lastOn+1>8 ? 1 : lastOn+1; 

  digitalWrite(LEDPins[nextToTurnOn], HIGH); 
  return nextToTurnOn; 
}

/* Scrolls lights from left to right, leaving a fixed light
untouched. If the middle button is pressed right before the fixed light
turns on, the function returns 1. Otherwise returns 0. */

int scrollingLightGame(int timeGap, int fixedLight){ 
  for (int i = 1; i < 9; i++){
    if (i == fixedLight){ 
      if (digitalRead(BUTTON_MIDDLE)){ 
        digitalWrite(LEDPins[fixedLight], LOW); 
        return 1;
      }
      delay(timeGap); 
      continue; 
    }
    digitalWrite(LEDPins[i], HIGH); 
    delay(timeGap);
    digitalWrite(LEDPins[i], LOW); 
  } 

  for (int i = 8; i > 0; i--){ 
    if (i == fixedLight){ 
      if (digitalRead(BUTTON_MIDDLE)){ 
        digitalWrite(LEDPins[fixedLight], LOW); 
        return 1;
      }
      delay(timeGap); 
      continue; 
    }
    digitalWrite(LEDPins[i], HIGH); 
    delay(timeGap);
    digitalWrite(LEDPins[i], LOW); 
  }

  return 0;

}

int gameLightDuration = 100; 

void loop() {
  if (digitalRead(BUTTON_BREATHE)){ 
    delay(20); 
    breathe_mode(); 
  }

  if (digitalRead(BUTTON_LEFT)){ 
    scrollingLED(8); 
  } else if (digitalRead(BUTTON_RIGHT)){ 
    scrollingLED(1); 
  }

  /* Plays a game where user must press button when the scrolling lights
  land on a particular light. Gets faster each time the user succeeds, until
  the speed is too fast. Then a win condition is met and all lights will blink
  until the reset button is pressed. */ 
  if (digitalRead(BUTTON_MIDDLE)){ 
    int fixedLight = rand() % 8 + 1; 
    digitalWrite(LEDPins[fixedLight], HIGH); 
    delay(500); 
    while (1){
      if (scrollingLightGame(gameLightDuration, fixedLight)) { 
        gameLightDuration -= 5; 
        break; 
      }
    }
    if (gameLightDuration == 0){ 
      while (1){ 
        for (int i = 1; i < 9; i++){ 
          pinMode(LEDPins[i], OUTPUT); 
          digitalWrite(LEDPins[i], HIGH); 
        }
        delay (500); 
        for (int i = 1; i < 9; i++){ 
          pinMode(LEDPins[i], OUTPUT); 
          digitalWrite(LEDPins[i], LOW); 
        }
        delay(500); 
      }
    }
    delay(50); 
  }

}