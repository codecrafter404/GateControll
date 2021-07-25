# GateControll
 Simple Programm to open a Gate using Bluetooth and the ESP32
 
 # Example Json Payloads and Response
 ## Open Gate Request
```
{
  "action": "open",
  "token": "Defined Token"
}
```
## Response
```
{
  "succed": "true",
  "data": "If isn't succed, then here is the Error message..."
}
```
# Configurable Options in main.cpp
| Option | Default | Datatype | Description |
| --- | --- | --- | --- |
| led_pin | 2 | int | The Output pin on your ESP32 |
| gateLockDelay | 30 | int | The while the Gate is opening (dont accept Request), in Seconds |
| gateToggleDelay | 10000 | int | The blink time (Between on/off) |
| espName | ESP32 by Codecrafter | string | The Beacon name |
| token | 1234567890 | string | The security Token |
