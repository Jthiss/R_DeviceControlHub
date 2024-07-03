#include "motor.h"

Motor::Motor(HardwareSerial& _motorserial): motorserial(_motorserial) 
{
    
}

/**
* @brief  读取 PID 参数命令
* 主机发送该命令读取当前电机的 PID 的参数
* 数据域 说明 数据
* DATA[0] 头字节 0x3E
* DATA[1] 命令字节 0x30
* DATA[2] ID 字节 0x01~0x20
* DATA[3] 数据长度字节 0x00
* DATA[4] 帧头校验字节 DATA[0]~DATA[3]字节校验和
*/
void Motor::readPIDParams() 
{
    // 计算帧头校验字节
    uint8_t checksum = 0x3E + 0x30 + params.motorID + 0x00;

    // 发送命令
    motorserial.write(0x3E); // 头字节
    motorserial.write(0x30); // 命令字节
    motorserial.write(params.motorID); // ID字节
    motorserial.write(0x00); // 数据长度字节
    motorserial.write(checksum); // 帧头校验字节
}

/**
* @brief 读取驱动回复的PID参数数据帧，并存入结构体对象(大部分命令指令的驱动回复都是使用这个函数)
* @return true 解析成功，false 解析失败
*/
bool Motor::readPIDParamsFrame() 
{
    if (motorserial.available() >= 12) 
    {
        // 读取驱动回复数据
        uint8_t data[12];
        for (int i = 0; i < 12; i++) 
        {
            data[i] = motorserial.read();
        }

        // 解析驱动回复数据
        uint8_t headerChecksum = data[0] + data[1] + data[2] + data[3];
        uint8_t frameChecksum = data[5] + data[6] + data[7] + data[8] + data[9] + data[10];

        if (headerChecksum == data[4] && frameChecksum == data[11]) 
        {
            // 将数据存入结构体对象
            params.motorID = data[2];
            params.anglePidKp = data[5];
            params.anglePidKi = data[6];
            params.speedPidKp = data[7];
            params.speedPidKi = data[8];
            params.iqPidKp = data[9];
            params.iqPidKi = data[10];
            return true;
        }
    }
    return false;
}






/**
* @brief 发送命令以写入 PID 参数到 RAM（断电失效），驱动回复（12 字节）---readPIDParamsFrame() 
*/
void Motor::writePIDParamsToRAM() 
{
    // 计算帧头校验字节
    uint8_t checksumHeader = 0x3E + 0x31 + params.motorID + 0x06; // 计算头部字节的校验和
    uint8_t checksumData = params.anglePidKp + params.anglePidKi + params.speedPidKp + params.speedPidKi + params.iqPidKp + params.iqPidKi; // 计算数据字段的校验和

    // 发送命令
    motorserial.write(0x3E);          // 头字节
    motorserial.write(0x31);          // 命令字节
    motorserial.write(params.motorID); // ID 字节
    motorserial.write(0x06);          // 数据长度字节
    motorserial.write(checksumHeader); // 头部校验字节
    motorserial.write(params.anglePidKp); // 位置环 P 参数
    motorserial.write(params.anglePidKi); // 位置环 I 参数
    motorserial.write(params.speedPidKp); // 速度环 P 参数
    motorserial.write(params.speedPidKi); // 速度环 I 参数
    motorserial.write(params.iqPidKp);   // 转矩环 P 参数
    motorserial.write(params.iqPidKi);   // 转矩环 I 参数
    motorserial.write(checksumData);     // 数据字段校验和
}


/**
* @brief 发送命令以写入 PID 参数到 ROM（断电有效），驱动回复（12 字节）---readPIDParamsFrame() 
*/
void Motor::writePIDParamsToROM() 
{
    // 计算帧头校验字节
    uint8_t checksumHeader = 0x3E + 0x32 + params.motorID + 0x06; // 计算头部字节的校验和
    uint8_t checksumData = params.anglePidKp + params.anglePidKi + params.speedPidKp + params.speedPidKi + params.iqPidKp + params.iqPidKi; // 计算数据字段的校验和

    // 发送命令
    motorserial.write(0x3E);          // 头字节
    motorserial.write(0x31);          // 命令字节
    motorserial.write(params.motorID); // ID 字节
    motorserial.write(0x06);          // 数据长度字节
    motorserial.write(checksumHeader); // 头部校验字节
    motorserial.write(params.anglePidKp); // 位置环 P 参数
    motorserial.write(params.anglePidKi); // 位置环 I 参数
    motorserial.write(params.speedPidKp); // 速度环 P 参数
    motorserial.write(params.speedPidKi); // 速度环 I 参数
    motorserial.write(params.iqPidKp);   // 转矩环 P 参数
    motorserial.write(params.iqPidKi);   // 转矩环 I 参数
    motorserial.write(checksumData);     // 数据字段校验和
}




