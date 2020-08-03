/* *****************************************************************

   Download latest Blinker library here:
   https://github.com/blinker-iot/blinker-library/archive/master.zip


   Blinker is a cross-hardware, cross-platform solution for the IoT.
   It provides APP, device and server support,
   and uses public cloud services for data transmission and storage.
   It can be used in smart home, data monitoring and other fields
   to help users build Internet of Things projects better and faster.

   Make sure installed 2.5.0 or later ESP8266/Arduino package,
   if use ESP8266 with Blinker.
   https://github.com/esp8266/Arduino/releases

   Make sure installed 1.0.2 or later ESP32/Arduino package,
   if use ESP32 with Blinker.
   https://github.com/espressif/arduino-esp32/releases

   Docs: https://diandeng.tech/doc
         https://github.com/blinker-iot/blinker-doc/wiki

 * *****************************************************************

   Blinker 库下载地址:
   https://github.com/blinker-iot/blinker-library/archive/master.zip

   Blinker 是一套跨硬件、跨平台的物联网解决方案，提供APP端、设备端、
   服务器端支持，使用公有云服务进行数据传输存储。可用于智能家居、
   数据监测等领域，可以帮助用户更好更快地搭建物联网项目。

   如果使用 ESP8266 接入 Blinker,
   请确保安装了 2.5.0 或更新的 ESP8266/Arduino 支持包。
   https://github.com/esp8266/Arduino/releases

   如果使用 ESP32 接入 Blinker,
   请确保安装了 1.0.2 或更新的 ESP32/Arduino 支持包。
   https://github.com/espressif/arduino-esp32/releases

   文档: https://diandeng.tech/doc
         https://github.com/blinker-iot/blinker-doc/wiki

 * *****************************************************************/

#define BLINKER_WIFI

//定义小爱同学
#define BLINKER_MIOT_OUTLET

#include <Blinker.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Gree.h>


const uint16_t kIrLed = 16;  // ESP8266 GPIO pin to use. 16

IRGreeAC gree_ac(kIrLed);


char auth[] = "96f061be83e1";
char ssid[] = "Venus";
char pswd[] = "prometheus";

unsigned char Gree_Mode_Statu = 0;

// 新建组件对象
BlinkerButton AC_Power("btn-switch");
BlinkerButton AC_Mode("btn-mode");

BlinkerButton AC_Plus("btn-up");
BlinkerButton AC_Minus("btn-down");

BlinkerButton AC_Fan("btn-fan");
BlinkerButton AC_Temp("btn-temp");
BlinkerButton AC_Swap("btn-swap");

BlinkerButton AC_Light("btn-bk-light");
BlinkerButton AC_Sleep("btn-sleep");
BlinkerButton AC_Timer("btn-timer");

BlinkerText TempDis("tex-temp");
BlinkerText TimerDis("tex-timer");
BlinkerText FanSpeed("tex-fan");
BlinkerText DisMode("tex-mode");

int counter = 0;
unsigned char TIMER = 0;
uint16_t timer_value = 30;

bool oState = false;

void AC_Power_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);

  //开关
  if (state == BLINKER_CMD_ON)
  {
    gree_ac.on();
    gree_ac.send();
    // app
    AC_Power.print("on");

    //小爱同学
    BlinkerMIOT.powerState("on");
    BlinkerMIOT.print();

    oState = true;
  }
  else if (state == BLINKER_CMD_OFF)
  {
    gree_ac.off();
    gree_ac.send();
    // app
    AC_Power.print("off");

    //小爱同学
    BlinkerMIOT.powerState("off");
    BlinkerMIOT.print();

    oState = false;
  }
  //指示灯
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);
  digitalWrite(LED_BUILTIN, HIGH);
}

