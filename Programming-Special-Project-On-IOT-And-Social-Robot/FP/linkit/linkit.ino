#include <Grove_LED_Bar.h>
// #include <LBLE.h>
// #include <LBLEPeriphral.h>
#include <LWiFi.h>

#include "DHT.h"
#include "MCS.h"

// int t;
// int h;
int i, l;

char _lwifi_ssid[] = "TELDAP-2";
char _lwifi_pass[] = "TELDAP4F";

MCSLiteDevice mcs("ry8IekG3Y", "f2a8a1ec59aa6b86ad7b375a28c05ed5de64c554569e71f89b4dc5ffc0397971",
                  "59.124.152.117", 3000);
// MCSDisplayInteger temp("temp");
// MCSDisplayInteger humid("humid");
MCSDisplayInteger light("light");

// LBLEService counterService("4e38e0c3-ab04-4c5d-b54a-852900379bb3");
// LBLECharacteristicInt counterCharacteristic("4e38e0c4-ab04-4c5d-b54a-852900379bb3",
//                                             LBLE_READ | LBLE_WRITE);

// DHT __dht2(2, DHT22);
Grove_LED_Bar __bar2(3, 2, 0);

// socket // new
const uint16_t port = 8080;
const char* host = "10.50.2.30";  // TODO. WAIT FOR SERVER IP
WiFiClient client;

// void lblesetup() {
//     LBLE.begin();
//     while (!LBLE.ready()) {
//         delay(100);
//     }
//     Serial.println("BLE ready");
//     Serial.print("device address is:");
//     Serial.println(LBLE.getDeviceAddress());  //顯示7697 BLE addr

//     counterService.addAttribute(counterCharacteristic);

//     LBLEPeripheral.addService(counterService);
//     LBLEPeripheral.begin();

//     LBLEAdvertisementData __advertisement;
//     __advertisement.configAsConnectableDevice("guai guai");
//     LBLEPeripheral.advertise(__advertisement);
// }

void read() {
    // counterCharacteristic.setValue(1);
    // LBLEPeripheral.notifyAll(counterCharacteristic);

    // // while (!counterCharacteristic.isWritten()) {
    // //     ;
    // // }

    // if (counterCharacteristic.isWritten()) {
    //     readVal = counterCharacteristic.getValue();
    //     Serial.print("BLE: ");
    //     Serial.println(readVal);

    //     setLED();
    // }

    // socket new
    if (client.connected() && (client.available() > 0)) {
        Serial.print("set brightness to: ");
        int size;
        int readVal = 0;
        while ((size = client.available()) > 0) {
            uint8_t* msg = (uint8_t*)malloc(size);
            size = client.read(msg, size);
            // String tmp = (char*)msg;
            readVal *= 10;
            readVal += (int)*msg - '0';
            // readVal += tmp.toInt();
            // Serial.write(msg, size);
            // Serial.println(tmp);
            free(msg);
        }
        // readVal /= 1300;
        Serial.println(readVal);
        if (readVal > 10)
            readVal = 10;
        else if (readVal < 0)
            readVal = 0;
        setLED(readVal);
    }
}

void setLED(int readVal) {
    __bar2.setLevel(readVal);
    Serial.println("LED change");
}

void socketSetup() {  // socket // new
    while (!client.connect(host, port)) {
        Serial.println("trying connecting...");
        delay(1000);
    }
    Serial.println("socket created");
}

void setup() {
    Serial.begin(9600);

    // mcs.addChannel(temp);
    // mcs.addChannel(humid);
    mcs.addChannel(light);
    Serial.println("Wi-Fi 開始連線");
    while (WiFi.begin(_lwifi_ssid, _lwifi_pass) != WL_CONNECTED) {
        delay(1000);
    }
    Serial.println("Wi-Fi 連線成功");
    while (!mcs.connected()) {
        mcs.connect();
        Serial.println("reconnecting...");
    }
    Serial.println("MCS 連線成功");
    // Serial.begin(9600);
    // __dht2.begin();
    // __bar2.begin();

    // lblesetup();
    __bar2.begin();
    socketSetup();  // new
}

void loop() {
    while (!mcs.connected()) {
        mcs.connect();
        if (mcs.connected()) {
            Serial.println("MCS 已重新連線");
        }
    }
    mcs.process(100);

    // if (LBLEPeripheral.connected()) {
    //     Serial.println("BLE connected");
    // }

    // t = __dht2.readTemperature();
    // h = __dht2.readHumidity();
    // temp.set(t);
    // humid.set(h);

    l = analogRead(0);
    light.set(l);
    // Serial.println(l);

    // Serial.print("攝氏溫度 : ");
    // Serial.println(t);
    // Serial.print("相對溼度 :");
    // Serial.println(h);

    // for (i = 0; i <= 10; i++){
    //     __bar2.setLevel(i);
    //     delay(1000);
    //     Serial.println(analogRead(0));
    // }

    read();

    delay(1000);
}