/**
* @brief 发送命令以读取当前电机的加速度参数
*/
void Motor::readAcceleration() 
{
    // 计算帧头校验字节
    uint8_t checksum = 0x3E + 0x33 + params.motorID + 0x00;

    // 发送命令
    motorserial.write(0x3E);   // 头字节
    motorserial.write(0x33);   // 命令字节
    motorserial.write(params.motorID);  // ID 字节
    motorserial.write(0x00);   // 数据长度字节
    motorserial.write(checksum);   // 帧头校验字节
}

/**
* @brief 读取驱动回复数据帧中的加速度参数
* @return true 解析成功，false 解析失败
*/
bool Motor::readAccelerationFrame() {
    if (motorserial.available() >= 10) {
        // 读取驱动回复数据
        uint8_t data[10];
        for (int i = 0; i < 10; i++) {
            data[i] = motorserial.read();
        }

        // 计算头部校验和
        uint16_t headerChecksum = 0;
        for (int i = 0; i < 4; i++) {
            headerChecksum += data[i];
        }

        // 计算帧内校验和
        uint16_t frameChecksum = 0;
        for (int i = 5; i < 9; i++) {
            frameChecksum += data[i];
        }

        // 解析加速度数据
        uint32_t acceleration = 0;
        acceleration |= data[5];      // 加速度低字节
        acceleration |= data[6] << 8; // 加速度字节 2
        acceleration |= data[7] << 16;// 加速度字节 3
        acceleration |= data[8] << 24;// 加速度字节 4

        // 检查校验和
        if (headerChecksum == data[4] && frameChecksum == data[9]) {
            // 将加速度存入结构体对象
            params.acceleration = acceleration;
            return true;
        }
    }
    return false;
}


/**
* @brief 发送命令以读取当前编码器的当前位置
*/
void Motor::readEncoderPosition() 
{
    // 计算帧头校验字节
    uint8_t checksum = 0x3E + 0x90 + params.motorID + 0x00;

    // 发送命令
    motorserial.write(0x3E);   // 头字节
    motorserial.write(0x90);   // 命令字节
    motorserial.write(params.motorID);  // ID 字节
    motorserial.write(0x00);   // 数据长度字节
    motorserial.write(checksum);   // 帧头校验字节
}


/**
 * @brief 从收到的数据中解析编码器参数
 * @return true 解析成功，false 解析失败
 */
bool Motor::parseEncoderData() 
{
   // 检查串口是否有足够的数据
   if (motorserial.available() >= 12) 
   {
       // 读取数据
       uint8_t data[12];
       for (int i = 0; i < 12; i++) 
       {
           data[i] = motorserial.read();
       }

       // 计算校验和
       uint16_t checksum = 0;
       for (int i = 0; i < 4; i++) 
       {
           checksum += data[i];
       }
       for (int i = 5; i < 11; i++) 
       {
           checksum += data[i];
       }

       // 验证校验和并解析数据
       if (checksum == (data[4] + (data[11] << 8))) 
       {
           // 解析编码器参数
           params.encoder = data[5] | (data[6] << 8);
           params.encoderRaw = data[7] | (data[8] << 8);
           params.encoderOffset = data[9] | (data[10] << 8);
           return true;  // 解析成功
       }
   }
   return false;  // 解析失败
}


/**
 * @brief 从收到的数据中解析编码器参数 
 * @param  uint16_t encoderOffset 需要写入的编码器值，14bit编码器的数值范围:0~16383
 */