// kGreeAuto, kGreeDry, kGreeCool, kGreeFan, kGreeHeat
void AC_Mode_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);

  if (Gree_Mode_Statu == 0)
  {
    gree_ac.setMode(kGreeCool);
    gree_ac.send();
    DisMode.print("COOL");
  }
  else if (Gree_Mode_Statu == 1)
  {
    gree_ac.setMode(kGreeDry);
    gree_ac.send();
    DisMode.print("DRY");
  }
  else if (Gree_Mode_Statu == 2)
  {
    gree_ac.setMode(kGreeAuto);
    gree_ac.send();
    DisMode.print("AUTO");
  }
  else if (Gree_Mode_Statu == 3)
  {
    gree_ac.setMode(kGreeFan);
    gree_ac.send();
    DisMode.print("FAN", 0);
  }
  else if (Gree_Mode_Statu == 4)
  {
    gree_ac.setMode(kGreeHeat);
    gree_ac.send();
    DisMode.print("HEAT", 0);
  }
  BLINKER_LOG("Mode", Gree_Mode_Statu);

  Gree_Mode_Statu++;
  if (Gree_Mode_Statu >= 5)
    Gree_Mode_Statu = 0;

  //指示灯
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);
  digitalWrite(LED_BUILTIN, HIGH);
}
// Sec
void AC_Plus_callback(const String & state)
{
  uint8_t s_temp = 0;

  BLINKER_LOG("get button state: ", state);

  if (TIMER == 1)
  {
    timer_value = timer_value + 30;
    if (timer_value >= 720 )
      timer_value = 720;
    TimerDis.print("Set Time", timer_value);

  }
  else if (TIMER == 0 || TIMER == 2) {
    s_temp = gree_ac.getTemp() + 1;

    if (s_temp >= 30)
      s_temp = 30;
    BLINKER_LOG("tempertature", s_temp);
    gree_ac.setTemp(s_temp);// 16-30C
    gree_ac.send();

    TempDis.print("Temperature", s_temp);
  }

  //指示灯
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);
  digitalWrite(LED_BUILTIN, HIGH);
}
void AC_Minus_callback(const String & state)
{
  uint8_t s_temp = 0;

  if (TIMER == 1)
  {
    timer_value = timer_value - 30;
    if (timer_value <= 0 )
      timer_value = 30;
    TimerDis.print("Set Time", gree_ac.getTimer());
  }
  else if (TIMER == 0 || TIMER == 2) {
    BLINKER_LOG("get button state: ", state);

    s_temp = gree_ac.getTemp() - 1;

    if (s_temp <= 16)
      s_temp = 16;

    gree_ac.setTemp(s_temp);// 16-30C
    gree_ac.send();

    TempDis.print("Temperature", s_temp);
  }

  //指示灯
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);
  digitalWrite(LED_BUILTIN, HIGH);
}

// Third
void AC_Fan_callback(const String & state)
{

  BLINKER_LOG("get button state: ", state);

  static unsigned char speeds = 0;

  gree_ac.setFan(speeds);
  gree_ac.send();

  if (speeds == 0)
    FanSpeed.print("Auto");
  else if (speeds == 1)
    FanSpeed.print("Level 1");
  else if (speeds == 2)
    FanSpeed.print("Level 2");
  else if (speeds == 3)
    FanSpeed.print("Level 3");

  speeds++;
  if (speeds >= 4)
    speeds = 0;

  //指示灯
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);
  digitalWrite(LED_BUILTIN, HIGH);
}

void AC_Temp_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);
  //开关
  //    if(state == BLINKER_CMD_ON)
  //    {
  //      gree_ac.setLight(true);
  //      gree_ac.send();
  //      //app按键部分
  //      AC_Light.print("on");
  //    }
  //    else if(state == BLINKER_CMD_OFF)
  //    {
  //      gree_ac.setLight(false);
  //      gree_ac.send();
  //      //app按键部分
  //      AC_Light.print("off");
  //    }
}

