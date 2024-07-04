#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor {
  private:
    struct MotorParams {
      uint8_t motorID;    
      int32_t anglePidKp;  
      int32_t anglePidKi;  
      int32_t speedPidKp;  
      int32_t speedPidKi;  
      int32_t iqPidKp;     
      int32_t iqPidKi;     
      int32_t acceleration;   // 单位1dps/s
      uint16_t encoder;       //编码器位置
      uint16_t encoderRaw;    //编码器原始位置
      uint16_t encoderOffset; //编码器零偏
      int64_t motorAngle;
      int16_t circleAngle;
      int8_t temperature;
      int16_t powerControl;
      int16_t speed;
      int32_t speedControl;
      int64_t angleControl;
      int32_t maxSpeed;
      uint8_t spinDirection;  
      int32_t angleIncrement;

    };
    struct productInfo
    {
      uint8_t driverName[20]; // 驱动名称
      uint8_t motorName[20]; // 电机名称
      uint8_t hardwareVersion; // 驱动硬件版本
      uint8_t firmwareVersion; // 固件版本
    };

    productInfo motorinfo;
    MotorParams params;
    HardwareSerial& motorserial;

  public:
    Motor(HardwareSerial& _motorserial = Serial2);

    bool readPIDParamsFrame();
    void readPIDParams();
    void writePIDParamsToRAM();
    void writePIDParamsToROM();
    void readAcceleration();

    bool readAccelerationFrame();
    void writeAccelerationToRAM();

    void readEncoderPosition();
    bool parseEncoderData();

    void setEncoderZeroOffset(uint16_t encoderOffset);
    bool parseEncoderZeroOffsetResponse();

    void writeEncoderInitialPositionToROM();
    bool parseEncoderInitialROMResponse();

    void readMultiTurnAngle();
    bool parseMultiTurnAngleResponse();

    void readSingleTurnAngleCommand();
    bool parseSingleTurnAngleResponse();

    void readMotorStatusAndError();
    bool parseMotorStatusAndErrorResponse();
    void clearErrorFlag();
    bool parseClearErrorFlagResponse();
    void readMotorStatus2();
    bool parseMotorStatus2Response();
    void readMotorStatus3();
    bool parseMotorStatus3Response();

    void sendMotorRun();
    void sendMotorStop();
    void sendMotorClose();
    bool parseMotorResponse();

    void sendOpenLoopControl(int16_t powerControl);
    void sendSpeedControl(int32_t speedControl);
    void sendMultiTurnPositionControl1(int64_t angleControl);
    void sendMultiTurnPositionControl2(int64_t angleControl, uint32_t maxSpeed);
    void sendSingleTurnPositionControl1(uint8_t spinDirection, uint16_t angleControl);
    void sendSingleTurnPositionControl2(uint8_t spinDirection, uint16_t angleControl, uint32_t maxSpeed);
    void sendIncrementalPositionControl1(int32_t angleIncrement);
    void sendIncrementalPositionControl2(int32_t angleIncrement, uint32_t maxSpeed);
    bool parseControlResponse();
    

    void readMotorAndDriverInfo();
    bool parseDriverReply();

    void sendReadMultiTurnAngle();
    void sendIncrementalPositionControl3(int32_t angleIncrement, uint32_t maxSpeed);
    bool parseMotorAngleReply();

};

#endif
