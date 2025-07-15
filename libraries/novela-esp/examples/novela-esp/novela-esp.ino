/*
Product link:	https://www.elecrow.com/esp32-display-series-hmi-touch-screen.html
Specifically: https://www.elecrow.com/esp32-display-5-inch-hmi-display-rgb-tft-lcd-touch-screen-support-lvgl.html

Code	  link:	https://github.com/Elecrow-RD/CrowPanel-ESP32-Display-Course-File
Specifically: https://github.com/Elecrow-RD/CrowPanel-ESP32-Display-Course-File/tree/main/CrowPanel_ESP32_Tutorial/Code/V2.X/Lesson%202%20Draw%20GUI%20with%20LovyanGFX/4.3inch_5inch_7inch/Draw

Lesson	link:	https://www.youtube.com/watch?v=WHfPH-Kr9XU
*/

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <string>

#include <novela.h>
#include <ReliableConnectionUsbCdc.h>
#include <ReliableConnectionSerial1.h>
#include <arduino_clock.h>
#include <serial_logger.h>
#include <soft_cursor.h>
#include <lgfx_canvas.h>

#include "gfx_conf.h"

LGFX screen;
LGFXCanvas canvas = LGFXCanvas();
ReliableConnectionUsbCdc usb = ReliableConnectionUsbCdc();
ReliableConnectionSerial1* serial1 = ReliableConnectionSerial1::getInstance();
ArduinoClock ticker = ArduinoClock();
SerialLogger logger = SerialLogger();
SoftCursor cursor = SoftCursor(ticker, canvas, logger);
Novela novela(canvas, serial1, ticker, logger, &cursor);

int lastType = 0;
int counter = 0;
bool tap = false;

void setup()
{
  // Wait for USB serial with timeout
  unsigned long start = millis();
  while(!Serial && millis() - start < 5000)  // 5 second timeout
  {
    delay(1);
  }

  //Serial.begin(921600);
  Serial.begin(115200);
  Serial.println("Hello, Operator.");
  delay(2000); // Wait for USB to be initialized

  Serial.println("usb initialized");

  serial1->enableXonXoff();
  serial1->begin();

  Serial.println("serial1 initialized");

  logger.setLevel(Logger::Level::INFO);

  Serial.println("Logger initialized");

  screen.begin();
  delay(200);

  Serial.println("Screen initialized");

  canvas.begin(&screen);

  Serial.println("canvas initialized");

  // Call this last to ensure that everything is initialized before we start up the terminal.
  novela.begin();

  Serial.println("novela initialized");
  Serial.println("setup() complete.");

  Serial.write("\033[2J\033[H"); // Clear screen and send cursor to home
}

void loop()
{
  std::vector<char> output = serial1->read();
  if(!output.empty())
  {
    novela.process(output);
    usb.write(output);
  }

  novela.update();

  std::vector<char> input = usb.read();
  if(!input.empty())
  {
    serial1->write(input);

    // FIXME - remove these debug lines
    novela.process(input);
    novela.update();
  }

  // FIXME - debugging, remove
  if(lastType == 0)
  {
    lastType = millis();
  }
  else
  {
    if(millis() - lastType > 1333)
    {
      lastType = millis();
      novela.process({counter + '0'});
      counter = (counter + 1) % 10;
      novela.update();
    }
  }

  uint16_t touchX, touchY;
  bool isTouched = screen.getTouch(&touchX, &touchY);
  if (isTouched && !tap)
  {
    Serial.println("Touch started");
    tapped();
  }
  else if (!isTouched && tap)
  {
    Serial.println("Touch ended");
  }

  tap = isTouched;
}

void tapped()
{
  canvas.nextTheme();
}