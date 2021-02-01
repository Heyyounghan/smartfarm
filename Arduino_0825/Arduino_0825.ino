#include <ezTask.h>
#include <AccelStepper.h>
#include <SoftwareSerial.h>
#include <WiFiEsp.h>

#define HALFSTEP 8  //Half-step mode (8 step control signal sequence)

// Motor pin definitions
#define mtrPin1  10     // IN1 on the ULN2003 driver 1
#define mtrPin2  11     // IN2 on the ULN2003 driver 1
#define mtrPin3  12     // IN3 on the ULN2003 driver 1
#define mtrPin4  13     // IN4 on the ULN2003 driver 1

#define mtrPin_1  6     // IN1 on the ULN2003 driver 22
#define mtrPin_2  7     // IN2 on the ULN2003 driver 23
#define mtrPin_3  8     // IN3 on the ULN2003 driver 24
#define mtrPin_4  9    // IN4 on the ULN2003 driver 25

// 모터드라이버
int motorA1 = A0; //모터드라이브 in1 변수선언 6 번 포트로 제어 (out1) 정회전
int motorA2 = A1; //모터드라이브 in2 변수선언 7 번 포트로 제어 (out2) 역회전

//모터 A enable, PWM 제어 (ENA) VCC 랑 연결해주세요. 

////////// 환풍기 변수 선언 ////////////////

int fanB1 = A2; // 모터드라이브 in3 변수선언 8번 포트로 제어 (out1) 정회전
int fanB2 = A3; // 모터드라이브 in4 변수선언 9번 포트로 제어 (out2) 역회전
int ENA = A4;

// WiFi 모듈
#define BT_RXD 2
#define BT_TXD 3

AccelStepper stepper1(HALFSTEP, mtrPin1, mtrPin3, mtrPin2, mtrPin4);
AccelStepper stepper2(HALFSTEP, mtrPin_1, mtrPin_3, mtrPin_2, mtrPin_4);
SoftwareSerial ESP_wifi(BT_RXD, BT_TXD);

int leftWheelForeward = 1;
int leftWheelBackward = -1;
int rightWheelForeward = -1;
int rightWheelBackward = 1;

unsigned long previousMillis = 0; //이전시간
const long delayTime = 1000; //1초(1000) 대기시간

char ssid[] = "U+Net7828";           //  와이파이에 접속할 SSID
char pass[] = "DD6C015249";             //  와이파이의 비밀번호
const char* server = "192.168.219.101"; //  서버의 
const char* cmd = "/Cmd";

String code = "";
bool flag_stop;

int i = 0;
int stepCnt = 360; //스텝이 360도 회전하기 위해 필요한 스텝 수
int stepDelay= 5; 



int status = WL_IDLE_STATUS;


void parseData(String line){
  String tmp_str = "";

  int code_data = line.indexOf("]");
  if (code_data >= 0){
    tmp_str = "[";
    code = line.substring(line.indexOf(tmp_str) + tmp_str.length(), code_data);
  }
  flag_stop = 1;
  //char* copy_str = str.c_str();
  //int count = 2;
  //boolean chk = false;
  //for(int j = str.length() - 1; i > 0; i--){
  //  Serial.println(copy_str[j]);
  //  if (copy_str == "["){
  //   break;
  //  }
    /*
    if(copy_str[j] == "" || count < 0){
      break;
    }
    else{
      code[count] = copy_str[j];
      count--;
    }
    
    if (copy_str[j] == "]" && copy_str[j - 1] == "["){
      break;
    }
    else if(copy_str[j] == "]"){
      chk = true;
      continue;
    }
    if (chk && count >= 0){
      code[count] = copy_str[j];
      count--;
    }
  }
  */
  
}
void connection(){
  WiFiEspClient client;
  client.setTimeout(5000);
  client.flush();
  
  if (client.connect(server, 3000)) {
    String redirect = "";
    client.print(String("GET ") + cmd + " HTTP/1.1\r\n" +
    "Host: " + server + "\r\n" +
    "Connection: close\r\n\r\n");
    Serial.println("Cmd");

    while(client.available()){
      char c = client.read();
      redirect += c;
    }
    parseData(redirect);
    client.stop();
    
  }
}

void setup() {
  Serial.begin(9600);
  
  pinMode(motorA1, OUTPUT); //모터드라이버 워터펌프 in1
  pinMode(motorA2, OUTPUT); // 모터드라이버 워터펌프 in2 
  pinMode(fanB1, OUTPUT); //모터드라이버 환풍기 in3
  pinMode(fanB2, OUTPUT); //모터드라이버 환풍기 in4
  pinMode(ENA, OUTPUT);

  ESP_wifi.begin(9600);
  
  stepper1.setMaxSpeed(2000.0);
  stepper2.setMaxSpeed(2000.0);
  
  WiFi.init(&ESP_wifi);               // esp8266 초기화
  while(status != WL_CONNECTED){
    Serial.print("Attempting to connect to WPA SSID:");
    Serial.print(ssid);
    status = WiFi.begin(ssid, pass);  // 입력한 ssid, pass로 wifi 접속
  }
  Serial.println("You're connected to the network");
  
}

