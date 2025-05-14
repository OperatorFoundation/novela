#include <Arduino.h>
#include <string>

#include <gfx_canvas.h>
#include <novela.h>
#include <ReliableConnection.h>

GfxCanvas canvas = GfxCanvas();
Novela novela(canvas);

ReliableConnection serial(4,5);

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello, Operator.");
}

void loop()
{
  byte c = serial.readOne();
  novela.process(c);
}
