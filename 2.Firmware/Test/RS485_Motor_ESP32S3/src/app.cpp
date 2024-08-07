#include <app.h>


/*Change to your screen resolution*/
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf_1[ screenWidth * 50 ];
static lv_color_t buf_2[ screenWidth * 50 ];



Button button1 = {8, 0, false};
INA226 ina(Wire);
lv_ui guider_ui;
Adafruit_NeoPixel pixels(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);
BleKeyboard bleKeyboard = BleKeyboard("My Esp32BLE");
lv_obj_t *label;
Yeelight* yeelight = new Yeelight();
Motor* rmd_s = new Motor();
Adafruit_MPU6050 mpu;
hw_timer_t * timer = NULL;
hw_timer_t * timer_ble = NULL;
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

int64_t i=0;
uint8_t sys_status = 1;
uint8_t mission_status = APP_1;
uint8_t mission_flag = 0;
uint8_t volume_flag = 1;
uint8_t count_flag = 0;
bool yeelight_initialized = false;
unsigned long lastDebounceTime = 0;
unsigned long DebounceTime = 0;
int keyState = 0;
int switchState = 0;




#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p ) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();

    // 仅在需要时设置地址窗口
    static lv_area_t last_flush_area;  // 用来记录上一次更新的区域
    if (area->x1 != last_flush_area.x1 || area->y1 != last_flush_area.y1 || area->x2 != last_flush_area.x2 || area->y2 != last_flush_area.y2 ) {
        tft.setAddrWindow(area->x1, area->y1, w, h);
        last_flush_area = *area;  // 更新上一次更新的区域
    }

    // 批量写入颜色数据
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);  // 注意这里使用了area计算总像素数

    tft.endWrite();

    lv_disp_flush_ready(disp);
}

/*
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
    uint16_t touchX, touchY;

    bool touched = tft.getTouch( &touchX, &touchY, 600 );

    if( !touched )
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;


        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print( "Data x " );
        Serial.println( touchX );

        Serial.print( "Data y " );
        Serial.println( touchY );
    }
}
*/


void Yeelight_Linking()
{
    
    while (!yeelight->feedback())   //这里有问题
    {
        yeelight->lookup();
        Serial.printf( "." );
        delay(50);
    }
    
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data )
{
}

// void test_label(lv_timer_t* timer)
// {
//     i+=1;
//     LVGL_Arduino = "Sec_count="+String(i)+String("s");
//     // Serial.printf(LVGL_Arduino.c_str());
//     lv_label_set_text( label, LVGL_Arduino.c_str() );
//     lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
// }

void myWiFi_init()
{
    
    WiFi.begin("LAI", "lzbfc009563A-");
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
}

// 定时器中断处理函数
void timer_interrupt(){
    //判斷當前是什麽模式
    //使得標志位為對應的值
    mission_flag = 1;
}

void timer_ble_interrupt(){
    //判斷當前是什麽模式
    //使得標志位為對應的值
    volume_flag = 1;
}

void My_LVGL_Init()
{
    lv_init();


#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    tft.setRotation( 0 ); /* Landscape orientation, flipped */
    /*Set the touchscreen calibration data,
     the actual data for your display can be acquired using
     the Generic -> Touch_calibrate example from the TFT_eSPI library*/
   // uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
   // tft.setTouch( calData );

    lv_disp_draw_buf_init( &draw_buf, buf_1, buf_2, screenWidth * 50 );


    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.full_refresh = 1;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    //static lv_indev_drv_t indev_drv;
    //lv_indev_drv_init( &indev_drv );
    //indev_drv.type = LV_INDEV_TYPE_POINTER;
    //indev_drv.read_cb = my_touchpad_read;
    //lv_indev_drv_register( &indev_drv );

#if 1
    // lv_demo_stress();
    // lv_example_btn_1();
    setup_ui(&guider_ui);
    events_init(&guider_ui);
    custom_init(&guider_ui);
    /* Create simple label */
   /* lv_obj_t *label = lv_label_create( lv_scr_act() );
    lv_label_set_text( label, LVGL_Arduino.c_str() );
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );*/
#else

    /* Try an example from the lv_examples Arduino library
       make sure to include it as written above.
    lv_example_btn_1();
   */

    // uncomment one of these demos
    // lv_demo_widgets();            // OK
    lv_demo_benchmark();          // OK
    // lv_demo_keypad_encoder();     // works, but I haven't an encoder
    // lv_demo_music();              // NOK
    // lv_demo_printer();
    // lv_demo_stress();             // seems to be OK
#endif
    Serial.println( "LVGL Setup done" );
}


