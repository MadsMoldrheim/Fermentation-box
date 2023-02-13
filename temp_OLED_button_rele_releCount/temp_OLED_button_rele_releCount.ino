/*
  Sources
  https://www.instructables.com/id/Monochrome-096-i2c-OLED-display-with-arduino-SSD13/
  https://randomnerdtutorials.com/guide-for-ds18b20-temperature-sensor-with-arduino/
  https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
*/
/*********millisDelay init******
#include <millisDelay.h>
millisDelay heatDelay;*/

/*********OLED init*******/
//SCK -> A5
//SDA -> A4
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};

/******** temp init*****/
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

/*****own init*****/
int current_temp = 0;
int max_temp = 0;
int min_temp = 0;
int i = 0;
int Ulim = 60;
int Llim = 15;
int heatDelay = 0;
int releCount = 0;
bool error = false;
#define rele 7


void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(3), buttonpress, RISING);
  i = 5;
  pinMode(rele, OUTPUT);

  /******millisDelay setup*****/
  

  /******OLED setup*****/
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(0); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(300);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  /******temp setup*****/
  sensors.begin();
  sensors.requestTemperatures();
  current_temp = (sensors.getTempCByIndex(0));
  min_temp = current_temp;
  max_temp = current_temp;
}

void(* resetFunc) (void) = 0;

void loop() {
  temp_update();
  if (current_temp > Ulim && heatDelay == 0) {
    digitalWrite(rele, LOW);
    releCount ++;
    heatDelay = 60*15; //60*15 gir 15 minutter med delayet i bunnen av loopen
  }
  else if (current_temp < Llim && heatDelay == 0) {
    digitalWrite(rele, LOW);
    error = true;
  }
  else if (heatDelay == 0 &&  error != true) {
    digitalWrite(rele, HIGH);
  }
  
  if (i > 0) {
    drawtemp();
    i --;
  }
  else {
    clear_display_function();
  }

  if (heatDelay > 0) {
    heatDelay --;
  }

  if (current_temp < 50 && max_temp > 55) {
    resetFunc();
  }
  delay(1000);
}
void temp_update() {
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperature readings
  current_temp = (sensors.getTempCByIndex(0)); // Why "byIndex"?
  // You can have more than one DS18B20 on the same bus.
  // 0 refers to the first IC on the wire
  if (current_temp > max_temp) {
    max_temp = current_temp;
  }
  else if (current_temp < min_temp) {
    min_temp = current_temp;
  }
}

void clear_display_function() {
  display.clearDisplay();
  /*display.setTextSize(2);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(0,0);
    display.print(F("MAX:"));*/
  display.display();
}

void drawtemp(void) {     //OLED function
  display.clearDisplay();
  /*
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    display.println(F("Hello, world!"));

    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
    display.println(3.141592);
  */

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);            // Start at top-left corner
  display.print(F("MAX:")); display.println(max_temp);
  display.print(F("MIN:")); display.println(min_temp);
  display.print(F("NOW:")); display.println(current_temp); display.println(releCount);

  display.display();
}

void buttonpress() {
  i = 30;
}
