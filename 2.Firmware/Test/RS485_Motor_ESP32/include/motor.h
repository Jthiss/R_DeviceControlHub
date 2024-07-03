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

    };
  
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
    
};

#endif
