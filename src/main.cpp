/************* 用户配置区 *****************/
const char *SSID = "YOUR_WIFI";
const char *PWD  = "YOUR_PASS";
const char *MQTT_BROKER = "192.168.x.x";
const uint16_t MQTT_PORT = 1883;
const char *MQTT_USER = "";
const char *MQTT_PASS = "";
const char *OTA_PWD = "espota";
/******************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

WiFiClient espClient;
PubSubClient mqtt(espClient);

/*----------- 引脚 -----------*/
const uint8_t PIN_POT = A0;
const uint8_t PIN_BTN = 14; // D5
const uint8_t PIN_LED = 2;  // 板载 LED（反向）

/*----------- 逻辑 -----------*/
struct Light {
  uint8_t  id;          // 1 或 2
  bool     on;          // 开关状态
  uint8_t  bri;         // 0-255
  uint16_t raw;         // 上次原始 ADC
  char     topicSet[32];
  char     topicGet[32];
  char     topicStat[32];
};

Light lights[2] = {
  {1, true, 128, 512, "home/light1/brightness/set", "home/light1/brightness/get", "home/light1/status"},
  {2, true, 128, 512, "home/light2/brightness/set", "home/light2/brightness/get", "home/light2/status"}
};
uint8_t curLight = 0;   // 0 或 1

unsigned long lastPotMs = 0, lastBtnMs = 0, lastSyncMs = 0;
bool otaMode = false;

/*-------------- 工具 --------------*/
void publishLight(Light &l) {
  char payload[4];
  snprintf(payload, sizeof(payload), "%d", l.on ? l.bri : 0);
  mqtt.publish(l.topicSet, payload, true);
  mqtt.publish(l.topicStat, l.on ? "ON" : "OFF", true);
}

void callback(char* topic, byte* payload, unsigned int len) {
  for (auto &l : lights) {
    if (strcmp(topic, l.topicGet) == 0) {
      uint8_t v = atoi((char*)payload);
      l.bri = constrain(v, 0, 255);
      l.on  = (l.bri > 0);
      /* 把 ADC 也同步过去，避免跳变 */
      l.raw = map(l.bri, 0, 255, 0, 1023);
      return;
    }
  }
}

/*-------------- 初始化 --------------*/
void setup() {
  Serial.begin(115200);
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH); // 熄灭

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(PIN_LED, LOW); delay(200);
    digitalWrite(PIN_LED, HIGH); delay(200);
  }

  ArduinoOTA.setPassword(OTA_PWD);
  ArduinoOTA.onStart([]() { otaMode = true; });
  ArduinoOTA.begin();

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(callback);
}

/*-------------- 主循环 --------------*/
void loop() {
  if (WiFi.status() != WL_CONNECTED) ESP.restart();

  if (!mqtt.connected()) {
    if (mqtt.connect("ESP8266Knob", MQTT_USER, MQTT_PASS)) {
      for (auto &l : lights) mqtt.subscribe(l.topicGet);
    } else {
      delay(5000); return;
    }
  }
  mqtt.loop();
  ArduinoOTA.handle();

  /*-------- 旋钮 --------*/
  uint16_t adc = analogRead(PIN_POT);
  Light &l = lights[curLight];
  if (abs((int)adc - (int)l.raw) > 2 && millis() - lastPotMs > 200) {
    l.raw = adc;
    l.bri = map(constrain(adc, 0, 1023), 0, 1023, 0, 255);
    l.on  = (l.bri > 0);
    publishLight(l);
    lastPotMs = millis();
  }

  /*-------- 按键 --------*/
  static bool lastBtn = HIGH;
  bool btn = digitalRead(PIN_BTN);
  if (btn != lastBtn && millis() - lastBtnMs > 50) {
    lastBtn = btn; lastBtnMs = millis();
    if (btn == LOW) {           // 按下
      unsigned long t = millis();
      while (digitalRead(PIN_BTN) == LOW) {
        delay(10);
        if (millis() - t > 1000) {  // 长按进入 OTA
          otaMode = true;
          for (int i = 0; i < 20; i++) {
            digitalWrite(PIN_LED, LOW); delay(100);
            digitalWrite(PIN_LED, HIGH); delay(100);
          }
          ArduinoOTA.begin();
          return;
        }
      }
      // 短按：切换当前灯 on/off
      l.on = !l.on;
      if (l.on && l.bri == 0) l.bri = 128; // 默认中等亮度
      publishLight(l);
    }
  }

  /*-------- 定时同步 --------*/
  if (millis() - lastSyncMs > 1000) {
    for (auto &l : lights) publishLight(l);
    lastSyncMs = millis();
  }

  /*-------- LED 状态 --------*/
  if (otaMode) {
    digitalWrite(PIN_LED, (millis() / 200) % 2);
  } else {
    digitalWrite(PIN_LED, lights[curLight].on ? HIGH : LOW);
  }
}