void checkConfig()
{
  Serial.print("Mode:                  ");
  switch (ina.getMode())
  {
    case INA226_MODE_POWER_DOWN:      Serial.println("Power-Down"); break;
    case INA226_MODE_SHUNT_TRIG:      Serial.println("Shunt Voltage, Triggered"); break;
    case INA226_MODE_BUS_TRIG:        Serial.println("Bus Voltage, Triggered"); break;
    case INA226_MODE_SHUNT_BUS_TRIG:  Serial.println("Shunt and Bus, Triggered"); break;
    case INA226_MODE_ADC_OFF:         Serial.println("ADC Off"); break;
    case INA226_MODE_SHUNT_CONT:      Serial.println("Shunt Voltage, Continuous"); break;
    case INA226_MODE_BUS_CONT:        Serial.println("Bus Voltage, Continuous"); break;
    case INA226_MODE_SHUNT_BUS_CONT:  Serial.println("Shunt and Bus, Continuous"); break;
    default: Serial.println("unknown");
  }
  
  Serial.print("Samples average:       ");
  switch (ina.getAverages())
  {
    case INA226_AVERAGES_1:           Serial.println("1 sample"); break;
    case INA226_AVERAGES_4:           Serial.println("4 samples"); break;
    case INA226_AVERAGES_16:          Serial.println("16 samples"); break;
    case INA226_AVERAGES_64:          Serial.println("64 samples"); break;
    case INA226_AVERAGES_128:         Serial.println("128 samples"); break;
    case INA226_AVERAGES_256:         Serial.println("256 samples"); break;
    case INA226_AVERAGES_512:         Serial.println("512 samples"); break;
    case INA226_AVERAGES_1024:        Serial.println("1024 samples"); break;
    default: Serial.println("unknown");
  }

  Serial.print("Bus conversion time:   ");
  switch (ina.getBusConversionTime())
  {
    case INA226_BUS_CONV_TIME_140US:  Serial.println("140uS"); break;
    case INA226_BUS_CONV_TIME_204US:  Serial.println("204uS"); break;
    case INA226_BUS_CONV_TIME_332US:  Serial.println("332uS"); break;
    case INA226_BUS_CONV_TIME_588US:  Serial.println("558uS"); break;
    case INA226_BUS_CONV_TIME_1100US: Serial.println("1.100ms"); break;
    case INA226_BUS_CONV_TIME_2116US: Serial.println("2.116ms"); break;
    case INA226_BUS_CONV_TIME_4156US: Serial.println("4.156ms"); break;
    case INA226_BUS_CONV_TIME_8244US: Serial.println("8.244ms"); break;
    default: Serial.println("unknown");
  }

  Serial.print("Shunt conversion time: ");
  switch (ina.getShuntConversionTime())
  {
    case INA226_SHUNT_CONV_TIME_140US:  Serial.println("140uS"); break;
    case INA226_SHUNT_CONV_TIME_204US:  Serial.println("204uS"); break;
    case INA226_SHUNT_CONV_TIME_332US:  Serial.println("332uS"); break;
    case INA226_SHUNT_CONV_TIME_588US:  Serial.println("558uS"); break;
    case INA226_SHUNT_CONV_TIME_1100US: Serial.println("1.100ms"); break;
    case INA226_SHUNT_CONV_TIME_2116US: Serial.println("2.116ms"); break;
    case INA226_SHUNT_CONV_TIME_4156US: Serial.println("4.156ms"); break;
    case INA226_SHUNT_CONV_TIME_8244US: Serial.println("8.244ms"); break;
    default: Serial.println("unknown");
  }
  
  Serial.print("Max possible current:  ");
  Serial.print(ina.getMaxPossibleCurrent());
  Serial.println(" A");

  Serial.print("Max current:           ");
  Serial.print(ina.getMaxCurrent());
  Serial.println(" A");

  Serial.print("Max shunt voltage:     ");
  Serial.print(ina.getMaxShuntVoltage());
  Serial.println(" V");

  Serial.print("Max power:             ");
  Serial.print(ina.getMaxPower());
  Serial.println(" W");
}

