# ESP8266 旋钮调光 + Home Assistant 完整项目

## 1. 刷机步骤

### 1.1 首次刷机
1. 用 USB-TTL 或 NodeMCU 自带串口连接电脑。
2. 打开 PlatformIO，选择 `Upload`。
3. 等待上传完成，设备自动重启。

### 1.2 OTA 升级
1. 长按按键 1 s，直到 LED 快闪。
2. 在 PlatformIO 中选择 `Network Port (esp8266_knob_ha_full)`。
3. 点击 `Upload`，输入密码 `espota`。
4. 上传完成后设备自动重启。

## 2. 配置 WiFi 和 MQTT
1. 修改 `src/main.cpp` 中的 SSID、PWD、MQTT_B
