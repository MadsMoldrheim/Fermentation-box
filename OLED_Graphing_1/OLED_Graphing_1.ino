/*

  This program provides functions to draw:
  1. horizontal bar graph
  2. vertical bar graph
  3. dial
  4. cartesian graph

  It requires an microcrontrioller Arduino, teensy and a 0.96 dispaly (1306 driver chip)

  display. sources
  https://www.amazon.com/Diymall-Yellow-Arduino-display.-Raspberry/dp/B00O2LLT30/ref=pd_lpo_vtph_147_bs_t_1?_encoding=UTF8&psc=1&refRID=BQAPE07SVJQNDKEVMTQZ
  https://www.amazon.com/dp/B06XRCQZRX/ref=sspa_dk_detail_0?psc=1&pd_rd_i=B06XRCQZRX&pd_rd_wg=aGuhK&pd_rd_r=1WJPXAG68XFSADGDPNZR&pd_rd_w=DAAsB

  Adafruit libraries
  https://github.com/adafruit/Adafruit-GFX-Library/archive/master.zip
  https://github.com/adafruit/Adafruit_SSD1306

  Revisions
  rev     date        author      description
  1       12-5-2017   kasprzak    initial creation


  Pin settings

  Arduino   device
  5V        Vin
  GND       GND
  A0        read an input voltage from a pot across Vcc and Gnd (just for demo purposes)
  A1
  A2
  A3
  A4        SDA on the display. or use dedicated pin on board if provided
  A5        SCL on the display. use dedicated pin on board if provided
  1
  2
  3
  4
  5
  6
  7
  8
  9
  10
  11
  12
  13


*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define READ_PIN            A0
#define OLED_RESET          A4
#define LOGO16_GLCD_HEIGHT  16
#define LOGO16_GLCD_WIDTH   16

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// create what ever variables you need
double volts;
double bvolts;
double x, y;

// these are a required variables for the graphing functions
bool Redraw1 = true;
bool Redraw2 = true;
bool Redraw3 = true;
bool Redraw4 = true;
double ox , oy ;

// create the display object
//Adafruit_SSD1306 display(OLED_RESET);

void setup() {

  Serial.begin(9600);

  // initialize the display
  // note you may have to change the address
  // the most common are 0X3C and 0X3D

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // if you distribute your code, the adafruit license requires you show their splash screen
  // otherwise clear the video buffer memory then display
  display.display();
  //delay(2000);
  display.clearDisplay();
  display.display();

  // establish whatever pin reads you need
  pinMode(A0, INPUT);

}

unsigned long OldTime;
unsigned long counter;


  void loop(void) {

  // read some values and convert to volts
  bvolts = analogRead(A0);
  volts = (bvolts  / 204.6 ) ;


  // draw any of the following, sorry for the crazy long arguement lists
  // note 0.96 isn't very big, so you can't show many graphs at the same time
  // also colors are hard coded
  // the display i use draws text at the top in yellow--we'll use that for drawing the title


  // call one of these functions
 // DrawBarChartV(display, bvolts, 25,  60, 40, 40, 0, 1024 , 256, 0, "A0 (bits)", Redraw1);
  //  DrawBarChartH(display, volts, 10, 45, 100, 20, 0, 5, 1, 0, "A0 (volts)", Redraw2);
  //  DrawDial(display, volts, 65, 50, 25, 0, 5 , 1, 0, 200, "A0 (volts)", Redraw3);

  // for multiple graphs uncomment the next 2 lines
  //  DrawBarChartV(display, bvolts, 5,  60, 10, 40, 0, 1024 , 256, 0, "A0 (bits/volts)", Redraw1);
  //  DrawDial(display, volts, 90, 50, 25, 0, 5 , 1, 0, 200, "A0 (bits/volts)", Redraw3);

  // or show a cartesian style graph to plot values over time (or whatever)

    DrawCGraph(display, x++, bvolts, 30, 50, 75, 30, 0, 100, 25, 0, 1024, 512, 0, "Bits vs Seconds", Redraw4);
    if (x > 100) {
    while (1) {}
    }


  delay(1000);


  }


/*

  function to draw a cartesian coordinate system and plot whatever data you want
  just pass x and y and the graph will be drawn huge arguement list

  &display to pass the display object, mainly used if multiple displays are connected to the MCU
  x = x data point
  y = y datapont
  gx = x graph location (lower left)
  gy = y graph location (lower left)
  w = width of graph
  h = height of graph
  xlo = lower bound of x axis
  xhi = upper bound of x asis
  xinc = division of x axis (distance not count)
  ylo = lower bound of y axis
  yhi = upper bound of y asis
  yinc = division of y axis (distance not count)
  title = title of graph
  &redraw = flag to redraw graph on fist call only

*/


void DrawCGraph(Adafruit_SSD1306 &d, double x, double y, double gx, double gy, double w, double h, double xlo, double xhi, double xinc, double ylo, double yhi, double yinc, double dig, String title, boolean &Redraw) {

  double i;
  double temp;
  int rot, newrot;

  if (Redraw == true) {
    Redraw = false;
    d.fillRect(0, 0,  127 , 16, SSD1306_WHITE);
    d.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    d.setTextSize(1);
    d.setCursor(1, 1);
    d.println(title);
    ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
    oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    // draw y scale
    d.setTextSize(1);
    d.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    for ( i = ylo; i <= yhi; i += yinc) {
      // compute the transform
      // note my transform funcition is the same as the map function, except the map uses long and we need doubles
      temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
      if (i == 0) {
        d.drawFastHLine(gx - 3, temp, w + 3, SSD1306_WHITE);
      }
      else {
        d.drawFastHLine(gx - 3, temp, 3, SSD1306_WHITE);
      }
      d.setCursor(gx - 27, temp - 3);
      d.println(i, dig);
    }
    // draw x scale
    for (i = xlo; i <= xhi; i += xinc) {
      // compute the transform
      d.setTextSize(1);
      d.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
      temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
      if (i == 0) {
        d.drawFastVLine(temp, gy - h, h + 3, SSD1306_WHITE);
      }
      else {
        d.drawFastVLine(temp, gy, 3, SSD1306_WHITE);
      }
      d.setCursor(temp, gy + 6);
      d.println(i, dig);
    }
  }

  // graph drawn now plot the data
  // the entire plotting code are these few lines...

  x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  d.drawLine(ox, oy, x, y, SSD1306_WHITE);
  d.drawLine(ox, oy - 1, x, y - 1, SSD1306_WHITE);
  ox = x;
  oy = y;

  // up until now print sends data to a video buffer NOT the screen
  // this call sends the data to the screen
  d.display();

}


/////////////////////////////////////////////////////////
