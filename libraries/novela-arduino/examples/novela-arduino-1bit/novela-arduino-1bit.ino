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

const std::vector<char> XON  = {0x11};
const std::vector<char> XOFF = {0x13};

pimoroni::DVHSTX hstx;
GfxCanvas canvas = GfxCanvas(hstx);
ReliableConnectionUsbCdc usb = ReliableConnectionUsbCdc();
ReliableConnectionSerial1 serial1 = ReliableConnectionSerial1();
ArduinoClock ticker = ArduinoClock();
SerialLogger logger = SerialLogger();
SoftCursor cursor = SoftCursor(ticker, canvas, logger);
Novela novela(canvas, serial1, ticker, logger, &cursor);

std::vector<char> outputBuffer = std::vector<char>();

void setup()
{
  while(!Serial)
  {
    delay(1);
  }

  Serial.begin(921600);
  Serial.println("Hello, Operator.");

  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(115200);

  Serial.println("UART begin");
  Serial.write("\033[2J\033[H"); // Clear screen and send cursor to home

  logger.setLevel(Logger::Level::INFO);

  canvas.begin();

  // Call this last to ensure that everything is initialized before we start up the terminal.
  novela.begin();

  serial1.write(XON);
}

void loop()
{
  std::vector<char> output = serial1.read();

  if(output.empty()) // In idle moments, do everything else besides buffering output
  {
    if(!outputBuffer.empty()) // Empty the output buffer
    {
      novela.process(outputBuffer);
      //usb.write(buffer);

      outputBuffer.clear();
      serial1.write(XON);
    }

    novela.update();

    std::vector<char> input = usb.read();
    if(!input.empty())
    {
      serial1.write(input);
    }
  }
  else // Prioritize getting output so we don't overrun the buffer
  {
    // If we have new data, but we've already buffered some, tell the other side to stop sending
	if(!outputBuffer.empty())
	{
      serial1.write(XOFF);
	}

    // Transfer the new data into the buffer for later processing
    outputBuffer.insert(outputBuffer.end(), output.begin(), output.end());
  }
}
