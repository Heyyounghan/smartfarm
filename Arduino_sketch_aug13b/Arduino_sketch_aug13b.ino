#include <Wire.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <WiFiEsp.h>
#include "DHT.h"

char ssid[] = "U+Net7828";           //  와이파이에 접속할 SSID
char pass[] = "DD6C015249";             //  와이파이의 비밀번호
const char* server = "192.168.219.103"; //  MQTT서버의 IP

#define BT_RXD 2
#define BT_TXD 3 
#define tri0 13    // 트리거 핀 선언
#define ech0 12    // 에코 핀 선언
#define tri1 11    // 트리거 핀 선언
#define ech1 10    // 에코 핀 선언
#define tri2 9    // 트리거 핀 선언
#define ech2 8    // 에코 핀 선언
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


int SoilWaterPin = A0; //토양수분센서 pin번호
int cdsPin = A1;  //CDS pin번호

int soilwater = 0; //센서의 값을 받아 옴
int cds = 0;

String cmd = "";

unsigned long previousMillis = 0; //이전시간
const long delayTime = 1000; //1초(1000) 대기시간

WiFiEspClient client;
PubSubClient mqttClient(client);

SoftwareSerial ESP_wifi(BT_RXD, BT_TXD); 

int status = WL_IDLE_STATUS;
/*
void requestEvent() {
  if(cmd != ""){
    Wire.write((char* )cmd.c_str()); // respond with message of 6 bytes
    cmd = "";
  }
  // as expected by master
}
*/

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("아두이노")) {         //MQTT서버에 "아두이노"라는 ClientID로 접속
      Serial.println("connected"); 
      mqttClient.subscribe("cmd");             //"Topic"이라는 Topic을 Subscribe
      mqttClient.subscribe("getDis");
     }
     else {                                     //연결 실패시 다시 접속
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());         //MQTT 상태코드를 Serial 모니터에 출력
      Serial.println(" try again in 5 seconds");
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic, "cmd") == 0){
    String msg = "";
    for (int i = 0; i < length; i++) {
      msg +=(char)payload[i];
    }
    //cmd = msg;
    Serial.println(msg);
    Wire.beginTransmission(8);
    Wire.write((char* )msg.c_str());
    Wire.endTransmission();
  }
  else if(strcmp(topic, "getDis") == 0){
    
    long dur0, dis0, dur1, dis1, dur2, dis2;    // 거리 측정을 위한 변수 선언
     // 트리거 핀으로 10us 동안 펄스 출력
     
     digitalWrite(tri0,HIGH);
     delayMicroseconds(10);
     digitalWrite(tri0,LOW);
     dis0=pulseIn(ech0,HIGH)/58.2;
     delayMicroseconds(2);
     
     digitalWrite(tri1,HIGH);
     delayMicroseconds(10);
     digitalWrite(tri1,LOW);
     dis1 =pulseIn(ech1,HIGH)/58.2;
     delayMicroseconds(2);

     digitalWrite(tri2,HIGH);
     delayMicroseconds(10);
     digitalWrite(tri2,LOW);
     dis2 =pulseIn(ech2,HIGH)/58.2;
     delayMicroseconds(2);

     String distance = String(dis0) + "|" + String(dis1) + "|" + String(dis2);
     mqttClient.publish("Dis", (char* )distance.c_str());
     Serial.println("Dis/published");
  }

}

// 데이터 전송
void setup() {
  Wire.begin();
  //Wire.begin(8);        // join i2c bus (address optional for master)
  //Wire.onRequest(requestEvent);
  Serial.begin(9600);  // start serial for output
  ESP_wifi.begin(9600);
  dht.begin();
  pinMode(tri0, OUTPUT);    // 트리거 핀 출력으로 선언
  pinMode(ech0, INPUT);     // 에코 핀 입력으로 선언
  pinMode(tri1, OUTPUT);    // 트리거 핀 출력으로 선언
  pinMode(ech1, INPUT);     // 에코 핀 입력으로 선언
  pinMode(tri2, OUTPUT);    // 트리거 핀 출력으로 선언
  pinMode(ech2, INPUT);     // 에코 핀 입력으로 선언
  
  WiFi.init(&ESP_wifi);               // esp8266 초기화
  while(status != WL_CONNECTED){
    Serial.print("Attempting to connect to WPA SSID:");
    Serial.print(ssid);
    status = WiFi.begin(ssid, pass);  // 입력한 ssid, pass로 wifi 접속
  }
  Serial.println("You're connected to the network");
  mqttClient.setServer(server, 1883); // MQTT 서버 연결
  mqttClient.setCallback(callback);   // 데이터가 수신되었을 때의 callback 등록
}
 
void loop() {
  if (!mqttClient.connected()) {
      reconnect();
    }
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= delayTime){ //1000초 시간이 흘렀는지 체크
    previousMillis = currentMillis;
    
    int t = dht.readTemperature();
    int h = dht.readHumidity();
    
    soilwater = map(analogRead(SoilWaterPin), 0, 1023, 0,100);
    cds = map(analogRead(cdsPin) , 0, 1024, 0,100);
    
    String sensor = String(t) + "|" + String(h) + "|" + String(soilwater) + "|" + String(cds);
    Serial.println(sensor);
    mqttClient.publish("Now", (char* )sensor.c_str());
  }
  mqttClient.loop();
 
}
