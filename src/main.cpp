#include <Arduino.h>
#include <string.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>

int led_pin = 2;
// In Seconds
int gateLockDelay = 30;
// In millis
int gateToggleDelay = 1000;

String espName = "ESP32 by Codecrafter";
String token = "1234567890";

BluetoothSerial bt;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile boolean isOpening = false;


// Unlock Gate
void IRAM_ATTR onUnlockGate() {
  portENTER_CRITICAL_ISR(&timerMux);
  isOpening = false;
  portEXIT_CRITICAL_ISR(&timerMux);
  timerAlarmDisable(timer);		// stop alarm
  timerDetachInterrupt(timer);	// detach interrupt
  timerEnd(timer);
}

void setupTimer(){
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onUnlockGate, true);
  timerAlarmWrite(timer, 1000000 * gateLockDelay, false);
}

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
  Serial.println("Opening gate!");
  portENTER_CRITICAL(&timerMux);
  isOpening = true;
  portEXIT_CRITICAL(&timerMux);
  
  setupTimer();
  timerAlarmEnable(timer);

  digitalWrite(led_pin, HIGH);
  delay(gateToggleDelay);
  digitalWrite(led_pin, LOW);
}


void setup() {
  Serial.begin(9600);
  Serial.print("Beginn initalization.\n");
  // Init LED
  pinMode(led_pin, OUTPUT);
  Serial.println("Setting up BT");

  bt.begin(espName);

  Serial.println("Setting up Timer");
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

        if(!isOpening){
          openGate();
          response res;
          res.succed = true;
          res.data = "";
          bt.println(generateResponse(res));
        }else{
          response res;
          res.succed = false;
          res.data = "GATE IS OPENING";
          bt.println(generateResponse(res));
        }

      }else{
        response res;
        res.succed = false;
        res.data = "INVALID TOKEN";
        bt.println(generateResponse(res));
      }
      return;
    }
    if(!strcmp(req.action, "state")){
      // Open gate
      if(!strcmp(req.token, token.c_str())){

        String state = "";
        if(isOpening){
          state = "OPENING";
        }else{
          state = "READY";
        }
        response res;
        res.succed = true;
        res.data = state.c_str();
        bt.println(generateResponse(res));

      }else{
        response res;
        res.succed = false;
        res.data = "INVALID TOKEN";
        bt.println(generateResponse(res));
      }
      return;
    }

    response res;
    res.succed = false;
    res.data = "INVALID ACTION";
    bt.println(generateResponse(res));
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
