#include <Arduino.h>
#include <string>

#include <tela.h>
#include <ReliableConnectionUsbCdc.h>
#include <arduino_clock.h>
#include <serial_logger.h>
#include <soft_cursor.h>

ReliableConnectionUsbCdc usb = ReliableConnectionUsbCdc();
ArduinoClock ticker = ArduinoClock();
SerialLogger logger = SerialLogger();

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

  logger.setLevel(Logger::Level::INFO);

  Serial.println("setup() complete.");

  Serial.write("\033[2J\033[H"); // Clear screen and send cursor to home
}

void loop()
{
  std::vector<char> input = usb.read();
  if(!input.empty())
  {
    usb.write(input);
  }
}