void Motor::setEncoderZeroOffset(uint16_t encoderOffset) 
{
    // 向驱动发送设置编码器零偏的命令
    uint8_t checksumHeader = 0x3E + 0x91 + params.motorID + 0x02;
    uint8_t encoderOffset1 = (uint8_t)(encoderOffset & 0xFF);
    uint8_t encoderOffset2 = (uint8_t)(encoderOffset >> 8);

    motorserial.write(0x3E);  // 头字节
    motorserial.write(0x91);  // 命令字节
    motorserial.write(params.motorID);  // ID字节
    motorserial.write(0x02);  // 数据长度字节
    motorserial.write(checksumHeader);  // 校验和
    motorserial.write(encoderOffset1);  // 编码器零偏低字节
    motorserial.write(encoderOffset2);  // 编码器零偏高字节
    motorserial.write(encoderOffset1 + encoderOffset2);     //DATA[5]~DATA[6]字节校验和
}

/**
 * @brief 从收到的数据中解析编码器参数 
 * @return true 解析成功，false 解析失败
 */
bool Motor::parseEncoderZeroOffsetResponse() {
    if (motorserial.available() >= 8) {
        uint8_t data[8];
        for (int i = 0; i < 8; i++) {
            data[i] = motorserial.read();
        }
        // 检查帧头校验
        uint8_t checksum = 0;
        for (int i = 0; i < 4; i++) {
            checksum += data[i];
        }
        // 检查帧尾校验
        if (checksum == data[4] && data[5] == data[2] && data[6] == data[3] && (data[7] == (data[5] + data[6]))) {
            // 提取编码器零偏值
            params.encoderOffset = data[0] | (data[1] << 8);
            // 处理编码器零偏值
            // ...
            return true;
        }
    }
    return false;
}


/**
 * @brief 将电机当前编码器位置作为初始位置写入到 ROM 
 */
void Motor::writeEncoderInitialPositionToROM() 
{
    // 计算校验和
    uint8_t checksum = 0x3E + 0x19 + params.motorID + 0x00;

    // 发送命令
    motorserial.write(0x3E);      // 头字节
    motorserial.write(0x19);      // 命令字节
    motorserial.write(params.motorID);   // ID字节
    motorserial.write(0x00);      // 数据长度字节
    motorserial.write(checksum);  // 校验和
}


/**
 * @brief 将电机当前编码器位置作为初始位置写入到 ROM的驱动回复（5byte）(用不上的) 
 * @return true 解析成功，false 解析失败
 */
bool Motor::parseEncoderInitialROMResponse() 
{
    if (motorserial.available() >= 5) 
    {
        uint8_t data[5];
        for (int i = 0; i < 5; i++) 
        {
            data[i] = motorserial.read();
        }
        // 检查帧头校验
        uint8_t checksum = data[0] + data[1] + data[2] + data[3];
        if (checksum == data[4]) 
        {
            return true;
        }
    }
    return false;
}


/**
 * @brief 主机发送该命令以读取当前电机的多圈绝对角度值
 */
void Motor::readMultiTurnAngle() 
{
    // 计算校验和
    uint8_t checksum = 0x3E + 0x92 + params.motorID + 0x00;

    // 发送命令
    motorserial.write(0x3E);    // 头字节
    motorserial.write(0x92);    // 命令字节
    motorserial.write(params.motorID); // ID字节
    motorserial.write(0x00);    // 数据长度字节
    motorserial.write(checksum);  // 校验和
}


/**
 * @brief 电机在收到命令后回复主机，该帧数据中包含了以下参数：
            1. 电机角度 motorAngle，为 int64_t 类型数据，正值表示顺时针累计角度，负值表示逆时针累计角度，单位 0.01°/LSB。
 * @return true 解析成功，false 解析失败
 */
bool Motor::parseMultiTurnAngleResponse() 
{
    if (motorserial.available() >= 14) 
    {
        uint8_t data[14];
        for (int i = 0; i < 14; i++) 
        {
            data[i] = motorserial.read();
        }

        // 检查帧头校验
        uint8_t checksum = data[0] + data[1] + data[2] + data[3];

        if (checksum == data[4]) 
        {
            int64_t motorAngle = 0;
            motorAngle |= data[5];
            motorAngle |= ((int64_t)data[6] << 8);
            motorAngle |= ((int64_t)data[7] << 16);
            motorAngle |= ((int64_t)data[8] << 24);
            motorAngle |= ((int64_t)data[9] << 32);
            motorAngle |= ((int64_t)data[10] << 40);
            motorAngle |= ((int64_t)data[11] << 48);
            motorAngle |= ((int64_t)data[12] << 56);

            params.motorAngle = motorAngle;
            return true;
        }
    }
    return false;
}


