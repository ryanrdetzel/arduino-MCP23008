/**
 * When a pin on port B is falling, the same pin on port A is switched.
 * Connect a range of switches to port B and leds on port A to see it happening.
 * 
 * Connect digital pin 3 to MCP23008 INTB pin.
 * You'll also need a small cap (100nF tipically) between the arduino interrupt pin and GND.
 */
#include <Wire.h>
#include <MCP23008.h>

#define MCP23008_ADDR 0x20
#define INT_PIN 3

MCP23008 mcp = MCP23008(MCP23008_ADDR);

volatile bool interrupted = false;

void userInput() {
    interrupted = true;
}

void setup() {
    Wire.begin();
    Serial.begin(115200);
    
    mcp.init();
    mcp.portMode(MCP23008Port::A, 0);          //Port A as output
    mcp.portMode(MCP23008Port::B, 0b11111111); //Port B as input

    mcp.interruptMode(MCP23008InterruptMode::Separated);      
    mcp.interrupt(MCP23008Port::B, FALLING);

    mcp.writeRegister(MCP23008Register::IPOL_A, 0x00);
    mcp.writeRegister(MCP23008Register::IPOL_B, 0x00);

    mcp.writeRegister(MCP23008Register::GPIO_A, 0x00);
    mcp.writeRegister(MCP23008Register::GPIO_B, 0x00);

    mcp.clearInterrupts();

    pinMode(INT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT_PIN), userInput, FALLING);
}

void loop() {
    uint8_t a, b;
    uint8_t captureA, captureB;
    uint8_t currentA, currentB;

    if(!interrupted) 
    {
        // just to be sure that arduino and mcp are in the "same state"
        // regarding interrupts
        mcp.clearInterrupts();
        return;
    }

    //debouncing
    delay(100);
    interrupted = false;
    
    mcp.interruptedBy(a, b);
    // this is the state of the port the moment the interrupt was triggered
    mcp.clearInterrupts(captureA, captureB);
    // this is the state of the B port right now, after the delay to act as debouncing
    currentB = mcp.readPort(MCP23008Port::B);

    if((b & ~currentB) == (b & ~captureB)) {
        // the pin that triggered the interrupt is still in the same state after the deboucing delay
        currentA = mcp.readPort(MCP23008Port::A);
        mcp.writeRegister(MCP23008Register::GPIO_A, currentA ^ b);
    }
}