void AC_Swap_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);
  static unsigned char SW = 0;
  if (!SW) {
    gree_ac.setSwingVertical(true, kGreeSwingAuto);
    gree_ac.send();
    SW = 1;
  }
  else {
    gree_ac.setSwingVertical(false, kGreeSwingAuto);
    gree_ac.send();
    SW = 0;
  }

  //指示灯
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);
  digitalWrite(LED_BUILTIN, HIGH);
}

//Fourth
void AC_Light_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);

  //开关
  if (state == BLINKER_CMD_ON)
  {
    gree_ac.setLight(true);
    gree_ac.send();
    //app按键部分
    AC_Light.print("on");
  }
  else if (state == BLINKER_CMD_OFF)
  {
    gree_ac.setLight(false);
    gree_ac.send();
    //app按键部分
    AC_Light.print("off");
  }

  //指示灯
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);
  digitalWrite(LED_BUILTIN, HIGH);
}

void AC_Sleep_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);
  static unsigned char SW = 0;
  if (!SW) {
    gree_ac.setSleep(true);
    gree_ac.send();
    SW = 1;
  }
  else {
    gree_ac.setSleep(false);
    gree_ac.send();
    SW = 0;
  }

  //指示灯
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);
  digitalWrite(LED_BUILTIN, HIGH);
}

void AC_Timer_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);

  // Second Press
  if (TIMER == 1) {
    gree_ac.setTimer(20);
    gree_ac.send();
    BLINKER_LOG("open timer");
  }
  else if (TIMER == 2) {
    gree_ac.setTimer(0);
    gree_ac.send();
    BLINKER_LOG("close timer");
  }

  // First Press
  if (TIMER == 0)
    TIMER = 1;
  else if (TIMER == 1)
    TIMER = 2;
  else if (TIMER == 2)
    TIMER = 0;

  //指示灯
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);
  digitalWrite(LED_BUILTIN, HIGH);
}
// 小同学询问事件
void miotQuery(int32_t queryCode)
{
  BLINKER_LOG("MIOT Query codes: ", queryCode);

  switch (queryCode)
  {
    case BLINKER_CMD_QUERY_ALL_NUMBER :
      BLINKER_LOG("MIOT Query All");
      BlinkerMIOT.powerState(oState ? "on" : "off");
      BlinkerMIOT.print();
      break;
    case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
      BLINKER_LOG("MIOT Query Power State");
      BlinkerMIOT.powerState(oState ? "on" : "off");
      BlinkerMIOT.print();
      break;
    default :
      BlinkerMIOT.powerState(oState ? "on" : "off");
      BlinkerMIOT.print();
      break;
  }
}

// 如果未绑定的组件被触发，则会执行其中内容
void dataRead(const String & data)
{
  BLINKER_LOG("Blinker readString: ", data);
  counter++;
  //Number1.print(counter);
}

void setup()
{
  // 初始化串口
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  BLINKER_DEBUG.debugAll();

  // 初始化有LED的IO
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  //格力空调
  gree_ac.begin();

  gree_ac.setMode(kGreeCool);
  gree_ac.setTemp(27);
  gree_ac.setXFan(false);
  gree_ac.setLight(true);
  gree_ac.setSleep(false);
  gree_ac.setTurbo(false);
  gree_ac.setTimer(0);

  //Gree Air condiction
  AC_Power.attach(AC_Power_callback);
  AC_Mode.attach(AC_Mode_callback);

  AC_Plus.attach(AC_Plus_callback);
  AC_Minus.attach(AC_Minus_callback);

  AC_Fan.attach(AC_Fan_callback);
  AC_Temp.attach(AC_Temp_callback);
  AC_Swap.attach(AC_Swap_callback);

  AC_Light.attach(AC_Light_callback);
  AC_Sleep.attach(AC_Sleep_callback);
  AC_Timer.attach(AC_Timer_callback);

  // 初始化blinker
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachData(dataRead);

  BlinkerMIOT.attachPowerState(AC_Power_callback);
  BlinkerMIOT.attachQuery(miotQuery);
}

void loop() {
  Blinker.run();
}
