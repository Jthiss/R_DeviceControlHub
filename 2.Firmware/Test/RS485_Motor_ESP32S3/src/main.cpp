#include <ArduinoJson.h>
#include <lvgl.h>
#include <WiFi.h>
#include <BleKeyboard.h>
#include <Adafruit_NeoPixel.h>
#include <yeelight.h>
#include <motor.h>
#if LV_USE_TFT_ESPI
#include <TFT_eSPI.h>
#endif
//#include <examples/lv_examples.h>
#include <demos/lv_demos.h>

#define LED_PIN 48   //对应的是板上的WS2812
#define LED_NUM  1   //WS2812数量


/*Set to your screen resolution and rotation*/
#define TFT_HOR_RES   240
#define TFT_VER_RES   240
#define TFT_ROTATION  LV_DISPLAY_ROTATION_1
/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))



/*User value*/
// TFT_eSPI tft = TFT_eSPI(); 
Adafruit_NeoPixel pixels(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);
BleKeyboard bleKeyboard = BleKeyboard("My Esp32BLE");
lv_obj_t *label;
Yeelight* yeelight = new Yeelight();
Motor* rmd_s = new Motor();
hw_timer_t * timer = NULL;
hw_timer_t * timer_ble = NULL;
uint32_t draw_buf[DRAW_BUF_SIZE / 4];
String LVGL_Arduino = "Hello Arduino! ";
int64_t i=0;
int my_toggle1 = 0;  //触发事件1标志位
int my_toggle2 = 0;  //触发事件2标志位
int my_toggle3 = 0;  //...
int new_bright = 50;    //亮度---后续用foc电机输入控制
uint8_t sys_status = 1;
uint8_t mission_flag = 0;
uint8_t volume_flag = 1;


void my_print( lv_log_level_t level, const char * buf );
void my_disp_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map);
static uint32_t my_tick(void);
void my_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data );
void Yeelight_Linking();
void test_label(lv_timer_t* timer);
void myWiFi_init();
void timer_interrupt();
void timer_ble_interrupt();
void My_LVGL_Init();


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

    label = lv_label_create( lv_screen_active() );
    lv_timer_create(test_label,1000,&LVGL_Arduino);
    Serial.printf( "LVGL Setup done\r\n" );

    /* Yeelight Search */
    yeelight->lookup();
    Serial.printf( "Yeelight Looking" );
    
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
        mission_flag = 0;
    }

    //定时器1中断标志位对应处理函数
    if(volume_flag == 1)
    {
        //尺度设为50（理论上尺度就是要小才能进行小的音量变换
        //---但30以内是不行的，编码器数据有波动，实际导致音量变太多是因为读取太多次，就是定时器时间太短）
        uint8_t volume_status = rmd_s->checkVolume(70);                                                       
        if(volume_status==1)
            bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
        else if(volume_status==2)
            bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
        volume_flag = 0;
        Serial.printf("test_v\r\n");
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
void my_print( lv_log_level_t level, const char * buf )
{
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}
#endif

/* LVGL calls it when a rendered image needs to copied to the display*/
void my_disp_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map)
{
    /*Call it to tell LVGL you are ready*/
    lv_display_flush_ready(disp);
}

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
    return millis();
}


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
    /*LVGL Init*/
    lv_init();

    /*Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb(my_tick);

    /* register print function for debugging */
#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print );
#endif

    lv_display_t * disp;
#if LV_USE_TFT_ESPI
    /*TFT_eSPI can be enabled lv_conf.h to initialize the display in a simple way*/
    disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
#else
    /*Else create a display yourself*/
    disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

    /*Initialize the (dummy) input device driver*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, my_touchpad_read);

    /* Create a simple label
     * ---------------------
     lv_obj_t *label = lv_label_create( lv_scr_act() );
     lv_label_set_text( label, "Hello Arduino, I'm LVGL!" );
     lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );

     * Try an example. See all the examples
     *  - Online: https://docs.lvgl.io/master/examples.html
     *  - Source codes: https://github.com/lvgl/lvgl/tree/master/examples
     * ----------------------------------------------------------------

     lv_example_btn_1();

     * Or try out a demo. Don't forget to enable the demos in lv_conf.h. E.g. LV_USE_DEMOS_WIDGETS
     * -------------------------------------------------------------------------------------------

     lv_demo_widgets();
     */
    lv_demo_widgets();
}
