#include <ArduinoJson.h>
#include <lvgl.h>
#include <WiFi.h>
#include <BleKeyboard.h>
#include <Adafruit_NeoPixel.h>
#include <yeelight.h>
#include <motor.h>
#include <Wire.h>
#include <INA226.h>
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


/*Change to your screen resolution*/
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];


/*User value*/
INA226 ina(Wire);
lv_ui guider_ui;
Adafruit_NeoPixel pixels(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);
BleKeyboard bleKeyboard = BleKeyboard("My Esp32BLE");
lv_obj_t *label;
Yeelight* yeelight = new Yeelight();
Motor* rmd_s = new Motor();
hw_timer_t * timer = NULL;
hw_timer_t * timer_ble = NULL;
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */
String LVGL_Arduino = "Hello Arduino! ";
int64_t i=0;
int my_toggle1 = 0;  //触发事件1标志位
int my_toggle2 = 0;  //触发事件2标志位
int my_toggle3 = 0;  //...
int new_bright = 50;    //亮度---后续用foc电机输入控制
uint8_t sys_status = 1;
uint8_t mission_flag = 0;
uint8_t volume_flag = 1;
uint8_t count_flag = 0;



void Yeelight_Linking();
void test_label(lv_timer_t* timer);
void myWiFi_init();
void timer_interrupt();
void timer_ble_interrupt();
void My_LVGL_Init();
void INA226_Init();
void INA226_Read();

void setup()
{

    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    //系统状态灯
    pixels.begin();
    pixels.setPixelColor(0, pixels.Color(0,20,20)); // 荧光蓝色
    pixels.show();

    Serial.begin( 115200 );
    //Serial.println( LVGL_Arduino );

    //用于RS485通讯对应串口
    Serial1.begin(115200);  

    //配置蓝牙
    bleKeyboard.begin();

    //配置INA
    INA226_Init();

    // 初始化定时器
    timer = timerBegin(0,80,true);
    timer_ble = timerBegin(1,80,true);

    // 配置定时器
    timerAttachInterrupt(timer,timer_interrupt,true);
    timerAttachInterrupt(timer_ble,timer_ble_interrupt,true);
    
    //配置WiFi
    myWiFi_init();

    //配置LVGL
    My_LVGL_Init();
    

    /* Yeelight Search */
    yeelight->lookup();
    Serial.printf( "Yeelight Looking" );
    Serial.printf("FreeHeap = %dbytes \r\nHeapSize = %dbytes \r\nPsramSize = %dbytes \r\nSketchSize = %dbytes \r\n", ESP.getFreeHeap(),ESP.getHeapSize(),ESP.getPsramSize(),ESP.getSketchSize());
}

void mission1()
{
    //定時器設置
    // 定时模式，单位us，可重裝載
    timerAlarmWrite(timer,200000,true);     //每0.2s讀取一次
    timerAlarmWrite(timer_ble,100000,true); //每0.1s讀取一次音量
    // 启动定时器
    timerAlarmEnable(timer);    
    timerAlarmEnable(timer_ble); 
}

void loop()
{
    /* LVGL */
    lv_timer_handler(); /* let the GUI do its work */

    //任务初始化（切换任务前可以结束的任务都结束，资源该释放的释放）
    if(sys_status)  //初始化只运行一次
    {
        sys_status = 0;
        //判断任务标志位：
        //旋鈕定時發送讀取編碼器
        mission1();
    }
    


    //定时器0中断标志位对应处理函数
    if(mission_flag == 1)
    {
        //判断任务标志位：（後續多任務改爲switch）
        //發送讀取編碼器指令
        rmd_s->readMultiTurnAngle();
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
        //尺度设为50（理论上尺度就是要小才能进行小的音量变换
        //---但30以内是不行的，编码器数据有波动，实际导致音量变太多是因为读取太多次，就是定时器时间太短）
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
    
    // if (!initialized) 
    // {
    //     Yeelight_Linking();
    //     initialized = true;
    // }
    // if(my_toggle1)
    // {
    //     Serial.printf("Yeelight Toggled\r\n");
    //     Serial.println(yeelight->sendCommand("toggle", "[]"));
    //     delay(5000);
    // }

    // if(my_toggle2)
    // {
    //     /* brightness control */
    //     String brightness_params = String("[") + new_bright + ", \"smooth\", 500]";
    //     Serial.println(yeelight->sendCommand("set_bright", brightness_params));
    // }

    // if(my_toggle3)
    // {
    //     /* set default */
    //     Serial.printf("Yeelight Default Set\r\n");
    //     Serial.println(yeelight->sendCommand("set_default", "[]"));
    // }

    //轮询任务：用于查询（按键触发、串口接收等）
    rmd_s->parseMultiTurnAngleResponse();
    

    //delay(100);  //延時過程中接受n次數據幀則需要在n次循環顯示(旋鈕模式盡量不設置延時)
    //------------后续应用方法封装好后调试再添加（根据灯的效果很快就可以写好）
    
}   



#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
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

void test_label(lv_timer_t* timer)
{
    i+=1;
    LVGL_Arduino = "Sec_count="+String(i)+String("s");
    // Serial.printf(LVGL_Arduino.c_str());
    lv_label_set_text( label, LVGL_Arduino.c_str() );
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
}

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

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    //static lv_indev_drv_t indev_drv;
    //lv_indev_drv_init( &indev_drv );
    //indev_drv.type = LV_INDEV_TYPE_POINTER;
    //indev_drv.read_cb = my_touchpad_read;
    //lv_indev_drv_register( &indev_drv );

#if 1

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
    lv_demo_widgets();            // OK
    // lv_demo_benchmark();          // OK
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

void INA226_Init()
{
    Wire.begin(13 ,14);
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

    Serial.println("-----------------------------------------------");
}

void INA226_Read()
{
    Serial.print("Bus voltage:   ");
    char Bus_voltage[10];
    sprintf(Bus_voltage,"%.5f",ina.readBusVoltage());
    strcat(Bus_voltage," V");
    Serial.printf("%s\r\n",Bus_voltage);
    lv_event_send(guider_ui.screen_label_1,LV_EVENT_VALUE_CHANGED,Bus_voltage);

    Serial.print("Bus power:     ");
    Serial.print(ina.readBusPower(), 5);
    Serial.println(" W");


    Serial.print("Shunt voltage: ");
    Serial.print(ina.readShuntVoltage(), 5);
    Serial.println(" V");

    Serial.print("Shunt current: ");
    Serial.print(ina.readShuntCurrent(), 5);
    Serial.println(" A");

    Serial.println("");
}