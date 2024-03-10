///////// IMPORT EXTERNAL MODULES AND LIBRARIES //////////
// Import the Adafruit NeoPixel library for controlling NeoPixel LEDs
#include <Adafruit_NeoPixel.h>
// Import the avr/power library for power management on AVR microcontrollers
#include <avr/power.h>
// Import a custom or placeholder library for handling the control surface functionality
#include <Control_Surface.h>
// Initialize a USBMIDI_Interface object for handling MIDI over USB
USBMIDI_Interface midi;

///////// INITIALIZATION OF EXTERNAL MODULES AND LIBRARIES //////////
//LEDs
// Define the pin connected to the NeoPixel strip
#define PIN 2
// Define the number of pixels in the NeoPixel strip
#define NUMPIXELS 45
// Initialize the Adafruit_NeoPixel object with the number of pixels and the pin number
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
// Define the delay between pixels for animations
#define DELAYVAL 500  // Time (in milliseconds) to pause between pixels

///////// CONSTANTS DECLARATION //////////
// Define RGB values for each octave
const int COLORS_BY_OCTAVE[9][3] = {
  { 255, 0, 0 },
  { 255, 170, 0 },
  { 170, 255, 0 },
  { 0, 255, 0 },
  { 0, 255, 170 },
  { 0, 170, 255 },
  { 0, 0, 255 },
  { 171, 0, 255 },
  { 255, 0, 170 }
};

// Define active color
const int ACTIVE_COLOR[3] = { 255, 255, 255 };

// Define Mux button count
const int MUX_BUTTON_COUNT = 13;

// Define the minimum and maximum octave values
const int OCTAVE_MIN = 0;                                                           // minimal octave value
const int OCTAVE_MAX = sizeof(COLORS_BY_OCTAVE) / sizeof(COLORS_BY_OCTAVE[0]) - 1;  // maximal octave value

// Define pins for the up and down buttons
const int BUTTON_UP_PIN = 3;    // the number of the pushbutton 1 pin
const int BUTTON_DOWN_PIN = 4;  // the number of the pushbutton 2 pin

// Define the pin and address pins for the multiplexer
const pin_t muxPin = A0;                                  //pin_t so far not necessary
AH::Array<pin_t, 4> muxAddressPins = { 16, 15, 14, 10 };  //pin_t+this specific array is necessary
// Initialize the multiplexer with the pin and address pins
CD74HC4067 mux(muxPin, muxAddressPins);  // initializing multiplexor

///////// VARIABLES DECLARATION //////////
// Define the default octave value
int octave = 4;

// Define variables for reading the button states
int buttonUpState = 0;        // variable for reading the pushbutton 1 status
int buttonDownState = 0;      // variable for reading the pushbutton 2 status
int lastButtonUpState = 0;    // variable for setting the last pushbutton 1 status
int lastButtonDownState = 0;  // variable for setting the last pushbutton 2 status

// Define arrays for button states and last states
int muxButtonStates[13] = { 0 };  // 13 buttons
int lastMuxButtonStates[13] = { 0 };

///////// FUNCTIONS DEFINITIONS //////////
// Function to get the RGB color for the current octave
int* getColorByOctave(int octave) {
  int* result = COLORS_BY_OCTAVE[octave];
  return result;
}

// Function to get pixel indexes for mux button index
int* getPixelIdxssByMuxIdx(int muxtBtnIndex) {
  // Declare an array to hold the pixel indexes
  static int indexes[3];  // Use static to ensure the array remains in scope after the function returns

  // Calculate the pixel indexes based on the mux button index
  indexes[0] = 38 - muxtBtnIndex * 3;
  indexes[1] = 38 - muxtBtnIndex * 3 - 1;
  indexes[2] = 38 - muxtBtnIndex * 3 - 2;

  // Return the address of the first element of the array
  return indexes;
}

// Function to set all pixels to the color of the current octave
void setPixelsColorByOctave(int octave) {
  int* currentColor = getColorByOctave(octave);

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(currentColor[0], currentColor[1], currentColor[2]));
  }  // set the color the current octava value to every LED pixel

  pixels.show();  // Neopixel library function, updates the physical LEDs
}

// Function to set selected pixels according to given state
void setSelectedPixelsColorByState(int* pixelIndexes, int octave, bool active) {
  int* currentOctaveColor = getColorByOctave(octave);
  // if active = true, set pixel colors to white
  if (active) {
    Serial.print("tady by se to melo zmenit-zmacknuto");
    Serial.println();
    for (int i = 0; i < 3; i++) {
      Serial.println(pixelIndexes[i]);
      Serial.println(ACTIVE_COLOR[0]);
      pixels.setPixelColor(pixelIndexes[i], pixels.Color(ACTIVE_COLOR[0], ACTIVE_COLOR[1], ACTIVE_COLOR[2]));
    }
  }
  // if active = false, set pixel colers to octave
  else {
    for (int i = 0; i < 3; i++) {
      pixels.setPixelColor(pixelIndexes[i], pixels.Color(currentOctaveColor[0], currentOctaveColor[1], currentOctaveColor[2]));
    }
  }
  pixels.show();
}

