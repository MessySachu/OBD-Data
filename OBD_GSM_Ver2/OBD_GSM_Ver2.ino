#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <Canbus.h>
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

const int chipSelect = 9;
//Declare SD File
File dataFile;

char *EngineRPM;
char buffer[64];

SoftwareSerial gprsSerial(10,11);

//#define gprsSerial Serial

#define TRUE 1
#define FALSE 0

String OBD_to_GSM;

boolean restart_Bearer_Setting() 
{   
   gprsSerial.println(F("AT+SAPBR=0,1"));   
   gprsSerial.println(F("AT+SAPBR=1,1"));
   gprsSerial.println(F("AT+SAPBR=2,1"));
}

boolean restart_gprs(int restart)
{
  int restart_gsm_ret_val = 0;
  int reset_bearer_val = 0;
    
  if (restart)
  {
    do {
       restart_gsm_ret_val = restart_gsm();
    }while (restart_gsm_ret_val != 1);
  }
   //Serial.println("HI");
   gprsSerial.println(F("AT+CGATT=1")); //Attach or detach GPRS service
   delay(100);
   toSerial();
   gprsSerial.println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"")); // bearer settings
   gprsSerial.println(F("AT+SAPBR=3,1,\"APN\",\"xxxx\""));   // bearer settings
    gprsSerial.println(F("AT+SAPBR=1,1"));  // bearer settings
    if( check_AT_OK())
    {
      Serial.println(F("AT+SAPBR 1,1, apn OK"));
      gprsSerial.println(F("AT+SAPBR=2,1"));
      if( gprsSerial.find("0.0.0.0") )
      {
       Serial.println(F("AT+SAPBR=2,1 0.0.0.0 Error"));
       do 
       {
         reset_bearer_val = restart_Bearer_Setting(); 
       } while (reset_bearer_val != 1);      
      }
      else
      {
       Serial.println(F("AT+SAPBR=2,1 Valid IP")); 
      }
    }
    else
    {
     Serial.println(F("AT+SAPBR 1,1 apn ERROR"));
     do {
       reset_bearer_val = restart_Bearer_Setting(); 
     } while (reset_bearer_val != 1);
    }
        
    return TRUE;
}

void setup()
{
   int init_gsm_value= 0;
   int init_gprs_val = 0;
   int gprs_attach_fail_cntr = 0;
   int cntr=0;
   
  pinMode(chipSelect, OUTPUT);
  if(Canbus.init(CANSPEED_500))  /* Initialize MCP2515 CAN controller at the specified speed */
  {
    Serial.println(F("CAN Init Ok"));
    delay(1500);
  } 
  else
  {
    Serial.println(F("Can't init CAN"));
    return;
  } 
  if (!SD.begin(chipSelect)) {
    Serial.println(F("uSD card failed to initialize, or is not present"));
    return;
  }
  else{
      Serial.println(F("uSD card initialized."));
      delay(1500);
  } 

  if (! rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  
   delay(10000);
   gprsSerial.begin(9600);
   Serial.begin(115200);
  
  delay(1000);
   do
   {
     init_gsm_value = Init_gsm_modem();
   }while(init_gsm_value!=1);
   
   gprsSerial.flush();
   Serial.flush();
   
   
   gprsSerial.println(F("AT+CSQ"));
   delay(200);
   toSerial();
   if( check_good_CSQ())
   {
    //Check later, if we need to do something
   }
   else
   {
    //Check later, if we need to do something
   }

   gprsSerial.println(F("AT+CMEE=2"));

   do {
     init_gprs_val = restart_gprs(0);
     if (1 != init_gprs_val)
       gprs_attach_fail_cntr++;
     if(5 == gprs_attach_fail_cntr)
     {
      gprs_attach_fail_cntr = 0;
      init_gprs_val = restart_gprs(1);
     }
   } while (init_gprs_val != 1);
}

boolean check_good_CSQ()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
    
  }
  if( (gprsSerial.find(",99")) || (gprsSerial.find(",7")) || (gprsSerial.find(",6")) || (gprsSerial.find(",5"))|| 
      (gprsSerial.find("0,")) || (gprsSerial.find("1,")) || (gprsSerial.find("2,")) ||
      (gprsSerial.find("3,")) || (gprsSerial.find("4,")) || (gprsSerial.find("5,")) ||
      (gprsSerial.find("6,")) || (gprsSerial.find("7,")) || (gprsSerial.find("8,")) || (gprsSerial.find("9,")) || (gprsSerial.find("10,"))
    ) 
  {
    Serial.println(F("RSSI and/or BER are very good"));
    return FALSE;
  }
  else
  {
    Serial.println(F("RSSI and BER good"));
    return TRUE;
  }
}

boolean check_AT_OK()
{
  char ch;
  while(gprsSerial.available()!=0)
  {
    ch = gprsSerial.read();
    Serial.write(ch);
  }
  if(gprsSerial.find("OK"))
    return TRUE;
  else
    return FALSE;
}

static boolean Init_gsm_modem()
{
  gprsSerial.println(F("AT"));
  delay(100);
  gprsSerial.println(F("ATE0"));
  delay(100);
  gprsSerial.println(F("AT+CMGF=1"));
  delay(100);
  gprsSerial.println(F("AT+CNMI=2,2,0,0,0"));
  delay(100);
  return TRUE;
}