void _Go() {  //foreward
   for (int k=0; k<stepCnt; k++) {
    stepper1.move(leftWheelForeward); //등속함수는 move , 동작
    stepper2.move(rightWheelForeward); // 
    stepper1.setSpeed(500); // 속도, move 후에 setSpeed 호출  
    stepper2.setSpeed(500);
    stepper1.runSpeedToPosition(); // 감속은 run 등속은 runSpeedtoposition 사용 
    stepper2.runSpeedToPosition();
    delay(stepDelay);
    if (k == stepCnt - 1)
    {
      return;
    }
  }
  /*
  for (;;) {
    stepper1.move(leftWheelForeward);
    stepper2.move(rightWheelForeward);
    stepper1.setSpeed(500);
    stepper2.setSpeed(500);
    stepper1.runSpeedToPosition();
    stepper2.runSpeedToPosition();
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= delayTime){ //시간이 흘렀는지 체크
      previousMillis = currentMillis;
      return;
    }
    
  }
  */
    //delay(10);
    /*
    controlKey = Serial.read();
    if (controlKey == 's' || controlKey == 'a' || controlKey == 'd' || controlKey == '1' || controlKey == 'q' || controlKey == 'e')
    {
      return;
    }
    */

 
}

void _Back() { //Backward
  for (;;) {
    stepper1.move(leftWheelBackward);
    stepper2.move(rightWheelBackward);
    stepper1.setSpeed(500);
    stepper2.setSpeed(500);
    stepper1.runSpeedToPosition();
    stepper2.runSpeedToPosition();
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= delayTime){ //시간이 흘렀는지 체크
      previousMillis = currentMillis;
      return;
    }
  }
    //delay(10);
    //controlKey = Serial.read();
    /*
    if (controlKey == 'w' || controlKey == 'a' || controlKey == 'd' || controlKey == '1' || controlKey == 'q' || controlKey == 'e')
    {
      return;
    }
    */

}

void pumpstart(){
      digitalWrite(motorA1, HIGH);
      digitalWrite(motorA2, LOW); //앞으로 회전
      digitalWrite(ENA, 255);
//on/ off 만 가능하면 되기 때문에 순방향/ 역방향 회전은 하지 않음 
} //펌프 작동


void pumpstop(){ // 
  digitalWrite(motorA1, LOW); // A1, A2 둘다 Low면 멈춤
  digitalWrite(motorA2, LOW); // 동일 
  digitalWrite(ENA, 0);
  

} // 정지 
 
/////// 환풍기 ////////

void fanstart(){ // 환풍기 작동 
  digitalWrite(fanB1, HIGH); // 정방향 
  digitalWrite(fanB2, LOW);
  digitalWrite(ENA, 255);

}

void fanstop(){ //환풍기 멈춤
  digitalWrite(fanB1, HIGH);
  digitalWrite(fanB2, LOW); 
}



//ezTask를 통한 멀티 태스크
/*
ezTask Blind(1000,[](){
  if(code[0] == '1'){
    Serial.println("Go");
    _Go();
  }
  else if(code[0] == '0'){
    Serial.println("Back");
    _Back();
  }
  
});

ezTask Water(1000,[](){
  if(code[1] == '1'){
    Serial.println("Water_on");
    pumpstart();
  }
  else if(code[1] == '0'){
    Serial.println("Water_off");
    pumpstop();
  }
  
});

ezTask Fan(1000,[](){
  if(code[2] == '1'){
    Serial.println("Fan_start");
    fanstart();
  }
  else if(code[2] == '0'){
    Serial.println("Fan_stop");
    fanstop();
  }
  
});
*/
ezTask Connection(1000,connection);

void loop() {/*
  WiFiEspClient client;
  client.setTimeout(5000);
  client.flush();
  
  if (client.connect(server, 3000)) {
    String redirect = "";
    int lines_received = 0;

    client.print(String("GET ") + cmd + " HTTP/1.1\r\n" +
    "Host: " + server + "\r\n" +
    "Connection: close\r\n\r\n");
    Serial.println("Cmd");
  
    while(client.available()) {
      char c = client.read();
      redirect += c;
    }
    Serial.print("This is pased data : ");
    //Serial.println(redirect);
    Serial.println(code);
    parseData(redirect);
    client.stop();
  }*/
  connection();
  Serial.println(code);
  //Blind.run();
  //Water.run();
  //Fan.run();
  /*
  if(code[0] == '1'){
    Serial.println("Go");
    _Go();
  }
  else if(code[0] == '0'){
    Serial.println("Back");
    _Back();
  }
  if(code[1] == '1'){
    Serial.println("Water_on");
    pumpstart();
  }
  else if(code[1] == '0'){
    Serial.println("Water_off");
    pumpstop();
  }
  if(code[2] == '1'){
    Serial.println("Fan_start");
    fanstart();
  }
  else if(code[2] == '0'){
    Serial.println("Fan_stop");
    fanstop();
  }
  */
  /*
  switch (controlKey) {
    case 'w': //전진
      _Go();
      Serial.println(controlKey);
      //controlKey = '0';
      break;
    case 's': //후진
      _Back();
      Serial.println(controlKey);
      //controlKey = '0';
      break;
    default :
      controlKey = Serial.read();
      break;
  }
  */
  
}
