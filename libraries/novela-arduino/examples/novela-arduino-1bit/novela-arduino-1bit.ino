#include <Arduino.h>
#include <string>

#include <gfx_canvas.h>
#include <novela.h>
#include <ReliableConnectionUsbCdc.h>
#include <ReliableConnectionSerial1.h>
#include <arduino_clock.h>
#include <serial_logger.h>
#include <soft_cursor.h>
#include <drivers/dvhstx/dvhstx.hpp>

pimoroni::DVHSTX hstx;
GfxCanvas canvas = GfxCanvas(hstx);
ReliableConnectionUsbCdc usb = ReliableConnectionUsbCdc();
ReliableConnectionSerial1* serial1 = ReliableConnectionSerial1::getInstance();
ArduinoClock ticker = ArduinoClock();
SerialLogger logger = SerialLogger();
SoftCursor cursor = SoftCursor(ticker, canvas, logger);
Novela novela(canvas, serial1, ticker, logger, &cursor);

void setup()
{
  // Wait for USB serial with timeout
  unsigned long start = millis();
  while(!Serial && millis() - start < 5000)  // 5 second timeout
  {
    delay(1);
  }

  Serial.begin(921600);
  Serial.println("Hello, Operator.");
  delay(2000); // Wait for USB to be initialized

  serial1->enableXonXoff();
  serial1->begin();

  logger.setLevel(Logger::Level::INFO);

  canvas.begin();

  // Call this last to ensure that everything is initialized before we start up the terminal.
  novela.begin();

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
  }
}
