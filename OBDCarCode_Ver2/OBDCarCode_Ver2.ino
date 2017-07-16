#include <SoftwareSerial.h>
#include <OBD2UART.h>

SoftwareSerial mySerial(A2, A3);    //Connect OBD Here
//#define mySerial Serial

COBD obd;

void readBatteryVoltage()
{
  mySerial.print('[');
  mySerial.print(millis());
  mySerial.print(']');
  mySerial.print("Battery:");
  return (obd.getVoltage(), 1);
  mySerial.println('V');
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  
  // this will begin serial
  byte version = obd.begin();

  mySerial.print("OBD-II Connected ");
  if (version > 0) {
    Serial.print("Ver. ");
    Serial.print(version / 10);
    Serial.print('.');
    Serial.println(version % 10);
  } 
  else {
    Serial.println("NOT Connected");
//    for (;;);
  }
  delay(1000);
}


String PackageHTTPData(){
  String TempValue = "";
  int value;
  if (obd.readPID(PID_THROTTLE, value)) {
    TempValue += String(value);
    TempValue += ",";
  }
  else
    TempValue += "*,";
  if (obd.readPID(PID_ENGINE_LOAD, value)) {
    TempValue += String(value);
    TempValue += ",";
  }
  else
    TempValue += "*,";
  if (obd.readPID(PID_SPEED, value)) {
    TempValue += String(value);
    TempValue += ",";
  }
  else
    TempValue += "*,";
  if (obd.readPID(PID_RPM, value)) {
    TempValue += String(value);
    TempValue += ",";
  }
  else
    TempValue += "*,";
  if (obd.readPID(PID_RUNTIME, value)) {
    TempValue += String(value);
    TempValue += ",";
  }
  else
    TempValue += "*,";
  if (obd.readPID(PID_DISTANCE, value)) {
    TempValue += String(value);
    TempValue += ",";
  }
  else
    TempValue += "*,";
  return TempValue;
}

void loop()
{
  readBatteryVoltage();
  Serial.println(PackageHTTPData());
}

