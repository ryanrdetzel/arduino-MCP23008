#pragma once

#include <Arduino.h>
#include <Wire.h>

#define _MCP23008_INTERRUPT_SUPPORT_ ///< Enables support for MCP23008 interrupts.


/**
 * Controls if the two interrupt pins mirror each other.
 * See "3.6 Interrupt Logic".
 */
enum class MCP23008InterruptMode : uint8_t
{
	Separated = 0,	///< Interrupt pins are kept independent
	Or = 0b01000000	///< Interrupt pins are mirrored
};

/**
 * Registers addresses.
 * The library use addresses for IOCON.BANK = 0.
 * See "3.2.1 Byte mode and Sequential mode".
 */
enum class MCP23008Register : uint8_t
{
	IODIR		= 0x00, 		///< Controls the direction of the data I/O for port A.
	IPOL		= 0x01,			///< Configures the polarity on the corresponding GPIO_ port bits for port A.
	GPINTEN	= 0x02,			///< Controls the interrupt-on-change for each pin of port A.
	DEFVAL	= 0x03,			///< Controls the default comparaison value for interrupt-on-change for port A.
	INTCON	= 0x04,			///< Controls how the associated pin value is compared for the interrupt-on-change for port A.
	IOCON		= 0x05,			///< Controls the device.
	GPPU		= 0x06,			///< Controls the pull-up resistors for the port A pins.
	INTF		= 0x07,			///< Reflects the interrupt condition on the port A pins.
	INTCAP	= 0x08,			///< Captures the port A value at the time the interrupt occured.
	GPIO		= 0x09,			///< Reflects the value on the port A.
	OLAT		= 0x0A,			///< Provides access to the port A output latches.
};

class MCP23008
{
private:
	TwoWire* _bus;
	uint8_t _deviceAddr;
public:
	/**
	 * Instantiates a new instance to interact with a MCP23008 at the specified address.
	 */
	MCP23008(uint8_t address, TwoWire& bus = Wire);
	~MCP23008();
#ifdef _DEBUG
	void debug();
#endif
	/**
	 * Initializes the chip with the default configuration.
	 * Enables Byte mode (IOCON.BANK = 0 and IOCON.SEQOP = 1).
	 * Enables pull-up resistors for all pins. This will only be effective for input pins.
	 * 
	 * See "3.2.1 Byte mode and Sequential mode".
	 */
	void init();
	/**
	 * Controls the pins direction on a whole port at once.
	 * 
	 * 1 = Pin is configured as an input.
	 * 0 = Pin is configured as an output.
	 * 
	 * See "3.5.1 I/O Direction register".
	 */
	void portMode(uint8_t directions, uint8_t pullups = 0xFF, uint8_t inverted = 0x00);
	/**
	 * Controls a single pin direction. 
	 * Pin 0-7
	 * 
	 * 1 = Pin is configured as an input.
	 * 0 = Pin is configured as an output.
	 *
	 * See "3.5.1 I/O Direction register".
	 * 
	 * Beware!  
	 * On Arduino platform, INPUT = 0, OUTPUT = 1, which is the inverse
	 * of the MCP23008 definition where a pin is an input if its IODIR bit is set to 1.
	 * This library pinMode function behaves like Arduino's standard pinMode for consistency.
	 * [ OUTPUT | INPUT | INPUT_PULLUP ]
	 */
	void pinMode(uint8_t pin, uint8_t mode, bool inverted = false);

	/**
	 * Writes a single pin state.
	 * Pin 0-7
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */
	void digitalWrite(uint8_t pin, uint8_t state);
	/**
	 * Reads a single pin state.
	 * Pin 0-7
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */ 
	uint8_t digitalRead(uint8_t pin);

	/**
	 * Writes pins state to a whole port.
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */
	void writePort(uint8_t value);
	/**
	 * Writes pins state to both ports.
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */
	void write(uint8_t value);

	/**
	 * Reads pins state for a whole port.
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */
	uint8_t readPort();
	/**
	 * Reads pins state for both ports. 
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */
	uint8_t read();

	/**
	 * Writes a single register value.
	 */
	void writeRegister(MCP23008Register reg, uint8_t value);

	/**
	 * Reads a single register value.
	 */
	uint8_t readRegister(MCP23008Register reg);

#ifdef _MCP23008_INTERRUPT_SUPPORT_

	/**
	 * Controls how the interrupt pins act with each other.
	 * If intMode is SEPARATED, interrupt conditions on a port will cause its respective INT pin to active.
	 * If intMode is OR, interrupt pins are OR'ed so an interrupt on one of the port will cause both pints to active.
	 * 
	 * Controls the IOCON.MIRROR bit. 
	 * See "3.5.6 Configuration register".
	 */
	void interruptMode(MCP23008InterruptMode intMode);
	/**
	 * Configures interrupt registers using an Arduino-like API.
	 * mode can be one of CHANGE, FALLING or RISING.
	 */
	void interrupt(uint8_t mode);
	/**
	 * Disable interrupts for the specified port.
	 */
	void disableInterrupt();

	/**
	 * Clears interrupts on both ports.
	 */
	void clearInterrupts();

#endif
};