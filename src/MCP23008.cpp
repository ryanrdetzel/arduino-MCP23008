#include "MCP23008.h"

MCP23008::MCP23008(uint8_t address, TwoWire& bus) {
	_deviceAddr = address;
	_bus = &bus;
}

MCP23008::~MCP23008() {}

void MCP23008::init()
{
	//BANK = 	0 : sequential register addresses
	//MIRROR = 	0 : use configureInterrupt 
	//SEQOP = 	1 : sequential operation disabled, address pointer does not increment
	//DISSLW = 	0 : slew rate enabled
	//HAEN = 	0 : hardware address pin is always enabled on 23017
	//ODR = 	0 : open drain output
	//INTPOL = 	0 : interrupt active low
	writeRegister(MCP23008Register::IOCON, 0b00100000);

	//enable all pull up resistors (will be effective for input pins only)
	writeRegister(MCP23008Register::GPPU, 0xFF);
}

void MCP23008::portMode(uint8_t directions, uint8_t pullups, uint8_t inverted)
{
	writeRegister(MCP23008Register::IODIR, directions);
	writeRegister(MCP23008Register::GPPU, pullups);
	writeRegister(MCP23008Register::IPOL, inverted);
}

void MCP23008::pinMode(uint8_t pin, uint8_t mode, bool inverted)
{
	MCP23008Register iodirreg = MCP23008Register::IODIR;
	MCP23008Register pullupreg = MCP23008Register::GPPU;
	MCP23008Register polreg = MCP23008Register::IPOL;
	uint8_t iodir, pol, pull;

	iodir = readRegister(iodirreg);
	if(mode == INPUT || mode == INPUT_PULLUP) bitSet(iodir, pin);
	else bitClear(iodir, pin);

	pull = readRegister(pullupreg);
	if(mode == INPUT_PULLUP) bitSet(pull, pin);
	else bitClear(pull, pin);

	pol = readRegister(polreg);
	if(inverted) bitSet(pol, pin);
	else bitClear(pol, pin);

	writeRegister(iodirreg, iodir);
	writeRegister(pullupreg, pull);
	writeRegister(polreg, pol);
}

void MCP23008::digitalWrite(uint8_t pin, uint8_t state)
{
	MCP23008Register gpioreg = MCP23008Register::GPIO;
	uint8_t gpio;

	gpio = readRegister(gpioreg);
	if(state == HIGH) bitSet(gpio, pin);
	else bitClear(gpio, pin);

	writeRegister(gpioreg, gpio);
}

uint8_t MCP23008::digitalRead(uint8_t pin)
{
	MCP23008Register gpioreg = MCP23008Register::GPIO;
	uint8_t gpio;

	gpio = readRegister(gpioreg);
	if(bitRead(gpio, pin)) return HIGH;
	return LOW;
}

void MCP23008::writePort(uint8_t value)
{
	writeRegister(MCP23008Register::GPIO, value);
}

void MCP23008::write(uint8_t value)
{
	writeRegister(MCP23008Register::GPIO, value);
}

uint8_t MCP23008::readPort()
{
	return readRegister(MCP23008Register::GPIO);
}

uint8_t MCP23008::read()
{
	return readRegister(MCP23008Register::GPIO);
}

void MCP23008::writeRegister(MCP23008Register reg, uint8_t value)
{
	_bus->beginTransmission(_deviceAddr);
	_bus->write(static_cast<uint8_t>(reg));
	_bus->write(value);
	_bus->endTransmission();
}

uint8_t MCP23008::readRegister(MCP23008Register reg)
{
	_bus->beginTransmission(_deviceAddr);
	_bus->write(static_cast<uint8_t>(reg));
	_bus->endTransmission();
	_bus->requestFrom(_deviceAddr, (uint8_t)1);
	return _bus->read();
}

#ifdef _MCP23008_INTERRUPT_SUPPORT_

void MCP23008::interruptMode(MCP23008InterruptMode intMode)
{
	uint8_t iocon = readRegister(MCP23008Register::IOCON);
	if(intMode == MCP23008InterruptMode::Or) iocon |= static_cast<uint8_t>(MCP23008InterruptMode::Or);
	else iocon &= ~(static_cast<uint8_t>(MCP23008InterruptMode::Or));

	writeRegister(MCP23008Register::IOCON, iocon);
}

void MCP23008::interrupt(uint8_t mode)
{
	MCP23008Register defvalreg = MCP23008Register::DEFVAL;
	MCP23008Register intconreg = MCP23008Register::INTCON;

	//enable interrupt for port
	writeRegister(MCP23008Register::GPINTEN, 0xFF);
	switch(mode)
	{
	case CHANGE:
		//interrupt on change
		writeRegister(intconreg, 0);
		break;
	case FALLING:
		//interrupt falling : compared against defval, 0xff
		writeRegister(intconreg, 0xFF);
		writeRegister(defvalreg, 0xFF);
		break;
	case RISING:
		//interrupt rising : compared against defval, 0x00
		writeRegister(intconreg, 0xFF);
		writeRegister(defvalreg, 0x00);
		break;
	}
}

void MCP23008::disableInterrupt()
{
	writeRegister(MCP23008Register::GPINTEN, 0x00);
}

void MCP23008::clearInterrupts()
{
	readRegister(MCP23008Register::INTCAP);
}


#endif
