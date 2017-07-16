/*************************************************************************
* Testing sketch for Freematics OBD-II UART Adapter
* Reads and prints several OBD-II PIDs value
* Distributed under GPL v2.0
* Visit http://freematics.com for more information
* Written by Stanley Huang <support@freematics.com.au>
*************************************************************************/

#include <SoftwareSerial.h>
#include <OBD2UART.h>

// On Arduino Leonardo, Micro, MEGA or DUE, hardware serial can be used for output
// as OBD-II UART adapter uses to Serial1
// On Arduino UNO and those have no Serial1, we use software serial for output
// as OBD-II UART adapter uses to Serial
SoftwareSerial mySerial(A2, A3);    //Connect OBD Here
//#define mySerial Serial

COBD obd;

String readPIDSingle1()   //ThrottlePosition
{
    int value;
    if (obd.readPID(PID_THROTTLE, value)) {
      return String(value);
    }
    else
      return "*";
}

String readPIDSingle2()   //Engine Load
{
    int value;
    if (obd.readPID(PID_ENGINE_LOAD, value)) {
      return String(value);
    }
    else
      return "*";
}

String readPIDSingle3()   //Speed
{
    int value;
    if (obd.readPID(PID_SPEED, value)) {
      return String(value);
    }
    else
      return "*";
}

String readPIDSingle4()   //Engine RPM
{
    int value;
    if (obd.readPID(PID_RPM, value)) {
      return String(value);
    }
    else
      return "*";
}

String readPIDSingle5()   //Engine run time
{
    int value;
    if (obd.readPID(PID_RUNTIME, value)) {
      return String(value);
    }
    else
      return "*";
}

String readPIDSingle6()   //Distance Travelled (Didnt work)
{
    int value;
    if (obd.readPID(PID_DISTANCE_WITH_MIL, value)) {
      return String(value);
    }
    else
      return "*";
}

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
  String TempValue;
  TempValue += readPIDSingle1();
  TempValue += ",";
  TempValue += readPIDSingle2();
  TempValue += ",";
  TempValue += readPIDSingle3();
  TempValue += ",";
  TempValue += readPIDSingle4();
  TempValue += ",";
  TempValue += readPIDSingle5();
  TempValue += ",";
  TempValue += readPIDSingle6();
  TempValue += ",";
  return TempValue;
}

String PackageHTTPData1(){
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
//  Serial.println(PackageHTTPData());
  Serial.println(PackageHTTPData1());
}