void I2C_Init()
{
    Wire.begin(13 ,14);
    // Try to initialize!
    Wire1.begin(47,21);

    //下來可以測試同I2C
    //----------------------INA226-----------------------------
    // Default INA226 address is 0x40
    bool success = ina.begin();
    // Check if the connection was successful, stop if not
    if(!success)
    {
        Serial.println("INA226 Connection error");
        while(1);
    }
    // Configure INA226
    ina.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);

    // Calibrate INA226. Rshunt = 0.01 ohm, Max excepted current = 4A
    ina.calibrate(0.01, 4);

    // Display configuration
    checkConfig();

    //----------------------MPU6050-----------------------------
    if (!mpu.begin(MPU6050_I2CADDR_DEFAULT,&Wire1,0)) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) {
        delay(10);
        }
    }
    Serial.println("MPU6050 Found!");

    //setupt motion detection
    mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
    mpu.setMotionDetectionThreshold(1);
    mpu.setMotionDetectionDuration(20);
    mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
    mpu.setInterruptPinPolarity(true);
    mpu.setMotionInterrupt(true);

    Serial.println("-----------------------------------------------");
}

void INA226_Read()
{
    
    char Trans_Buf[15];
    float ina_voltage = ina.readBusVoltage();
    float ina_power = ina.readBusPower();
    float ina_svoltage = ina.readShuntVoltage();
    float ina_scurrent = ina.readShuntCurrent();

    char voltage_per[5];
    int voltage_perv;
    //添加逻辑（电压转换为百分比）
    if(ina_voltage>=4.20000)
    {
        sprintf(voltage_per,"%s","100%");
        voltage_perv = 100;
    }
    else if(ina_voltage<=3.60000)
    {
        sprintf(voltage_per,"%s","0%");
        voltage_perv = 0;
    }
    else
    {
        voltage_perv = (int)(((ina_voltage-3.6)/0.6)*100);
        sprintf(voltage_per,"%d",voltage_perv);
        strcat(voltage_per,"%");   
    }
    lv_event_send(guider_ui.screen_arc_1,LV_EVENT_VALUE_CHANGED,&voltage_perv);   
    lv_event_send(guider_ui.screen_label_12,LV_EVENT_VALUE_CHANGED,voltage_per);
     
    // Serial.printf("voltage_per:%s\r\n",voltage_per);
    sprintf(Trans_Buf,"%.5f",ina_voltage);
    strcat(Trans_Buf," V");
    // Serial.printf("Bus voltage:%s\r\n",Trans_Buf);
    lv_event_send(guider_ui.screen_label_1,LV_EVENT_VALUE_CHANGED,Trans_Buf);
    strcpy(Trans_Buf, "");  //清空
    // Serial.printf("Clear voltage:%s\r\n",Trans_Buf);

    sprintf(Trans_Buf,"%.5f",ina_power);
    strcat(Trans_Buf," W");
    // Serial.printf("Bus power:%s\r\n",Trans_Buf);
    lv_event_send(guider_ui.screen_label_5,LV_EVENT_VALUE_CHANGED,Trans_Buf);
    strcpy(Trans_Buf, "");  //清空
    
    sprintf(Trans_Buf,"%.5f",ina_svoltage);
    strcat(Trans_Buf," V");
    // Serial.printf("Shunt voltage:%s\r\n",Trans_Buf);
    lv_event_send(guider_ui.screen_label_8,LV_EVENT_VALUE_CHANGED,Trans_Buf);
    strcpy(Trans_Buf, "");  //清空
    
    sprintf(Trans_Buf,"%.5f",ina_scurrent);
    strcat(Trans_Buf," A");
    // Serial.printf("Shunt current:%s\r\n",Trans_Buf);
    lv_event_send(guider_ui.screen_label_9,LV_EVENT_VALUE_CHANGED,Trans_Buf);
    strcpy(Trans_Buf, "");  //清空
    
    // sprintf(Bus_power,"%.5f",ina_power);
    // strcat(Bus_power," W");
    // Serial.printf("Bus power:%s\r\n",Bus_power);

    
    // sprintf(Shunt_voltage,"%.5f",ina_svoltage);
    // strcat(Shunt_voltage," V");
    // Serial.printf("Shunt voltage:%s\r\n",Shunt_voltage);
    
    
    // sprintf(Shunt_current,"%.5f",ina_scurrent);
    // strcat(Shunt_current," A");
    // Serial.printf("Shunt current:%s\r\n",Shunt_current);
}