/**
 * @brief 主机发送该命令以读取当前电机的单圈绝对角度值
 */
void Motor::readSingleTurnAngleCommand() 
{
    // 计算校验和
    uint8_t checksum = 0x3E + 0x94 + params.motorID + 0x00;

    // 发送命令
    motorserial.write(0x3E);    // 头字节
    motorserial.write(0x94);    // 命令字节
    motorserial.write(params.motorID); // ID字节
    motorserial.write(0x00);    // 数据长度字节
    motorserial.write(checksum);  // 校验和
}



/**
 * @brief 电机在收到命令后回复主机，该帧数据中包含了以下参数：
            1. 电机单圈角度 circleAngle，为 uint16_t 类型数据，以编码器零点为起始点，顺时针增加，再次到达零点时数值回 0，单位 0.01°/LSB，数值范围 0~35999。
 * @return true 解析成功，false 解析失败
 */
bool Motor::parseSingleTurnAngleResponse() 
{
    if (motorserial.available() >= 8) 
    {
        uint8_t data[8];
        for (int i = 0; i < 8; i++) 
        {
            data[i] = motorserial.read();
        }

        // 检查帧头校验
        uint8_t checksum = data[0] + data[1] + data[2] + data[3];
        if (checksum == data[4]) 
        {
            uint16_t circleAngle = 0;
            circleAngle |= data[5];
            circleAngle |= ((uint16_t)data[6] << 8);
            
            // Do something with the circleAngle data
            params.circleAngle = circleAngle;
            return true;
        }
    }
    return false;
}


void Motor::readMotorStatusAndError() 
{
    // 计算校验和
    uint8_t checksum = 0x3E + 0x9A + params.motorID + 0x00;

    // 发送命令
    motorserial.write(0x3E);    // 头字节
    motorserial.write(0x9A);    // 命令字节
    motorserial.write(params.motorID); // ID字节
    motorserial.write(0x00);    // 数据长度字节
    motorserial.write(checksum);  // 校验和
}


/**
 * @brief 电机在收到命令后回复主机，该帧数据中包含了以下参数：
            1. 电机单圈角度 circleAngle，为 uint16_t 类型数据，以编码器零点为起始点，顺时针增加，再次到达零点时数值回 0，单位 0.01°/LSB，数值范围 0~35999。
 * @return true 解析成功，false 解析失败
 */
bool Motor::parseMotorStatusAndErrorResponse() 
{
    if (motorserial.available() >= 13) 
    {
        uint8_t data[13];
        for (int i = 0; i < 13; i++) 
        {
            data[i] = motorserial.read();
        }

        // 检查帧头校验
        uint8_t checksum = data[0] + data[1] + data[2] + data[3];
        if (checksum == data[4]) 
        {
            int8_t temperature = data[5];
            uint16_t voltage = data[7] | (data[8] << 8);
            uint8_t errorState = data[11];

            // 处理温度、电压和错误状态信息

            return true;
        }
    }
    return false;
}

            

void Motor::clearErrorFlag() 
{
    // 计算校验和
    uint8_t checksum = 0x3E + 0x9B + params.motorID + 0x00;

    // 发送命令
    motorserial.write(0x3E);    // 头字节
    motorserial.write(0x9B);    // 命令字节
    motorserial.write(params.motorID); // ID字节
    motorserial.write(0x00);    // 数据长度字节
    motorserial.write(checksum);  // 校验和
}



bool Motor::parseClearErrorFlagResponse() 
{
    if (motorserial.available() >= 13) {
        uint8_t data[13];
        for (int i = 0; i < 13; i++) {
            data[i] = motorserial.read();
        }

        // 检查帧头校验
        uint8_t checksum = data[0] + data[1] + data[2] + data[3];
        if (checksum == data[4]) {
            int8_t temperature = data[5];
            uint16_t voltage = data[7] | (data[8] << 8);
            uint8_t errorState = data[11];

            // 处理温度、电压和错误状态信息

            return true;
        }
    }
    return false;
}



