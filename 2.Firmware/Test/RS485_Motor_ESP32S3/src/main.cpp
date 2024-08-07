#include <main.h>

void mission1();

void setup()
{
    //系统状态灯
    pixels.begin();
    pixels.setPixelColor(0, pixels.Color(0,20,20)); // 荧光蓝色
    pixels.show();

    //配置狀態切換按鍵
    pinMode(button1.PIN,INPUT_PULLUP);
    attachInterruptArg(button1.PIN, isr, &button1, FALLING);


    Serial.begin(115200);

    //用于RS485通讯对应串口
    Serial1.begin(115200);      

    //配置蓝牙
    bleKeyboard.begin();

    //配置I2C(INA226 MPU6050)
    I2C_Init();

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
    Yeelight_Init();

    Serial.printf("FreeHeap = %dbytes \r\nHeapSize = %dbytes \r\nPsramSize = %dbytes \r\nSketchSize = %dbytes \r\n", ESP.getFreeHeap(),ESP.getHeapSize(),ESP.getPsramSize(),ESP.getSketchSize());
}


void loop()
{
    Task_Switch();
    /* LVGL */
    lv_timer_handler(); /* let the GUI do its work */

    //任务初始化（切换任务前可以结束的任务都结束，资源该释放的释放）
    if(sys_status)  //初始化只运行一次
    {
        sys_status = 0;
        //判断任务标志位：
        Task_Init();
    }
    
    //定时器任务
    Timer_CB();

    //轮询任务
    Task_Loop();
    
    
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
    
}   

void mission1()
{
    // 定時器設置
    // 定时模式，单位us，可重裝載
    timerAlarmWrite(timer,200000,true);     //每0.2s讀取一次
    timerAlarmWrite(timer_ble,100000,true); //每0.1s讀取一次音量
    // 启动定时器
    timerAlarmEnable(timer);    
    timerAlarmEnable(timer_ble); 
}