void MPU6050_Read()
{
    sensors_event_t a, g, temp;
    char Trans_Buf[10];
    mpu.getEvent(&a, &g, &temp);

    /* Print out the values */
    sprintf(Trans_Buf,"%.2f",a.acceleration.x);
    Serial.print("AccelX:");
    Serial.print(Trans_Buf);
    lv_event_send(guider_ui.screen_1_label_12,LV_EVENT_VALUE_CHANGED,Trans_Buf);
    Serial.print(",");
    sprintf(Trans_Buf,"%.2f",a.acceleration.y);
    Serial.print("AccelY:");
    Serial.print(a.acceleration.y);
    lv_event_send(guider_ui.screen_1_label_14,LV_EVENT_VALUE_CHANGED,Trans_Buf);
    Serial.print(",");
    sprintf(Trans_Buf,"%.2f",a.acceleration.z);
    Serial.print("AccelZ:");
    Serial.print(Trans_Buf);
    lv_event_send(guider_ui.screen_1_label_15,LV_EVENT_VALUE_CHANGED,Trans_Buf);
    Serial.print(", ");
    sprintf(Trans_Buf,"%.2f",g.gyro.x);
    Serial.print("GyroX:");
    Serial.print(Trans_Buf);
    lv_event_send(guider_ui.screen_1_label_16,LV_EVENT_VALUE_CHANGED,Trans_Buf);
    Serial.print(",");
    sprintf(Trans_Buf,"%.2f",g.gyro.y);
    Serial.print("GyroY:");
    Serial.print(Trans_Buf);
    lv_event_send(guider_ui.screen_1_label_17,LV_EVENT_VALUE_CHANGED,Trans_Buf);
    Serial.print(",");
    sprintf(Trans_Buf,"%.2f",g.gyro.z);
    Serial.print("GyroZ:");
    Serial.print(Trans_Buf);
    lv_event_send(guider_ui.screen_1_label_18,LV_EVENT_VALUE_CHANGED,Trans_Buf);
    Serial.println("");
}

void Timer_CB()
{
    //定时器0中断标志位对应处理函数
    if(mission_flag == 1)
    {
        //判断任务标志位：（後續多任務改爲switch）
        //發送讀取編碼器指令
        rmd_s->readMultiTurnAngle();
        MPU6050_Read();
        count_flag++;
        if(count_flag == 5)
        {
            INA226_Read();  //每1s读取一次电源情况
            count_flag = 0;
        }
        mission_flag = 0;
    }

    //定时器1中断标志位对应处理函数
    if(volume_flag == 1)
    {
        uint8_t volume_status = rmd_s->checkVolume(70);                                                       
        if(volume_status==1)
        {
            bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
            Serial.printf("KEY_MEDIA_VOLUME_UP\r\n");
        }  
        else if(volume_status==2)
        {
            bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
            Serial.printf("KEY_MEDIA_VOLUME_DOWN\r\n");
        }
        volume_flag = 0;
        // Serial.printf("test_v\r\n");
    }
}

void Yeelight_Init()
{
    Serial.printf( "Yeelight Looking" );
    if (!yeelight_initialized) 
    {
        Yeelight_Linking();
        yeelight_initialized = true;
    }
}

void ARDUINO_ISR_ATTR isr(void* arg) {
    
    if(keyState == 0) //第一次進入時記錄第一次時間
    {
      keyState = 1;
      lastDebounceTime=millis();
    }
    else  //不爲第一次進入時，判斷是否達到消抖時間要求，沒有就不記為一次按鍵
    {
      if((millis()-lastDebounceTime)>30)  //消抖
      {
        Button* s = static_cast<Button*>(arg);
        s->numberKeyPresses += 1;
        s->pressed = true;
        keyState = 0;
      }
    }
}

void Task_Switch()
{
    if (button1.pressed) {
      Serial.printf("Button 1 has been pressed %u times\n", button1.numberKeyPresses);
      button1.pressed = false;
    }
}

void Task_Init()
{
    switch(mission_status)
    {
        case APP_1:
        {
            //旋鈕定時發送讀取編碼器
            mission1();
        };break;
        case APP_2:
        {
            //旋鈕定時發送讀取編碼器
            mission1();
        };break;
    }
}

void Task_Loop()
{
    switch(mission_status)
    {
        case APP_1:
        {
            //轮询任务：用于查询（按键触发、串口接收等）
            rmd_s->parseMultiTurnAngleResponse();
        };break;
        case APP_2:
        {
            
        };break;
    }
}