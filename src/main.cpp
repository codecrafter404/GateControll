#include <Arduino.h>
#include <string.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>

int led_pin = 2;
String espName = "ESP32 by Codecrafter";

String token = "1234567890";

BluetoothSerial bt;
boolean open = false;

struct request
{
  const char* action;
  const char* token;
  const char* parseError;
};

struct response
{
  boolean succed;
  const char* data;
};


String generateResponse(response res){
  DynamicJsonDocument doc(1024);
  doc["succed"] = res.succed;
  doc["data"] = res.data;
  String result = "";
  serializeJson(doc, result);
  return result;
}


request parseJson(String json){
  /*
    Sample Input:
    {
      "action": "someaction",
      "token": "sometoken"
    }
  */
  request result;
  
  DynamicJsonDocument jsondocument(1024);
  DeserializationError error = deserializeJson(jsondocument, json.c_str());
  
  if(error){

    result.parseError = (char*) error.f_str();
    return result;
  }
  result.action = jsondocument["action"];
  result.token = jsondocument["token"];
  return result;
}



void setup() {
  Serial.begin(9600);
  Serial.print("Beginn initalization.\n");
  // Init LED
  pinMode(led_pin, OUTPUT);
  Serial.println("Setting up BT");

  bt.begin(espName);
  Serial.print("Initalization finished.\n");
}

void loop() {

  if(bt.available()){
    String read = bt.readString(); 

  }
}




/*
 -->
{
  "action": "",
  "token": ""
}

 <--
 {
   "succed": "true",
   "data": ""
 }

*/
