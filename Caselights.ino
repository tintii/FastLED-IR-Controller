#include <IRremote.h>

#include <FastLED.h>

#define LED_PIN     2 //pin the LEDS are connected to
#define NUM_LEDS    103 //quantity of leds
#define LED_TYPE    WS2812 //type of leds
#define COLOR_ORDER GRB //colour order
CRGB leds[NUM_LEDS]; //array of leds

uint8_t brightness = 114; //initial brightness
uint8_t colourscheme;
uint8_t colourschemeqty;

//static uint8_t startIndex = 0;

#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 static_WhitePalette;
extern const TProgmemPalette16 static_WhitePalette_p PROGMEM;


//ir stuff

int RECV_PIN = 13; //IR Receiving LED connected to this pin
bool onoff; //defines the current state of the LEDs
unsigned long command; //stores the latest input command
unsigned long lastcommand; //stores the most recent command if the remote button is held and command repeated

IRrecv irrecv(RECV_PIN); //initialise the IR module

decode_results results; //store results from decoding remote signal


void setup() {
    delay( 3000 ); // power-up safety delay
    //Control scheme
    Serial.begin(9600);// Serial Output
    irrecv.enableIRIn(); // Start the IR receiver
    
    //FastLED stuff
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  brightness );
    
    SetupBlackPalette();
    onoff = false;
}


void loop()
{
  
  if (irrecv.decode(&results)) {//if a new command is received from the IR sensor
    
    command = results.value; //store in command variable
    //Serial.println(results.value, HEX); //print result in hex to serial port
    Serial.println(command); //print result to serial port - use to figure out what commands are being sent by your remote
    
 
    //ChangePalettePeriodically();
    //store last command for repetition in case button is held
    if (command != 4294967295){//when button is held on remote, this value is received. If so, repeat the previous command until release.
      lastcommand = command;
    }else{
      command = lastcommand;
    }
  
    switch (command) { //here is where you use the commands received from the remote (seen through serial monitor). Each code must be listed here for anything to happen
      case 16756815: //power button
        if (onoff == false){
          colourscheme=1;
          colourschemecycle(colourscheme);
          onoff = true;
          delay(1000);
        }else{
          colourscheme=0;
          colourschemecycle(colourscheme);
          onoff = false;
        }
        break;
      
      //colour schemes
      case 16748655: //up arrow - change colour scheme
        if (colourscheme < colourschemeqty && colourscheme > 0){
          colourscheme += 1;
        }else if (colourscheme == colourschemeqty){
          colourscheme=1;
        }
        colourschemecycle(colourscheme);
        break;
      case 16758855: //down arrow - change colour scheme
        if (colourscheme > 1){
          colourscheme -= 1;
        }else if (colourscheme == 1){
          colourscheme = colourschemeqty;
        }
        colourschemecycle(colourscheme);
        break;
      
      //brightness controls
      case 16722135: //brightness down
        if (brightness > 25){
          brightness = brightness - 20;
        }
        colourschemecycle(colourscheme);
        break;  
      case 16773135: //brightness up
        if (brightness < 254){
          brightness = brightness + 20;
        }
        colourschemecycle(colourscheme);
        break;  
      
      case 16754775: //auto button
        SetupTotallyRandomPalette();
      
      default:
        //SetupBlackPalette();
        break;
    }
    
    irrecv.resume(); // Receive the next value
  }
    /* for motion of leds - still testing
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; //defines motion speed
    FillLEDsFromPaletteColors( startIndex);*/
    
  FastLED.show();//display the leds

  //FastLED.delay(1000 / UPDATES_PER_SECOND); 
  delay(250);//delay for effect to finish processing. reduce to make smoother, but currently causes issues with remote response
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)//fill the leds with palette colours
{
    //uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

//function to cycle colour schemes.
//Add your colour schemes here.
//Remember to update the colourschemeqty to the largest case number
void colourschemecycle(uint8_t currentscheme)
{
  colourschemeqty = 13;
  switch (currentscheme)
  {
    case 0: fill_solid( currentPalette, 16, CRGB::Black);   currentBlending = LINEARBLEND;  FillLEDsFromPaletteColors( 1);  break;
    case 1: fill_solid( currentPalette, 16, CRGB::White);   currentBlending = LINEARBLEND;  FillLEDsFromPaletteColors( 1);  break;
    case 2: fill_solid( currentPalette, 16, CRGB::Red);     currentBlending = LINEARBLEND;  FillLEDsFromPaletteColors( 1);  break;
    case 3: fill_solid( currentPalette, 16, CRGB::Green);   currentBlending = LINEARBLEND;  FillLEDsFromPaletteColors( 1);  break;
    case 4: fill_solid( currentPalette, 16, CRGB::Blue);    currentBlending = LINEARBLEND;  FillLEDsFromPaletteColors( 1);  break;
    case 5: fill_solid( currentPalette, 16, CRGB::Purple);  currentBlending = LINEARBLEND;  FillLEDsFromPaletteColors( 1);  break;
    case 6: fill_solid( currentPalette, 16, CRGB::Orange);  currentBlending = LINEARBLEND;  FillLEDsFromPaletteColors( 1);  break;
    case 7: currentBlending = LINEARBLEND; fill_gradient(leds,0,CHSV( HUE_AQUA, 255, brightness),NUM_LEDS,CHSV( HUE_GREEN, 255, 255),SHORTEST_HUES);   break;//underwater look
    case 8: currentBlending = LINEARBLEND; fill_gradient(leds,0,CHSV( HUE_RED, 255, brightness),NUM_LEDS,CHSV( HUE_ORANGE, 255, 255),SHORTEST_HUES);   break;//fire look
    case 9: currentBlending = LINEARBLEND; fill_gradient(leds,0,CHSV( HUE_PURPLE, 255, brightness),NUM_LEDS,CHSV( HUE_PINK, 255, 255),SHORTEST_HUES);  break;//ping look
    case 10: SetupN7Palette();                              currentBlending = NOBLEND;      FillLEDsFromPaletteColors( 1);  break;
    case 11: SetupBlackAndWhiteStripedPalette();            currentBlending = NOBLEND;      FillLEDsFromPaletteColors( 1);  break;
    case 12: SetupPurpleAndGreenPalette();                  currentBlending = LINEARBLEND;  FillLEDsFromPaletteColors( 1);  break;
    case 13: SetupRedWhiteBluePalette();                    currentBlending = NOBLEND;      FillLEDsFromPaletteColors( 1);  break;
  }
  
}


void SetupBlackPalette()
{
  fill_solid( currentPalette, 16, CRGB::Black);
}

void SetupN7Palette()
{
  CRGB red = CRGB::Red;
  CRGB white = CRGB::Gray;
  currentPalette = CRGBPalette16{
                                 white, white, red, red,
                                 red, white, white, white,
                                 white, white, red, red,
                                 red, white, white, white
                                 };
}

void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}

void SetupRedWhiteBluePalette()
{
    CRGB red = CRGB::Red;
    CRGB white = CRGB::Gray;
    CRGB blue = CRGB::Blue;
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   red, white, blue,  black,
                                   red, white, blue,  black,
                                   red, white, blue,  black,
                                   red, white, blue,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.

const TProgmemPalette16 static_WhitePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};


// Additionl notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact 
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved 
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.
