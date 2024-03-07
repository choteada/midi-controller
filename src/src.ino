///////// IMPORT EXTERNAL MODULES AND LIBRARIES //////////
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <Control_Surface.h>
USBMIDI_Interface midi;

///////// INITIALIZATION OF EXTERNAL MODULES AND LIBRARIES //////////
//LEDs
#define PIN            2
#define NUMPIXELS      45
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

///////// CONSTANTS DECLARATION //////////
const int colorsByOctave[9][3] = {
  { 255, 0, 0 },
  { 255, 170, 0 },
  { 170, 255, 0 },
  { 0, 255, 0 },
  { 0, 255, 170 },
  { 0, 170, 255 },
  { 0, 0, 255 },
  { 171, 0, 255 },
  { 255, 0, 170 }
};                          // rgb values for each octave

const int octaveMin = 0;    // minimal octave value
const int octaveMax = 8;    // maximal octave value

const int buttonUPPin = 3;     // the number of the pushbutton 1 pin
const int buttonDOWNPin = 4;     // the number of the pushbutton 2 pin

const pin_t muxPin = A0;                                           //pin_t so far not necessary
AH::Array<pin_t, 4> muxAddressPins = {16, 15, 14, 10};            //pin_t+this specific array is necessary
//const int muxAddressPins[4] = {16, 15, 14, 10};                 // old try to write an array
CD74HC4067 mux(muxPin, muxAddressPins);                           // initializing multiplexor

///////// VARIABLES DECLARATION //////////
int octave = 4;                 // starting octave value

int buttonUpState = 0;           // variable for reading the pushbutton 1 status
int buttonDownState = 0;         // variable for reading the pushbutton 2 status
int lastButtonUpState = 0;       // variable for setting the last pushbutton 1 status
int lastButtonDownState = 0;     // variable for setting the last pushbutton 2 status

// MUX states definition
int MUX_C13_STATE = 0;
int MUX_C12_STATE = 0;
int MUX_C11_STATE = 0;
int MUX_C10_STATE = 0;
int MUX_C9_STATE  = 0;
int MUX_C8_STATE  = 0;
int MUX_C7_STATE  = 0;
int MUX_C6_STATE  = 0;
int MUX_C5_STATE  = 0;
int MUX_C4_STATE  = 0;
int MUX_C3_STATE  = 0;
int MUX_C2_STATE  = 0;
int MUX_C1_STATE  = 0;

// MUX laststates definition
int MUX_C13_LASTSTATE = 0;
int MUX_C12_LASTSTATE = 0;
int MUX_C11_LASTSTATE = 0;
int MUX_C10_LASTSTATE = 0;
int MUX_C9_LASTSTATE  = 0;
int MUX_C8_LASTSTATE  = 0;
int MUX_C7_LASTSTATE  = 0;
int MUX_C6_LASTSTATE  = 0;
int MUX_C5_LASTSTATE  = 0;
int MUX_C4_LASTSTATE  = 0;
int MUX_C3_LASTSTATE  = 0;
int MUX_C2_LASTSTATE  = 0;
int MUX_C1_LASTSTATE  = 0;

///////// FUNCTIONS DEFINITIONS //////////
int* getColorByOctave(int octave) {
  int* result = colorsByOctave[octave];
  return result;
}                                                // select the color from array library according to current octave value

void setPixelsColorByOctave(int octave) {
  int* currentColor = getColorByOctave(octave);

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(currentColor[0], currentColor[1], currentColor[2]));
  }                                              // set the color the current octava value to every LED pixel

  pixels.show();                                 // Neopixel library function, updates the physical LEDs
}

