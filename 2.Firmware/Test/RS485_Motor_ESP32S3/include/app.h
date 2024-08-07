#ifndef APP_H
#define APP_H

#include <main.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <lvgl.h>
#include <WiFi.h>
#include <BleKeyboard.h>
#include <Adafruit_NeoPixel.h>
#include <yeelight.h>
#include <motor.h>
#include <Wire.h>
#include <INA226.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#if 1
#include <TFT_eSPI.h>
#endif
//#include <examples/lv_examples.h>
#include <demos/lv_demos.h>
#include <gui_guider.h>
#include <custom.h>
#include <events_init.h>

#define LED_PIN 48   //对应的是板上的WS2812
#define LED_NUM  1   //WS2812数量

#define screenWidth 240
#define screenHeight 240

#define APP_1 0
#define APP_2 1
#define APP_3 2

struct Button {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};

void Yeelight_Linking();
void test_label(lv_timer_t* timer);
void myWiFi_init();
void timer_interrupt();
void timer_ble_interrupt();
void My_LVGL_Init();
void I2C_Init();
void INA226_Read();
void MPU6050_Read();
void Timer_CB();
void Yeelight_Init();
void ARDUINO_ISR_ATTR isr(void* arg);
void Task_Switch();
void Task_Init();
void Task_Loop();

extern INA226 ina;
extern lv_ui guider_ui;
extern Adafruit_NeoPixel pixels;
extern BleKeyboard bleKeyboard;
extern lv_obj_t *label;
extern Yeelight* yeelight;
extern Motor* rmd_s;
extern Adafruit_MPU6050 mpu;
extern hw_timer_t * timer;
extern hw_timer_t * timer_ble;
extern TFT_eSPI tft; /* TFT instance */
extern uint8_t sys_status;
extern uint8_t mission_flag;
extern uint8_t mission_status;
extern uint8_t volume_flag;
extern uint8_t count_flag;
extern int64_t i;
extern bool yeelight_initialized;
extern Button button1;


#endif