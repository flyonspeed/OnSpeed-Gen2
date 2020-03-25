/*
 * LSM9DS1.h
 *
 * Created: 05/01/2015 19:49:15
 *  Author: speirano
 */


#ifndef LSM9DS1_SENSOR_H_
#define LSM9DS1_SENSOR_H_

#include <Arduino.h>
#include <stdbool.h>
#include <stdint-gcc.h>


class LSM9DS1
{

private:
    uint8_t _address;

public:
    LSM9DS1(unsigned int address);
    ~LSM9DS1() {};
    virtual bool begin(void)=0;

protected:
    byte readRegister(byte slaveAddress, byte regToRead);
    bool writeRegister(byte slaveAddress, byte regToWrite, byte dataToWrite);
    uint8_t addr(void) {return _address;};
    int _x, _y, _z;
    unsigned char _ready_reg;
    unsigned char _status_reg;
    bool readInternal(unsigned char high_reg, unsigned char low_reg, int *value);
};


class LSM9DS1_A : public LSM9DS1
{
public:
    LSM9DS1_A(void);
    bool activate(void);
    bool deactivate(void);
    bool begin(void);
    int readX();
    int readY();
    int readZ();

};

class LSM9DS1_G : public LSM9DS1
{
public:
    LSM9DS1_G(void);
    bool activate(void);
    bool begin(void);
    bool deactivate(void);
    int readX();
    int readY();
    int readZ();

};
class LSM9DS1_M : public LSM9DS1
{
public:
    LSM9DS1_M(void);
    bool begin(void);
    bool activate(void);
    bool deactivate(void);
    int readX();
    int readY();
    int readZ();

};


extern  LSM9DS1_A smeAccelerometer;
extern  LSM9DS1_M smeMagnetic;
extern  LSM9DS1_G smeGyroscope;

#endif /* LSM9DS1_H_ */
