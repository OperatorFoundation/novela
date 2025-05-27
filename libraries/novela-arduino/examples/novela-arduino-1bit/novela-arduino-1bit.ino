#include <Arduino.h>
#include <string>

#include <gfx_canvas.h>
#include <novela_vterm.h>
#include <ReliableConnection.h>
#include <arduino_clock.h>

DVItext1 gfx = DVItext1(DVI_RES_640x240p60, adafruit_dvibell_cfg);
GfxCanvas canvas = GfxCanvas(gfx);
ReliableConnection connection = ReliableConnection(4, 5);
ArduinoClock ticker = ArduinoClock();
NovelaVterm novela(canvas, connection, ticker);

//ReliableConnection serial(4,5);

void setup()
{
//  while(!Serial)
//  {
//    delay(1);
//  }

  Serial.begin(9600);
  Serial.println("Hello, Operator.");

  Serial2.setTX(4);
  Serial2.setRX(5);
  Serial2.begin(115200);
}

void loop()
{
  int c = connection.tryReadOne();
  if(c != -1)
  {
    Serial.println(c);
    novela.process(static_cast<byte>(static_cast<unsigned char>(c)));
  }

  novela.update();
}
