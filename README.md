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
1. 修改 `src/main.cpp` 中的 SSID、PWD、MQTT_BROKER 等参数。
2. 重新上传固件。

## 3. 导入 Home Assistant
1. 将 `ha_config.yaml` 中的内容复制到 `configuration.yaml` 文件中。
2. 重启 Home Assistant。
3. 在仪表盘中添加 Light 卡片，即可看到「旋钮灯 1」和「旋钮灯 2」。

## 4. 接线图
- 电位器中间引脚接 A0。
- 按键一端接 D5 (GPIO14)，另一端接 GND。
- 板载 LED (GPIO2) 用于状态指示。

## 5. 功能说明
- 旋转旋钮：实时调节灯光亮度。
- 短按按键：切换当前灯的开关状态。
- 长按按键：进入 OTA 升级模式。


## 硬件列表
esp8266开发板x1
WH148 10K B 线性 电位器x1 或者
（RV09 10K 微型 电位器 单圈
）x1
6x6mm轻触开关x1