boolean check_gprs_status_and_reinit()
{
  int init_gprs_val = 0;
  
  gprsSerial.println(F("AT+CGATT?"));
  if( Serial.find("1") )
  {
    Serial.println(F("GPRS Attached in status_and_reinit"));
  }
  else
  {
   Serial.println(F("GPRS not attached. Error"));
   do {
     init_gprs_val = restart_gprs(0);
   } while (init_gprs_val != 1);
  }
  return TRUE; 
}

bool http_init()
{
  gprsSerial.flush();
   Serial.flush();
  gprsSerial.println(F("AT+HTTPINIT"));  // initialize http service
  delay(100);
  gprsSerial.println(F("AT+HTTPPARA=\"CID\",1"));
  delay(100);
  return TRUE;
}

boolean http_post_url()
{
  String FixedHTTP;
//gprsSerial.print("AT+HTTPPARA=\"URL\",\"http://kush.esy.es/vtur/updateDriverLocationAdmin.php?subscriptionkey=Deba&regdno=1234&lat=1729.22&long=7780.22&x_axis=0&y_axis=0&speed=10&alcohol=0&UUID=1122334455667788990"); // set http param value
  
//  String FixedHTTP = "AT+HTTPPARA=\"URL\",\"http://kush.esy.es/OBD/send_data.php?UUID=88007788&param1=12345&";
//  FixedHTTP += "Acc=6&EngLoad=0&SPEED=0&RPM=0&Run_Time=0&Distance=0";
//  FixedHTTP = "AT+HTTPPARA=\"URL\",\"http://kush.esy.es/vtur/updateDriverLocationAdmin.php?subscriptionkey=Deba&regdno=1234&";
  FixedHTTP = "AT+HTTPPARA=\"URL\",\"http://kush.esy.es/OBD/send_data.php?UUID=88007788&param1=";
 //FixedHTTP += "OBD_to_GSM";
//  FixedHTTP += PackageHTTPData();
  FixedHTTP += PackageHTTPData2();
  Serial.print(FixedHTTP);
  gprsSerial.print(FixedHTTP);
  
  delay(2000);
  gprsSerial.print(F("\"\r\n"));
  delay(100);
  toSerial();
  gprsSerial.print(0x1A);
  delay(2000);
  Serial.print(F("at_httppara"));
  gprsSerial.println(F("AT+HTTPACTION=0")); //set http action type 0 = GET, 1 = POST, 2 = HEAD
  delay(500);
  toSerial();
  Serial.println(F("AT_httpaction"));
  gprsSerial.println(F("AT+HTTPTERM"));
  return TRUE;
}

void http_post()
{
 int http_post_url_val = 0;

  http_init();  
  
  do {
    http_post_url_val = http_post_url();
  } while (http_post_url_val != 1);
}

void toSerial()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
    
  }
}

boolean restart_gsm()
{
  int init_gsm_value = 0;
  
  Serial.println(F("We are Reset"));
  gprsSerial.println(F("AT+CFUN=1,1"));
  toSerial();

  delay(2000);
  
    do
    {
      init_gsm_value = Init_gsm_modem();
    } while(init_gsm_value!=1);

  return TRUE;
}

void loop()
{
  OBD_to_GSM = PackageHTTPData2();
  http_post();
}

String ReturnRTCTime(){
  String RTCTime;
  DateTime now = rtc.now();
  RTCTime += String(now.year());
  RTCTime += ":";
  RTCTime += String(now.month());
  RTCTime += ":";
  RTCTime += String(now.day());
  RTCTime += ":";
  RTCTime += String(now.hour());
  RTCTime += ":";
  RTCTime += String(now.minute());
  RTCTime += ":";
  RTCTime += String(now.second());

  return RTCTime;
}

String PackageHTTPData2(){
  String CANOBDData;

  Canbus.ecu_req(PID_RPM,buffer); //Request engine RPM
  EngineRPM = buffer;
  Serial.print(F("Engine RPM: ")); //Uncomment for Serial debugging
  CANOBDData += String(buffer);
  CANOBDData += ",";
  Serial.print(buffer);

  Canbus.ecu_req(PID_COOLANT_TEMP,buffer);
  Serial.print(F("    Coolant Temp: "));
  CANOBDData += String(buffer);
  CANOBDData += ",";
  Serial.print(buffer);

  Canbus.ecu_req(PID_SPEED,buffer);
  Serial.print(F("    Speed: "));
  CANOBDData += String(buffer);
  CANOBDData += ",";
  Serial.print(buffer);

  Canbus.ecu_req(PID_MAF_FLOW,buffer);
  Serial.print(F("    MAF Sensor: "));
  CANOBDData += String(buffer);
  CANOBDData += ",";
  Serial.print(buffer);

  Canbus.ecu_req(PID_BATTERY_VOLTAGE,buffer);
  Serial.print(F("    Voltage:  "));
  CANOBDData += String(buffer);
  CANOBDData += ",";
  Serial.print(buffer);
  
  Canbus.ecu_req(PID_THROTTLE,buffer);
  Serial.print(F("    Throttle:  "));
  CANOBDData += String(buffer);
  Serial.print(buffer);

  CANOBDData += ",";

  CANOBDData += ReturnRTCTime();

  Serial.println();
  return CANOBDData;
}
