#include <SoftwareSerial.h>
#include <OBD2UART.h>

//#define mySerial Serial

//Connect OBD to 

COBD obd;
SoftwareSerial gprsSerial(10,11);
#define TRUE 1
#define FALSE 0

String OBD_to_GSM;

boolean restart_Bearer_Setting() 
{   
   gprsSerial.println("AT+SAPBR=0,1");   
   
   gprsSerial.println("AT+SAPBR=1,1");
   
   gprsSerial.println("AT+SAPBR=2,1");
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
   gprsSerial.println("AT+CGATT=1"); //Attach or detach GPRS service
   delay(100);
   toSerial();
   gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); // bearer settings
   gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"xxxx\"");   // bearer settings
    gprsSerial.println("AT+SAPBR=1,1");  // bearer settings
    if( check_AT_OK())
    {
      Serial.println(F("AT+SAPBR 1,1, apn OK"));
      gprsSerial.println("AT+SAPBR=2,1");
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
   
   delay(10000);
   gprsSerial.begin(9600);
   Serial.begin(115200);
   while (!Serial);
  
  // this will begin serial
  byte version = obd.begin();

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
   do
   {
     init_gsm_value = Init_gsm_modem();
   }while(init_gsm_value!=1);
   
   gprsSerial.flush();
   Serial.flush();
   
   
   gprsSerial.println("AT+CSQ");
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

   gprsSerial.println("AT+CMEE=2");

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
  gprsSerial.println("AT");
  delay(100);
  gprsSerial.println("ATE0");
  delay(100);
  gprsSerial.println("AT+CMGF=1");
  delay(100);
  gprsSerial.println("AT+CNMI=2,2,0,0,0");
  delay(100);
  return TRUE;
}

boolean check_gprs_status_and_reinit()
{
  int init_gprs_val = 0;
  
  gprsSerial.println("AT+CGATT?");
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
  gprsSerial.println("AT+HTTPINIT");  // initialize http service
  delay(100);
  gprsSerial.println("AT+HTTPPARA=\"CID\",1");
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
  FixedHTTP += PackageHTTPData();
  Serial.print(FixedHTTP);
  gprsSerial.print(FixedHTTP);
  
  delay(2000);
  gprsSerial.print("\"\r\n");
  delay(100);
  toSerial();
  gprsSerial.print(0x1A);
  delay(2000);
  Serial.print("at_httppara");
  gprsSerial.println("AT+HTTPACTION=0"); //set http action type 0 = GET, 1 = POST, 2 = HEAD
  delay(500);
  toSerial();
  Serial.println("AT_httpaction");
  gprsSerial.println("AT+HTTPTERM");
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
  gprsSerial.println("AT+CFUN=1,1");
  toSerial();

  delay(2000);
  
    do
    {
      init_gsm_value = Init_gsm_modem();
    } while(init_gsm_value!=1);

  return TRUE;
}


String PackageHTTPData(){ //Acc=1729.22&Speed=7780.22&RPM=0&Temp=0&Time=10&Dist=0&Dummy=1122334455667788990";
  String TempValue = "";
  int value;
//  TempValue += "Acc=";
  if (obd.readPID(PID_THROTTLE, value)) {
    TempValue += String(value);
    TempValue += ",";
  }
  else
    TempValue += "0,";
  
//  TempValue += "Eng_Load=";
  if (obd.readPID(PID_ENGINE_LOAD, value)) {
    TempValue += String(value);
    TempValue += ",";
  }
  else
    TempValue += "0,";
  
//  TempValue += "Speed=";
  if (obd.readPID(PID_SPEED, value)) {
    TempValue += String(value);
    TempValue += ",";
  }
  else
    TempValue += "0,";
  
//  TempValue += "RPM=";
  if (obd.readPID(PID_RPM, value)) {
    TempValue += String(value);
    TempValue += ",";
  }
  else
    TempValue += "0,";
  
//  TempValue += "RunTime=";
  if (obd.readPID(PID_RUNTIME, value)) {
    TempValue += String(value);
    TempValue += ",";
  }
  else
    TempValue += "0,";
//  
//  TempValue += "Dist=";
  if (obd.readPID(PID_DISTANCE, value)) {
    TempValue += String(value);
    TempValue += "";
  }
  else
    TempValue += "0";
//  TempValue += "Dummy=1122334455667788990";
  return TempValue;
}


void loop()
{
  OBD_to_GSM = PackageHTTPData();
  http_post();
}
