#include <ArduinoJson.h>
#include<Servo.h>
Servo myservo;
Servo myservo2;
int trig = 2;
int echo = 3;

#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// 모바일 HostSpot의 ID와 패스워드 (변경할 부분)
const String SSID = "kyho";
const String PASSWORD = "123456789";
// 서버주소와 포트 (변경할 부분)
const String SERVER_IP = "192.168.137.1";
const String SERVER_PORT = "8081";

// AT 명령 저장
String cmd = "";
// 전송 데이터 저장
String sendData = "";
// WiFi 접속 실패 여부
boolean FAIL_8266 = false;

//받는 객체 담기
String test = "";

// TX, RX
//SoftwareSerial serial1(19, 18);

void setup() {
  //모터 선언
  myservo.attach(8); 
  myservo2.attach(9);   
   //초음파 센서 선언
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(11, OUTPUT);
 


  
  Serial.println("Start module connection");
  do {
    Serial.begin(9600);
    Serial1.begin(9600);
    // Serial18266 모듈 재시작
    Serial1.println("AT+RST");
    delay(1000);
    // 만약 재시작되었다면
    if (Serial1.find("ready")) {
      Serial.println("Module ready");
      // Serial18266 모듈의 모드를 듀얼모드로 설정 (클라이언트)
      Serial1.println("AT+CWMODE=1");
      delay(500);
      // AP에 접속되면
      if (cwJoinAP()) {
        Serial.println("AP successful");
        FAIL_8266 = false;
        delay(5000);
        Serial.println("Start buffer initialization");
        while (Serial1.available() > 0) {
          char a = Serial1.read(); //out 프린트 담는다
        }
        Serial.println();
        Serial.println("Buffer initialization terminated");
      } else {
        Serial.println("AP connection failure");
        delay(500);
        FAIL_8266 = true;
      }
    } else {
      Serial.println("Module connection failure");
      delay(500);
      FAIL_8266 = true;
    }
  } while (FAIL_8266);
  Serial.println("Module connection complete");
}
boolean a = true;
String message = "";


void loop() {
  message = "rSerial1onsedata";
 
  if (a) {
    //json값 초기화 (test)
    test = "";
    
    sendDataToServer("data" + message); //이쪽에서 맨 밑에로 넘어간다.

    //조도값에 따른 led 밝기 
     int sensorValue = analogRead(A0);
    delay(1);
    Serial.println(sensorValue);
  
    if(sensorValue < 400) {
      digitalWrite(11,HIGH);
    
    } else {
     digitalWrite(11,LOW);

    }
      
    
    //초음파 값 계산
    digitalWrite(trig, HIGH);

    delay(3);

    digitalWrite(trig, LOW);

    int duration = pulseIn(echo, HIGH);
    int distance = duration/2/29;  
    Serial.println(distance);

      
    Serial.println(test);
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(test);
    int t1= root["msr_use"];
    int t2= root["msr_use2"];
    
    Serial.println(t1);
    Serial.println(t2);

    
    //첫번째 모터
    if(t1 == 0 && distance > 15) {
      myservo.write(80);
      delay(10);
    } else if (t1 == 1  ){
      myservo.write(0);
      delay(10);
    } else {  
    }

    //두번째 모터
    if(t2 == 0 && distance > 15) {
      myservo2.write(80);
      delay(10);
    } else if (t2 == 1  ){
      myservo2.write(0);
      delay(10);
    } else {  
    }
        
   }
}

void sendDataToServer(String data) {
    Serial.println("Start the data transfer part");
    cmd = "AT+CIPSTART=\"TCP\",\"" + SERVER_IP + "\"," + SERVER_PORT + "\r\n";
    Serial.println("Attempt to connect to server");
    Serial1.println(cmd);
    // 웹 서버에 접속되면
    if (Serial1.find("OK")) {
      Serial.println("Server connection successful");
    } else {
      Serial.println("Server connection failed");
    }
    // 서버로 GET 메시지 전송
    cmd = "GET /smartcar/AduinoCheck";
    //  cmd += data;
    cmd += "\r\nConnection: close\r\n\r\n";
    Serial.println(cmd);
    Serial1.print("AT+CIPSEND=");
    Serial1.println(cmd.length());
    if (Serial1.find("OK")) {
      Serial.println("Ready to send to server");
    } else {
      Serial.println("Failed to prepare to send to server");
    }
    Serial1.println(cmd);
  
    //데이터 전송이 완료되면
    if (Serial1.find("OK")) {
      Serial.println("Data transfer successful");
      Serial.println();
      delay(5000);
      Serial.println("Attempt to receive data");
      int state = false;
      while (Serial1.available()) {
        char c = Serial1.read();
        
        Serial.write(c);
        if (c == '/') {
          state = !state;
        } else if (state) {
          test += (String)c;
        }
  
      }
      Serial.println();
      Serial.println("End Receiving Data");
    } else {
  
      Serial.println("Data transfer failed");
      sendDataToServer(data);
    }
    delay(1000);
  }

boolean cwJoinAP() {
    String cmd = "AT+CWJAP=\"" + SSID + "\",\"" + PASSWORD + "\"";
    Serial1.println(cmd);
    if (Serial1.find("OK")) {
      return true;
    } else {
      return false;
    }
}
