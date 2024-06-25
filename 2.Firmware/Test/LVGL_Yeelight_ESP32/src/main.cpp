/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */
#include <ArduinoJson.h>
#include <lvgl.h>
#include <WiFi.h>
#include <yeelight.h>
#if LV_USE_TFT_ESPI
#include <TFT_eSPI.h>
#endif

/*Set to your screen resolution and rotation*/
#define TFT_HOR_RES   240
#define TFT_VER_RES   240
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

/*User value*/
lv_obj_t *label;
String LVGL_Arduino = "Hello Arduino! ";
int64_t i=0;
Yeelight* yeelight = new Yeelight();
bool initialized = false;
bool inactive = false;
int my_toggle1 = 0;  //触发事件1标志位
int my_toggle2 = 0;  //触发事件2标志位
int my_toggle3 = 0;  //...
int new_bright = 50;    //亮度---后续用foc电机输入控制

/*LVGL LOG*/
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
    lv_display_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data )
{

}

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
    return millis();
}

void Yeelight_Linking()
{
    while (!yeelight->feedback()) 
    {
        yeelight->lookup();
        Serial.printf( "." );
        delay(50);
    }
    Serial.printf( "\r\nYeelight detected!\r\n" );
}

void test_label(lv_timer_t* timer)
{
    i+=1;
    LVGL_Arduino = "Sec_count="+String(i)+String("s");
    Serial.printf(LVGL_Arduino.c_str());
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

void setup()
{

    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.begin( 115200 );
    Serial.println( LVGL_Arduino );
    myWiFi_init();


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
    lv_display_set_rotation(disp, TFT_ROTATION);

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

    //lv_example_btn_1();
    /*
     * Or try out a demo. Don't forget to enable the demos in lv_conf.h. E.g. LV_USE_DEMOS_WIDGETS
     * -------------------------------------------------------------------------------------------
     */
     //lv_demo_widgets();

    label = lv_label_create( lv_screen_active() );
    lv_timer_create(test_label,1000,&LVGL_Arduino);
    Serial.printf( "LVGL Setup done\r\n" );

    /* Yeelight Search */
    yeelight->lookup();
    Serial.printf( "Yeelight Looking" );
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    if (!initialized) 
    {
        Yeelight_Linking();
        initialized = true;
        inactive = false;
    }
    if(my_toggle1)
    {
        Serial.printf("Yeelight Toggled\r\n");
        Serial.println(yeelight->sendCommand("toggle", "[]"));
    }

    if(my_toggle2)
    {
        /* brightness control */
        String brightness_params = String("[") + new_bright + ", \"smooth\", 500]";
        Serial.println(yeelight->sendCommand("set_bright", brightness_params));
    }

    if(my_toggle3)
    {
        /* set default */
        Serial.printf("Yeelight Default Set\r\n");
        Serial.println(yeelight->sendCommand("set_default", "[]"));
    }

    //------------后续应用方法封装好后调试再添加（根据灯的效果很快就可以写好）

}