# MCP23008

Clone of https://github.com/blemasle/arduino-mcp23017 to work with the mcp23008 
Thanks to https://github.com/blemasle for the original library, this is mostly a find/replace.

This library provides full control over the Microchip's [MCP23008](https://www.microchip.com/wwwproducts/en/MCP23008), including interrupt support.

## Features
 * Individual pins read & write
 * Ports read & write
 * Registers read & write
 * Full interrupt support

## Usage
Unlike most Arduino library, no default instance is created when the library is included. It's up to you to create one with the appropriate chip I2C address.

```cpp
#include <Arduino.h>
#include <MCP23008.h>

MCP23008 mcp = MCP23008(0x24);
```

Additionaly, you can specify the `Wire` instance to use as a second argument. For instance `MCP23008(0x24, Wire1)`.  
See included examples for further usage.
