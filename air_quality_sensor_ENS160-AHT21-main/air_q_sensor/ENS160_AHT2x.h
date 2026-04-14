#pragma once
#include "sfe_ens160.h"
#include "sfe_bus.h"
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>

#ifndef __Humidity_AHT20_H__
#define __Humidity_AHT20_H__

#define AHT20_DEFAULT_ADDRESS 0x38

class Sensor_ENS160 : public QwDevENS160
{

	public: 

		Sensor_ENS160() {};

    ///////////////////////////////////////////////////////////////////////
    // begin()
    //
    //
    // This method follows the standard startup pattern in SparkFun Arduino
    // libraries.
    //
    //  Parameter   Description
    //  ---------   ----------------------------
    //  wirePort    optional. The Wire port. If not provided, the default port is used
    //  address     optional. I2C Address. If not provided, the default address is used.
    //  retval      true on success, false on startup failure
    //
    // This method is overridden, implementing two versions.
    //
    // Version 1:
    // User skips passing in an I2C object which then defaults to Wire.
		bool begin(uint8_t deviceAddress = ENS160_ADDRESS_HIGH)
		{
        // Setup  I2C object and pass into the superclass
        setCommunicationBus(_i2cBus, deviceAddress);

				// Initialize the I2C buss class i.e. setup default Wire port
        _i2cBus.init();

        // Initialize the system - return results
        return this->QwDevENS160::init();
		}

		//Version 2:
    // User passes in an I2C object and an address (optional).
    bool begin(TwoWire &wirePort, uint8_t deviceAddress = ENS160_ADDRESS_HIGH)
    {
        // Setup  I2C object and pass into the superclass
        setCommunicationBus(_i2cBus, deviceAddress);

				// Give the I2C port provided by the user to the I2C bus class.
        _i2cBus.init(wirePort, true);

        // Initialize the system - return results
        return this->QwDevENS160::init();
    }

	private: 

		//I2C bus class
		sfe_ENS160::QwI2C _i2cBus; 

};
	
class ENS160_SPI : public QwDevENS160
{
		public:

		ENS160_SPI() {};

    ///////////////////////////////////////////////////////////////////////
    // begin()
    //
    // This method is called to initialize the ISM330DHCX library and connect to
    // the ISM330DHCX device. This method must be called before calling any other method
    // that interacts with the device.
    //
    // This method follows the standard startup pattern in SparkFun Arduino
    // libraries.
    //
    //  Parameter   Description
    //  ---------   ----------------------------
    //  spiPort     optional. The SPI port. If not provided, the default port is used
    //  SPISettings optional. SPI "transaction" settings are need for every data transfer. 
		//												Default used if not provided.
    //  Chip Select mandatory. The chip select pin ("CS") can't be guessed, so must be provided.
    //  retval      true on success, false on startup failure
    //
    // This methond is overridden, implementing two versions.
    //
    // Version 1:
    // User skips passing in an SPI object which then defaults to SPI.

    bool begin(uint8_t cs)
    {
        // Setup a SPI object and pass into the superclass
        setCommunicationBus(_spiBus);

				// Initialize the SPI bus class with the chip select pin, SPI port defaults to SPI, 
				// and SPI settings are set to class defaults.
        _spiBus.init(cs, true);

        // Initialize the system - return results
        return this->QwDevENS160::init();
    }

    bool begin(SPIClass &spiPort, SPISettings ensSettings, uint8_t cs)
    {
        // Setup a SPI object and pass into the superclass
        setCommunicationBus(_spiBus);

				// Initialize the SPI bus class with provided SPI port, SPI setttings, and chip select pin.
        _spiBus.init(spiPort, ensSettings, cs, true);

        // Initialize the system - return results
        return this->QwDevENS160::init();
    }

		private:

		// SPI bus class
		sfe_ENS160::SfeSPI _spiBus;

};


enum registers
{
    sfe_aht20_reg_reset = 0xBA,
    sfe_aht20_reg_initialize = 0xBE,
    sfe_aht20_reg_measure = 0xAC,
};

class AHT20
{
private:
    TwoWire *_i2cPort; //The generic connection to user's chosen I2C hardware
    uint8_t _deviceAddress;
    bool measurementStarted = false;

    struct
    {
        uint32_t humidity;
        uint32_t temperature;
    } sensorData;

    struct
    {
        uint8_t temperature : 1;
        uint8_t humidity : 1;
    } sensorQueried;

public:
    //Device status
    bool begin(TwoWire &wirePort = Wire); //Sets the address of the device and opens the I2C bus
    bool isConnected();                   //Checks if the AHT20 is connected to the I2C bus
    bool available();                     //Returns true if new data is available

    //Measurement helper functions
    uint8_t getStatus();       //Returns the status byte
    bool isCalibrated();       //Returns true if the cal bit is set, false otherwise
    bool isBusy();             //Returns true if the busy bit is set, false otherwise
    bool initialize();         //Initialize for taking measurement
    bool triggerMeasurement(); //Trigger the AHT20 to take a measurement
    void readData();           //Read and parse the 6 bytes of data into raw humidity and temp
    bool softReset();          //Restart the sensor system without turning power off and on

    //Make measurements
    float getTemperature(); //Goes through the measurement sequence and returns temperature in degrees celcius
    float getHumidity();    //Goes through the measurement sequence and returns humidity in % RH
};
#endif
