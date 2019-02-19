#include <SoftwareSerial.h>
#include<dht.h>

#define RX 10
#define TX 11
#define DHT11_PIN 3

String AP = "bhageerathi";
String PASS = "password";
String API = "GFVFAICLLTXZ5WP9";
String HOST = "184.106.153.149";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
float valSensor1;
float valSensor2;
int countTrueCommand = 1;
int countTimeCommand = 0;
boolean found = false; 

dht DHT;

unsigned long seconds = 1000L;
unsigned long minutes = seconds * 60;
unsigned long hours = minutes * 60; 

SoftwareSerial esp8266(RX,TX); 

void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  countTrueCommand = 0;
}

void loop() { 
  getSensorData();
  String getData = "GET /update?api_key="+ API +"&field1="+String(valSensor1)+"&field2="+String(valSensor2);
  switch(countTrueCommand) {
    case 0: sendCommand("AT",5,"OK");break;
    case 1: sendCommand("AT+CIPMUX=1",5,"OK"); break;
    case 2: sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK"); break;
    case 3: sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">"); break;
    case 4: esp8266.println(getData);delay(1500);countTrueCommand++; break;
    case 5: sendCommand("AT+CIPCLOSE=0",5,"OK"); break;
    case 6: 
            Serial.println(valSensor1);
            Serial.println(valSensor2);
            Serial.print(getData);
            Serial.print(",");
            Serial.println(getData.length());
            countTrueCommand = 0;
            delay(10000);
            break;
  }
}

void getSensorData(){
  int chk = DHT.read11(DHT11_PIN);
  valSensor1 = DHT.temperature;
  valSensor2 = DHT.humidity;
  return;
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
    countTimeCommand++;
  }  
  if(found == true)
  {
    Serial.println("Successfull");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
}
