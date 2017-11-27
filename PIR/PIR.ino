//MQTT
#include <PubSubClient.h>
//ESP
#include <ESP8266WiFi.h>
//Wi-Fi Manger library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>//https://github.com/tzapu/WiFiManager
//AP TIMEOUT
#define AP_TIMEOUT 180
//SERIAL DEBUG
#define SERIAL_BAUDRATE 115200

//MQTT SETTINGS
#define MQTT_AUTH false
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

//GPIOS
#define PIR D2

//CONSTANTS
const char* AP_NAME  = "PIRCONTROLLER";
const char* MQTT_PIR_TOPIC = "room/motion";
const char* MQTT_SERVER = "mqtt_broker_ip_dns";

WiFiClient wclient;
PubSubClient client(MQTT_SERVER,1883,wclient);

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
  /*define o tempo limite até o portal de configuração ficar novamente inátivo,
   útil para quando alteramos a password do nosso wi-fi*/
  wifiManager.setTimeout(AP_TIMEOUT);
   //AP com password 12345678 -> wifiManager.autoConnect(AP_NAME,"12345678");
  if(!wifiManager.autoConnect(AP_NAME)) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reinicia a tenta novamente ligar-se ao nosso wi-fi
    ESP.restart();
    delay(5000);
  } 
  pinMode(PIR,INPUT); 
}

bool checkMqttConnection(){
  if (!client.connected()) {
    if (client.connect(AP_NAME)) {
      //SUBSCRIÇÃO DE TOPICOS
      Serial.println("CONNECTED");
    }
  }
  return client.connected();
}

bool lastState = false;
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (checkMqttConnection()){
      client.loop();
    bool realState =  digitalRead(PIR);
      if(lastState != realState){
        lastState = realState;
        if(realState){
          client.publish(MQTT_PIR_TOPIC,"YES");
       }else{
          client.publish(MQTT_PIR_TOPIC,"NO");
       }
    }
    }
  }
}

