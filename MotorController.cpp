#include <arduino.h>
#include "MotorController.h"

/**
   Init the state machine and the relais
*/
void MotorController_Class::init(void) {
  pinMode(MC_RELAIS_LEFT, OUTPUT);
  pinMode(MC_RELAIS_RIGHT, OUTPUT);

  digitalWrite(MC_RELAIS_LEFT, LOW);
  digitalWrite(MC_RELAIS_RIGHT, LOW);

  readPosition();
  posNew = posCurrent;

  Serial.println("MotorController started");
}


/**
   Read the current position of the rotator
   Scale the value
   Remember the last timestamp when read
*/
void MotorController_Class::readPosition(void) {
  long analog = analogRead(MC_ANALOG_PORT) - minADC;
  posCurrent = (analog * maxAngle) / maxADC;
  lastPositionRead = millis();
}


void MotorController_Class::process(void) {
  uint32_t cm = millis();
  /**
     enough time since last rotator position read?
  */
  if (cm - lastPositionRead > analogReadTimeInMs ) {
    //  yes
    //  read it
    readPosition();
  }

  switch (state) {
    case 1:
      // we start only, when the difference is greater the hysteresis variable
      // we need to turn right?
      if ( posCurrent + hysteresis < posNew) {
        // yes
        state = 10;
        // we need to turn left?
      } else if ( posCurrent - hysteresis > posNew ) {
        //  yes
        state = 20;
      }
      break;

    /*
       Start motor turning right
    */
    case 10:
      Serial.println("MotorController turning right");
      Serial.print("   ");
      Serial.print(posCurrent);
      Serial.print("/");
      Serial.println(posNew);
      digitalWrite(MC_RELAIS_LEFT, LOW);
      digitalWrite(MC_RELAIS_RIGHT, HIGH);
      lastMotorStart = cm;
      state = 11;
      break;

    /*
       Motor is turning right
    */
    case 11:
      // we reached the position?
      if ( posCurrent + hysteresis >= posNew ) {
        // yes
        state = 30;
        //  we are turning too long?
      } else if ( millis() - lastMotorStart >= maxMotorOnTimeInMs ) {
        //  yes
        Serial.println("MotorController motor too long on");
        //   switch to stop state
        state = 40;
      }
      break;

    /*
       Start Motor turning left
    */
    case 20:
      Serial.println("MotorController turning left");
      Serial.print("   ");
      Serial.print(posCurrent);
      Serial.print("/");
      Serial.println(posNew);
      digitalWrite(MC_RELAIS_LEFT, HIGH);
      digitalWrite(MC_RELAIS_RIGHT, LOW );
      lastMotorStart = cm;
      state = 21;
      break;

    /*
       Motor us turning left
    */
    case 21:
      // we reached the position?
      if ( posCurrent - hysteresis <= posNew ) {
        // yes
        state = 30;
        //  we are turning too long?
      } else if ( millis() - lastMotorStart >= maxMotorOnTimeInMs ) {
        //  yes
        Serial.println("MotorController motor too long on");
        state = 40;
      }
      break;

    /*
       Stop the motor turning
    */
    case 30:
      Serial.println("MotorController stopping motor");
      digitalWrite(MC_RELAIS_LEFT, LOW);
      digitalWrite(MC_RELAIS_RIGHT, LOW);
      lastMotorStart = cm;
      //  and switch to wait for rotor silenced
      state = 31;
      break;

    /**
       Motor is stoppen and waiting for silence
    */
    case 31:
      // long enough waited?
      if ( millis() - lastMotorStart >= motorStopTimeInMs ) {
        //  yes
        Serial.println("MotorController motor silenced");
        // start from beginning
        state = 1;
      }
      break;

    /**
       motor is turning too long
    */
    case 40:
      // stop the motor
      Serial.println("MotorController Emergency stop");
      digitalWrite(MC_RELAIS_LEFT, LOW);
      digitalWrite(MC_RELAIS_RIGHT, LOW);
      //  count the event
      ++numEmergencyStops;

      //  and goto idle loop
      state = 41;
      break;

    /*
       Special loop to keep in emergency reset state
    */
    case 41:
      break;
    /*
       unknown state
    */
    default:
      // report error
      Serial.print("MotorController unknown state ");
      Serial.println(state);

      // start from the beginning
      state = 1;
      break;
  }

}