// Function to set active mux buttons back to white
void setMuxPixelsToActiveColor() {
  for (int i = 0; i < MUX_BUTTON_COUNT; i++) {
    if (muxButtonStates[i] == 1) {
      int* pixels = getPixelIdxssByMuxIdx(i);
      setSelectedPixelsColorByState(pixels, octave, true);
    }
  }
}

///////// SETUP - run once at app start //////////
void setup() {
  Serial.begin(9600);  // Initialize serial communication at 9600 bps

  pixels.begin();             // Neopixel library function, sets up the communication with LEDs
  pixels.clear();             // Neopixel library function, clears the color data
  pixels.setBrightness(100);  // ??? setting the brightness of LED pixes (range 0-100)

  // LED blinking through all colors at app start
  for (int i = OCTAVE_MIN; i < OCTAVE_MAX; i++) {
    setPixelsColorByOctave(i);
    delay(100);
  }

  setPixelsColorByOctave(octave);  // set LED color to current octave corresponding color

  pinMode(BUTTON_UP_PIN, INPUT);    // initialize the pushbutton pin as an input
  pinMode(BUTTON_DOWN_PIN, INPUT);  // initialize the pushbutton pin as an input

  mux.begin();  // multiplexor initialize STOP

  Control_Surface.begin();  //STOP

  // Set multiplexer pins as inputs with pull-up resistors
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
}

///////// LOOP - run repeatedly //////////
void loop() {

  // READ BUTTON STATES
  buttonUpState = digitalRead(BUTTON_UP_PIN);      // reads the button state from the physical button
  buttonDownState = digitalRead(BUTTON_DOWN_PIN);  // reads the button state from the physical button

  // Reading MUX button states
  for (int i = 0; i < MUX_BUTTON_COUNT; i++) {
    muxButtonStates[i] = !ExtIO::digitalRead(mux.pin(i + 1));  // the state is being read oposite, fix - invert the value here
  }

  // button up control - OCTAVE UP
  if (buttonUpState == 1 && buttonUpState != lastButtonUpState && octave < OCTAVE_MAX) {
    octave++;

    Serial.print("---Octave UP---");  // print text
    Serial.println(octave);           // print current octave value

    setPixelsColorByOctave(octave);
    // for pressed mux buttons to be changed to white immediately
    setMuxPixelsToActiveColor();
    delay(50);  // delay before next step, alternative function is millis() -> non blocking
  }

  // button down control - OCTAVE DOWN
  if (buttonDownState == 1 && buttonDownState != lastButtonDownState && octave > OCTAVE_MIN) {
    octave--;

    Serial.print("---Octave DOWN---");  // print text
    Serial.println(octave);             // print current octave value

    setPixelsColorByOctave(octave);
    // for pressed mux buttons to be changed to white immediately
    setMuxPixelsToActiveColor();
    delay(50);  // delay before next step, alternative function is millis() -> non blocking
  }

  // Mux buttons - check each button's state and compare with the last state
  for (int i = 0; i < MUX_BUTTON_COUNT; i++) {
    // button is pressed
    if (muxButtonStates[i] == 1 && muxButtonStates[i] != lastMuxButtonStates[i]) {
      int* pixels = getPixelIdxssByMuxIdx(i);
      setSelectedPixelsColorByState(pixels, octave, true);

      Serial.print("mux button was pressed: ");
      Serial.print(i + 1);
      Serial.print(", pixels: ");
      // Assuming the function returns an array of 3 elements
      for (int i = 0; i < 3; i++) {
        Serial.print(pixels[i]);
        if (i < 2) {  // To separate the elements with a space or comma
          Serial.print(", ");
        }
      }
      Serial.println();  // To print a newline at the end
      delay(50);         // Delay to prevent bouncing
    }
    // button is released
    if (muxButtonStates[i] == 0 && muxButtonStates[i] != lastMuxButtonStates[i]) {
      int* pixels = getPixelIdxssByMuxIdx(i);
      setSelectedPixelsColorByState(pixels, octave, false);
      Serial.print("mux button was released: ");
      Serial.print(i + 1);
      Serial.print(", pixels: ");
      // Assuming the function returns an array of 3 elements
      for (int i = 0; i < 3; i++) {
        Serial.print(pixels[i]);
        if (i < 2) {  // To separate the elements with a space or comma
          Serial.print(", ");
        }
      }
      Serial.println();  // To print a newline at the end
      delay(50);         // Delay to prevent bouncing
    }
  }

  // update button last state
  lastButtonUpState = buttonUpState;
  lastButtonDownState = buttonDownState;

  // Update last states for the next loop
  for (int i = 0; i < MUX_BUTTON_COUNT; i++) {
    lastMuxButtonStates[i] = muxButtonStates[i];
  }
}
