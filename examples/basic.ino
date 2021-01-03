#include <Arduino.h>
#include <MCP23008.h>

MCP23008 mcp = MCP23008(0x20);

int previousValue = 0;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  mcp.init();
  mcp.portMode(0b11111111);  // input
}

void loop() {
  uint8_t currentValue = mcp.readPort();

  if (currentValue != previousValue){
    Serial.print(currentValue, BIN);
    Serial.println();
    previousValue = currentValue;
  }
}