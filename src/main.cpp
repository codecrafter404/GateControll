#include <Arduino.h>
#include <string.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>

String espName = "ESP32 by Codecrafter";
String token = "1234567890";

BluetoothSerial bt;
int led_pin = 2;

struct request
{
  const char* action = "";
  const char* token = "";
  const char* parseError = "";
};

struct response
{
  boolean succed;
  const char* data = "";
};


String generateResponse(response res){
  /* 
  {
   "succed": "true",
   "data": ""
  }
  */

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

void openGate(){
  digitalWrite(led_pin, HIGH);
  delay(1000);
  digitalWrite(led_pin, LOW);
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
    // Try to parse JSON
    request req;
    try{
      req = parseJson(read);
    }catch(const char* error){
      Serial.printf("[ERROR] %s", error);
      response res;
      res.succed = false;
      res.data = "INVALID JSON";
      bt.println(generateResponse(res));
      return;
    }

    // Error by parse?
    if(strcmp(req.parseError, "")){
      Serial.println("test1");
      Serial.printf("[ERROR] %s", req.parseError);

      response res;
      res.succed = false;
      res.data = "INVALID JSON";

      bt.println(generateResponse(res));
      return;
    }

    // React to Action
    if(!strcmp(req.action, "open")){
      // Open gate
      if(!strcmp(req.token, token.c_str())){
        openGate();
        response res;
        res.succed = true;
        res.data = "";
        bt.println(generateResponse(res));
      }else{
        response res;
        res.succed = false;
        res.data = "INVALID TOKEN";
        bt.println(generateResponse(res));
      }
    }
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
