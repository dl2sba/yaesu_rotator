#ifndef _MOTORCONTROLLER_D
#define _MOTORCONTROLLER_D


/**
  D0 16 // LED_BUILTIN
  D1 5  // I2C Bus SCL (clock)
  D2 4  // I2C Bus SDA (data)
  D3 0
  D4 2  // Same as "LED_BUILTIN", but inverted logic
  D5 14 // SPI Bus SCK (clock)
  D6 12 // SPI Bus MISO
  D7 13 // SPI Bus MOSI
  D8 15 // SPI Bus SS (CS)
  D9 3  // RX0 (Serial console)
  D10 1 // TX0 (Serial console)
*/

#define MC_RELAIS_LEFT  D1
#define MC_RELAIS_RIGHT D2
#define MC_ANALOG_PORT  A0

class MotorController_Class {
  public:
    void    init(void);
    void      readPosition(void);
    void      process(void);
    void      setNewPos(int p) { posNew = p; };
    int       getNewPos(void) { return posNew;};
    int       getCurrentPos(void) { return posCurrent;};
    uint8_t   isTurning(void) { return (state != 1); };
    uint8_t   isEmergencyStopped(void) { return (state == 41); };
    void      resetEmergencyStop(void) { if ( state == 41 ) state = 1; };
    
  public:
    int       minADC = 0;
    int       maxADC = 1023;
    int       maxAngle = 360;
    uint8_t   turning = LOW;
    uint32_t  maxMotorOnTimeInMs = 30 * 1000;
    uint32_t  motorStopTimeInMs = 3 * 1000;
    uint32_t  analogReadTimeInMs = 300;
    uint8_t   hysteresis = 5;
    uint8_t   nothalt = LOW;
    uint32_t  numEmergencyStops = 0;

  protected:
    int       state = 1;
    int       posCurrent = 0;
    int       posNew = 0;
    uint32_t  lastPositionRead;
    uint32_t  lastMotorStart; 
};

#endif

