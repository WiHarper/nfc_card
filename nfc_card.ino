const int powerBypassPin = PIN_PC0;
const int powerLedPin = PIN_PB0;


struct LedMap {
  uint8_t anodeIdx;
  uint8_t cathodeIdx;
};


const LedMap leds[20] = {
  {1, 4}, {4, 1}, {0, 4}, {4, 0}, {0, 1}, {1, 0}, {1, 2}, {2, 1},
  {2, 3}, {3, 2}, {3, 4}, {4, 3}, {0, 2}, {2, 0}, {1, 3}, {3, 1},
  {2, 4}, {4, 2}, {0, 3}, {3, 0}  
};


struct LedMasks {
  uint8_t dir_mask;
  uint8_t out_mask;
};
LedMasks ledMasks[20];


// Marquee tracking variables
uint8_t circleHead = 0;
uint16_t frameCounter = 0;
unsigned long setupEndTime = 0;


float powerBrightness = 127.0f;
float breatheStep = -0.25f;


#define MORSE_UNIT 170


struct MorseElement {
  bool pinState;
  uint8_t units;
};

// easter egg
const MorseElement morseSequence[] = {
  {true, 1}, {false, 1}, {true, 3}, {false, 1}, {true, 3}, {false, 3},
  {true, 1}, {false, 1}, {true, 1}, {false, 3},
  {true, 1}, {false, 1}, {true, 3}, {false, 1}, {true, 1}, {false, 1}, {true, 1}, {false, 3},
  {true, 1}, {false, 1}, {true, 1}, {false, 1}, {true, 1}, {false, 3},
  {true, 3}, {false, 1}, {true, 3}, {false, 1}, {true, 3}, {false, 3},
  {true, 3}, {false, 1}, {true, 1}, {false, 3},
  {true, 1}, {false, 1}, {true, 1}, {false, 1}, {true, 1}, {false, 1}, {true, 1}, {false, 3},
  {true, 1}, {false, 1}, {true, 3}, {false, 3},
  {true, 1}, {false, 1}, {true, 3}, {false, 1}, {true, 1}, {false, 3},
  {true, 1}, {false, 1}, {true, 3}, {false, 1}, {true, 3}, {false, 1}, {true, 1}, {false, 3},
  {true, 1}, {false, 3},
  {true, 1}, {false, 1}, {true, 3}, {false, 1}, {true, 1}, {false, 3},
  {true, 1}, {false, 1}, {true, 3}, {false, 1}, {true, 1}, {false, 1}, {true, 3}, {false, 1}, {true, 1}, {false, 1}, {true, 3}, {false, 3},
  {true, 3}, {false, 1}, {true, 1}, {false, 3},
  {true, 1}, {false, 3},
  {true, 3}, {false, 7}
};


const uint8_t morseLength = sizeof(morseSequence) / sizeof(morseSequence[0]);
uint8_t morseIndex = 0;
unsigned long lastMorseTransition = 0;
bool morseEngineInitialized = false;


void setup() {
  // in-rush prevention
  delay(300);


  // enable bypass MOSFET
  pinMode(powerBypassPin, OUTPUT);
  digitalWrite(powerBypassPin, LOW);


  // enable power LED
  pinMode(powerLedPin, OUTPUT);
  analogWrite(powerLedPin, 0);
 
  // pre-calculate hardware bitmasks
  for (int i = 0; i < 20; i++) {
    uint8_t aMask = (1 << (leds[i].anodeIdx + 1));
    uint8_t cMask = (1 << (leds[i].cathodeIdx + 1));
    ledMasks[i].dir_mask = aMask | cMask;          
    ledMasks[i].out_mask = aMask;                  
  }


  PORTB.DIRCLR = 0x3E;


  uint32_t startupStart = millis();


  while (millis() - startupStart < 300) {
    uint32_t elapsed = millis() - startupStart;
    uint16_t blankingDelay = (300 - elapsed) * 25;
   
    renderMatrixCometTail(0, 0);
   
    if (blankingDelay > 0) {
      delayMicroseconds(blankingDelay);
    }
   
    uint8_t powerFade = (elapsed * 127) / 300;
    analogWrite(powerLedPin, powerFade);
  }


  circleHead = 0;
  frameCounter = 0;
  setupEndTime = millis();
}


void loop() {
  renderMatrixCometTail(circleHead, frameCounter);


  frameCounter++;
  if (frameCounter >= 67) {
    circleHead = (circleHead + 1) % 20;
    frameCounter = 0;
  }


  unsigned long currentMillis = millis();


  if (currentMillis - setupEndTime < 10000) {
   
    // power LED breathing
    powerBrightness += breatheStep;
    if (powerBrightness >= 127.0f) {
      powerBrightness = 127.0f;
      breatheStep = -breatheStep;
    } else if (powerBrightness <= 0.0f) {
      powerBrightness = 0.0f;
      breatheStep = -breatheStep;
    }
    analogWrite(powerLedPin, (int)powerBrightness);


  } else {
   
    // easter egg
    if (!morseEngineInitialized) {
      lastMorseTransition = currentMillis;
      morseIndex = 0;
      analogWrite(powerLedPin, morseSequence[morseIndex].pinState ? 127 : 0);
      morseEngineInitialized = true;
    }


    unsigned long requiredDuration = (unsigned long)morseSequence[morseIndex].units * MORSE_UNIT;


    if (currentMillis - lastMorseTransition >= requiredDuration) {
      morseIndex = (morseIndex + 1) % morseLength;
      lastMorseTransition = currentMillis;
     
      analogWrite(powerLedPin, morseSequence[morseIndex].pinState ? 127 : 0);
    }
  }
}


void renderMatrixCometTail(uint8_t headIdx, uint16_t fCount) {
  for (uint8_t i = 0; i < 20; i++) {
    uint8_t dist = (headIdx - i + 20) % 20;


    if (dist >= 17) {
      continue;
    }


    if (dist == 16) {
      if (fCount >= 50) continue;
      else if (fCount >= 34 && (fCount & 3) != 0) continue;
      else if (fCount >= 17 && (fCount & 1) != 0) continue;
    }


    uint8_t dm = ledMasks[i].dir_mask;
    uint8_t om = ledMasks[i].out_mask;


    PORTB.OUTCLR = dm;
    PORTB.OUTSET = om;
    PORTB.DIRSET = dm;


    switch (dist) {
      case 0:
      case 1:
      case 2:  __builtin_avr_delay_cycles(100); break;
      case 3:  __builtin_avr_delay_cycles(70);  break;
      case 4:  __builtin_avr_delay_cycles(48);  break;
      case 5:  __builtin_avr_delay_cycles(32);  break;
      case 6:  __builtin_avr_delay_cycles(22);  break;
      case 7:  __builtin_avr_delay_cycles(15);  break;
      case 8:  __builtin_avr_delay_cycles(10);  break;
      case 9:  __builtin_avr_delay_cycles(7);   break;
      case 10: __builtin_avr_delay_cycles(5);   break;
      case 11: __builtin_avr_delay_cycles(4);   break;
      case 12: __builtin_avr_delay_cycles(3);   break;
      case 13: __builtin_avr_delay_cycles(2);   break;
      case 14: __builtin_avr_delay_cycles(1);   break;
      case 15: __builtin_avr_delay_cycles(1);   break;
      case 16: __builtin_avr_delay_cycles(1);   break;
    }


    PORTB.DIRCLR = dm;
  }
}