///////// SETUP - run once at app start //////////
void setup() {

  pixels.begin();                             // Neopixel library function, sets up the communication with LEDs
  pixels.clear();                             // Neopixel library function, clears the color data
  pixels.setBrightness(100);                  // ??? setting the brightness of LED pixes (range 0-100)

  // LED blinking through all colors at app start
  for (int i = octaveMin; i < octaveMax; i++) {
    setPixelsColorByOctave(i);
    delay(100);
  }

  setPixelsColorByOctave(octave); // set LED color to current octave corresponding color

  pinMode(buttonUPPin, INPUT);   // initialize the pushbutton pin as an input
  pinMode(buttonDOWNPin, INPUT); // initialize the pushbutton pin as an input

  mux.begin();              // multiplexor initialize STOP

  Control_Surface.begin();         //STOP

  ExtIO::pinMode(mux.pin(13), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(12), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(11), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(10), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(9), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(8), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(7), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(6), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(5), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(4), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(3), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(2), INPUT_PULLUP);
  ExtIO::pinMode(mux.pin(1), INPUT_PULLUP);

  Serial.begin(9600);
}

///////// LOOP - run repeatedly //////////
void loop() {

  buttonUpState = digitalRead(buttonUPPin);                             // reads the button state from the physical button
  buttonDownState = digitalRead(buttonDOWNPin);                         // reads the button state from the physical button

  MUX_C13_STATE = ExtIO::digitalRead(mux.pin(13));
  MUX_C12_STATE = ExtIO::digitalRead(mux.pin(12));
  MUX_C11_STATE = ExtIO::digitalRead(mux.pin(11));
  MUX_C10_STATE = ExtIO::digitalRead(mux.pin(10));
  MUX_C9_STATE  = ExtIO::digitalRead(mux.pin(9));
  MUX_C8_STATE  = ExtIO::digitalRead(mux.pin(8));
  MUX_C7_STATE  = ExtIO::digitalRead(mux.pin(7));
  MUX_C6_STATE  = ExtIO::digitalRead(mux.pin(6));
  MUX_C5_STATE  = ExtIO::digitalRead(mux.pin(5));
  MUX_C4_STATE  = ExtIO::digitalRead(mux.pin(4));
  MUX_C3_STATE  = ExtIO::digitalRead(mux.pin(3));
  MUX_C2_STATE  = ExtIO::digitalRead(mux.pin(2));
  MUX_C1_STATE  = ExtIO::digitalRead(mux.pin(1));

  // button up control
  if (buttonUpState == 1 && buttonUpState != lastButtonUpState && octave < octaveMax) {
    octave++;

    Serial.print("---Octave UP---");                             // print text
    Serial.println(octave);                                      // print current octave value

    setPixelsColorByOctave(octave);
    delay(50);                                                   // delay before next step, alternative function is millis() -> non blocking
  }

  // button down control
  if (buttonDownState == 1 && buttonDownState != lastButtonDownState && octave > octaveMin) {
    octave--;

    Serial.print("---Octave DOWN---");                           // print text
    Serial.println(octave);                                      // print current octave value

    setPixelsColorByOctave(octave);
    delay(50);                                                   // delay before next step, alternative function is millis() -> non blocking
  }

  // MUX buttons monitoring

  if (MUX_C13_STATE == 1 && MUX_C13_STATE != MUX_C13_LASTSTATE) {

    Serial.print("---MUX_C13---");                           // print text
    Serial.println(MUX_C13_STATE);
    delay(50);
  }

  // update button last state
  lastButtonUpState = buttonUpState;
  lastButtonDownState = buttonDownState;

  MUX_C13_LASTSTATE = MUX_C13_STATE;
  MUX_C12_LASTSTATE = MUX_C12_STATE;
  MUX_C11_LASTSTATE = MUX_C11_STATE;
  MUX_C10_LASTSTATE = MUX_C10_STATE;
  MUX_C9_LASTSTATE = MUX_C9_STATE;
  MUX_C8_LASTSTATE = MUX_C8_STATE;
  MUX_C7_LASTSTATE = MUX_C7_STATE;
  MUX_C6_LASTSTATE = MUX_C6_STATE;
  MUX_C5_LASTSTATE = MUX_C5_STATE;
  MUX_C4_LASTSTATE = MUX_C4_STATE;
  MUX_C3_LASTSTATE = MUX_C3_STATE;
  MUX_C2_LASTSTATE = MUX_C2_STATE;
  MUX_C1_LASTSTATE = MUX_C1_STATE;
}
