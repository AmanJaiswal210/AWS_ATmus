#include <AWS_IOT.h>
#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>

AWS_IOT hornbill;
#define ss 5
#define rst 14  
#define dio0 2
String InputString;
String voltage;
String mot;
String Percent;
String Dir;

char WIFI_SSID[]="Tenda_7F50C8";
char WIFI_PASSWORD[]="Vansh002244@";
char HOST_ADDRESS[]="a1uu4gk0hkhn8-ats.iot.us-east-2.amazonaws.com";
char CLIENT_ID[]= "Atmus_example";
char TOPIC_NAME[]= "$aws/things/AtmusDemo/shadow/update";


int status = WL_IDLE_STATUS;
int tick=0,msgCount=0,msgReceived = 0;
char Payload[512];
static char rcvdPayload[512];

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
    strncpy(rcvdPayload,payLoad,payloadLen);
    rcvdPayload[payloadLen] = 0;
    msgReceived = 1;
}



void setup() {
    Serial.begin(115200);
    //delay(2000);

    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(WIFI_SSID);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        // wait 1 seconds for connection:
        delay(1000);
    }

    Serial.println("Connected to wifi");

    if(hornbill.connect(HOST_ADDRESS,CLIENT_ID)== 0)
    {
        Serial.println("Connected to AWS");
        delay(500);

        if(0==hornbill.subscribe(TOPIC_NAME,mySubCallBackHandler))
        {
            Serial.println("Subscribe Successfull");
        }
        else
        {
            Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
            while(1);
        }
    }
    else
    {
        Serial.println("AWS connection failed, Check the HOST Address");
        while(1);
    }
  Serial.println("LoRa Sender");
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) { 
    Serial.println("Starting LoRa failed!");
    while (1);
  }
    //delay(2000);

}

void loop() {
   if(msgReceived == 1)
    {  
      msgReceived=0;
       
        //LoRa.beginPacket();
        //LoRa.print(rcvdPayload);
        //LoRa.endPacket();
        Serial.print("Message from AWS:");
        Serial.println(rcvdPayload); 
        awsReceived();

    }  
 
  while (Serial.available()) {
    InputString = Serial.readString();
    LoRa.beginPacket();
    LoRa.print(InputString);
    LoRa.endPacket();
    Serial.print("Lora Send : ");
    Serial.println(InputString);
  }
   getLoRaData();
}

void getLoRaData() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet \n");
    while (LoRa.available()) {
     // Serial.print((char)LoRa.read());
      String LoRaData = LoRa.readString();
      //Serial.print(LoRaData); 
    
    
    int pos1 = LoRaData.indexOf('/');
    int pos2 = LoRaData.indexOf('&');
    int pos3 = LoRaData.indexOf('$');
    int pos4 = LoRaData.indexOf('#');
    
    voltage = LoRaData.substring(0, pos1);
    mot = LoRaData.substring(pos1 +1, pos2);
    Percent = LoRaData.substring(pos2 +1, pos3);
    Dir = LoRaData.substring(pos3 +1, pos4);
    
    Serial.print("voltage = ");
    Serial.println(voltage); 
    Serial.print("motor = ");
    Serial.println(mot);
    Serial.print("Battery_Percentage = ");
    Serial.println(Percent);
    Serial.print("Dirction = ");
    Serial.println(Dir);       
    
    //create string payload for publishing
    String Atmus = "Voltage:";
    Atmus += String(voltage);
    Atmus += " motor:";
    Atmus += String(mot);
    Atmus += " Batt_Percentage:";
    Atmus += String(Percent);
    Atmus += " Direction:";
    Atmus += String(Dir);        

     //char payload[100];
    Atmus.toCharArray(Payload, 100);

    //Serial.println("Publishing:- ");
    //Serial.println(payload);
     if(hornbill.publish(TOPIC_NAME, Payload) == 0){// publishes payload and returns 0 upon success
      Serial.println("Success\n");
    }
    else{
      Serial.println("Failed!\n");
    } 
   }
 }
}

void awsReceived(){
  if(rcvdPayload=="on"){
     Serial.println("TEST");
     LoRa.beginPacket();
     LoRa.print(rcvdPayload);
     LoRa.endPacket();
     }
  }