void Motor::readMotorStatus2() 
{
    // 计算校验和
    uint8_t checksum = 0x3E + 0x9C + params.motorID + 0x00;

    // 发送命令
    motorserial.write(0x3E);    // 头字节
    motorserial.write(0x9C);    // 命令字节
    motorserial.write(params.motorID); // ID字节
    motorserial.write(0x00);    // 数据长度字节
    motorserial.write(checksum);  // 校验和
}


bool Motor::parseMotorStatus2Response() 
{
    if (motorserial.available() >= 13) {
        uint8_t data[13];
        for (int i = 0; i < 13; i++) {
            data[i] = motorserial.read();
        }

        // 检查帧头校验
        uint8_t checksum = data[0] + data[1] + data[2] + data[3];
        if (checksum == data[4]) {
            int8_t temperature = data[5];
            int16_t currentOrPower = data[6] | (data[7] << 8);
            int16_t speed = data[8] | (data[9] << 8);
            uint16_t encoder = data[10] | (data[11] << 8);

            // 处理电机温度、转矩电流/输出功率、转速和编码器位置信息

            return true;
        }
    }
    return false;
}



void Motor::readMotorStatus3() 
{
    // 计算校验和
    uint8_t checksum = 0x3E + 0x9D + params.motorID + 0x00;

    // 发送命令
    motorserial.write(0x3E);    // 头字节
    motorserial.write(0x9D);    // 命令字节
    motorserial.write(params.motorID); // ID字节
    motorserial.write(0x00);    // 数据长度字节
    motorserial.write(checksum);  // 校验和
}

bool Motor::parseMotorStatus3Response() 
{
    if (motorserial.available() >= 13) {
        uint8_t data[13];
        for (int i = 0; i < 13; i++) {
            data[i] = motorserial.read();
        }

        // 检查帧头校验
        uint8_t checksum = data[0] + data[1] + data[2] + data[3];
        if (checksum == data[4]) {
            int8_t temperature = data[5];
            int16_t phaseCurrentA = data[6] | (data[7] << 8);
            int16_t phaseCurrentB = data[8] | (data[9] << 8);
            int16_t phaseCurrentC = data[10] | (data[11] << 8);

            // 处理电机温度和相电流数据

            return true;
        }
    }
    return false;
}



void Motor::sendMotorClose() 
{
    // 计算校验和
    uint8_t checksum = 0x3E + 0x80 + params.motorID + 0x00;

    // 发送关闭命令
    motorserial.write(0x3E);    // 头字节
    motorserial.write(0x80);    // 命令字节
    motorserial.write(params.motorID); // ID字节
    motorserial.write(0x00);    // 数据长度字节
    motorserial.write(checksum);  // 校验和
}

bool Motor::parseMotorResponse()
{
    if (motorserial.available() >= 5) {
        uint8_t data[5];
        for (int i = 0; i < 5; i++) {
            data[i] = motorserial.read();
        }

        // 检查帧头校验
        uint8_t checksum = data[0] + data[1] + data[2] + data[3];
        if (checksum == data[4]) {

            // 处理电机温度和相电流数据

            return true;
        }
    }
    return false;
}


void Motor::sendMotorStop() 
{
    // 计算校验和
    uint8_t checksum = 0x3E + 0x81 + params.motorID + 0x00;

    // 发送停止命令
    motorserial.write(0x3E);    // 头字节
    motorserial.write(0x81);    // 命令字节
    motorserial.write(params.motorID); // ID字节
    motorserial.write(0x00);    // 数据长度字节
    motorserial.write(checksum);  // 校验和
}


void Motor::sendMotorRun() 
{
    // 计算校验和
    uint8_t checksum = 0x3E + 0x81 + params.motorID + 0x00;

    // 发送停止命令
    motorserial.write(0x3E);    // 头字节
    motorserial.write(0x88);    // 命令字节
    motorserial.write(params.motorID); // ID字节
    motorserial.write(0x00);    // 数据长度字节
    motorserial.write(checksum);  // 校验和
}

//开环
