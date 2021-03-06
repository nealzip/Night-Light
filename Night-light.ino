
// Version 1.2.1Test Brighness code. Minor Cleanup.
// Version 1.2 BTest Brighness code.
// Version 1.1 Before reorginizing code.

// This is a demonstration on how to use an input device to trigger changes on your neo pixels.
// You should wire a momentary push button to connect from ground to a digital IO pin.  When you
// press the button it will change to a new pixel animation.  Note that you need to press the
// button once to start the first animation!

#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN   A0    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.

#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 8
#define encoder0PinA  2
#define encoder0PinB  4
#define increment 1

volatile byte brightness = 1;

volatile byte red = 0x80;
volatile byte green = 0x80;
volatile byte blue = 0x80;
volatile byte color=1;

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool oldState = HIGH;
int showType = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.setPixelColor(7,strip.Color(0,0,5));
  strip.show(); // Initialize one pixels to 'Blue'
  strip.setPixelColor(6,strip.Color(0,5,0));
  strip.show(); // Initialize one pixels to 'Green'
  strip.setPixelColor(5,strip.Color(5,0,0));
  strip.show(); // Initialize one pixels to 'Red'
  delay(2000);
  //colorWipe(strip.Color(red, green, blue), 50);
  //strip.show(); // Initialize all pixels to 'RGB' value
  
  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);       // turn on pullup resistor
  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);       // turn on pullup resistor

  attachInterrupt(0, doEncoder, CHANGE);  // encoder pin on interrupt 0 - pin 2
  Serial.begin (9600);
  Serial.println("start");                // a personal quirk
 Serial.println(brightness,HEX);
}

void loop() {
  // Get current button state.
  bool newState = digitalRead(BUTTON_PIN);
  
  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(30);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) {
     /* showType++;
      if (showType > 3)
        showType=0;
      startShow(showType);*/
      changecolor(color);
      color++;
      if (color >= 4) color=1;
    }
  }

  // Set the last button state to the old state.
  oldState = newState;
   Serial.println(brightness,DEC);
   delay(100);
   colorWipe(strip.Color(red * int(brightness *.1), green * int(brightness *.1), blue * int(brightness *.1)), 0);
}

void changecolor(byte i) {
  switch(i){   
    case 1: red=10; green=0; blue=0;  // Red
            break;
    case 2: red=0; green=10; blue=0;  // Green
            break;
    case 3: red=0; green=0; blue=10;  // Blue
            break;  
    }
    colorWipe(strip.Color(red * int(brightness *.1), green * int(brightness *.1), blue * int(brightness *.1)), 0);
}

void startShow(int i) {
  switch(i){
    case 0: colorWipe(strip.Color(red, green, blue), 50);    // Black/off
            break;
    case 1: colorWipe(strip.Color(red, green, blue), 50);  // Red
            break;
    case 2: colorWipe(strip.Color(red,green, blue), 50);  // Green
            break;
    case 3: colorWipe(strip.Color(red,green, blue), 50);  // Blue
            break;
    case 4: rainbowCycle(20);
            break;
  }
}



// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
  Serial.print(red,HEX);
  Serial.print(green,HEX);
  Serial.print(blue,HEX);
}


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}


void doEncoder() {

   /*Serial.print("Start doEncoder: ");
  Serial.println(red,HEX);
  */
  /* If pinA and pinB are both high or both low, it is spinning
   * forward. If they're different, it's going backward.
   *
   * For more information on speeding up this process, see
   * [Reference/PortManipulation], specifically the PIND register.
   */
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    brightness++;
    if (brightness >= 10) brightness=10;
 /*
    //brightness = leftRotate(brightness);
    brightness++;
    if (brightness >= 7) brightness=1;
    red = red >> brightness;
    green = green >> brightness;
    blue = blue >> brightness;
    colorWipe(strip.Color(red, green, blue), 0);
    strip.show();
    */
  } else {
    brightness--;
    if (brightness <= 0) brightness=0;
    /*
    //brightness = rightRotate(brightness);
    brightness--;
    if (brightness <= 0) brightness=8;
    red = red << brightness;
    green = green << brightness;
    blue = blue << brightness;
    colorWipe(strip.Color(red, green, blue), 0);
    strip.show();
   */
  }
  /*
  Serial.print("In doEncoder: ");
  Serial.println(red,HEX);
  Serial.println(green,HEX);
  Serial.println(blue,HEX);

  Serial.println(brightness,HEX);
  
  
  //Serial.println(brightness);
  //Serial.println (brightness, DEC);
  delay(20);
  */
}

/*Function to left rotate N*/
int leftRotate(byte N)
{
   //Rotate Byte Left
   
   return((byte)(N << 1) | (N >> 7));   // rotate left
}
 
/*Function to right rotate N*/
int rightRotate(byte N)
{
   //Rotate Byte Right

  return(N = (byte)(N >> 1) | (N << 7));  // rotate right
}

