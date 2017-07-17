#include <SoftwareSerial.h>
SoftwareSerial gprsSerial(7,8);
#define TRUE 1
#define FALSE 0
//const char ntk_stnth[] PROGMEM= "AT+CSQ\r\n";
//const char ip_get[] PROGMEM= "AT+SAPBR=2,1\r\n";
//int i,j; //AT+CSQ

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
   //Serial.println("hllo");
   
   /*
   if( Serial.find("1") )
   {
    Serial.println(F("gprs attach OK"));
   }
   else
   {
    Serial.println(F("gprs attach ERROR"));
    return FALSE;
   }
*/
   gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); // bearer settings
/*   if( check_AT_OK())
   {
    Serial.println(F("AT+SAPBR 3,1, contype, gprs OK"));
   }
   else
   {
    Serial.println(F("AT+SAPBR 3,1, contype, gprs ERROR"));
    return FALSE;
   }
*/
   gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"xxxx\"");   // bearer settings
/*   if( check_AT_OK())
   {
    Serial.println(F("AT+SAPBR 3,1, apn OK"));
   }
   else
   {
    Serial.println(F("AT+SAPBR 3,1, apn ERROR"));
    return FALSE;
   }
*/
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
   Serial.begin(9600);
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
   
  
  //for (cntr=0;cntr<10;cntr++)  //loop
   //{
    //http_post();
    //delay(3000);
   //}
   //http_post();
   //delay(1000);
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
//    Serial.write(gprsSerial.read());    
  }
  if(gprsSerial.find("OK"))
    return TRUE;
  else
    return FALSE;
}

static boolean Init_gsm_modem()
{
//  gprsSerial.println("AT");
//  delay(100);
  
  gprsSerial.println("AT");
  delay(100);
  
/*
  if (check_AT_OK())
  {
    Serial.println(F("AT Command OK"));
  }
  else
  {
    Serial.println(F("AT command returned error"));
    return FALSE;
  }
 */
 
  gprsSerial.println("ATE0");
  delay(100);

  /*
  if (check_AT_OK())
  {
    Serial.println(F("ATE0 command OK"));
  }
  else
  {
    Serial.println(F("ATE0 command returned error"));
    return FALSE;
  }  
*/
  gprsSerial.println("AT+CMGF=1");
  delay(100);
  /*
  if (check_AT_OK())
  {
    Serial.println(F("AT+CMGF command OK"));
  }
  else
  {
    Serial.println(F("AT+CMGF command returned error"));
    return FALSE;
  }  
*/
  gprsSerial.println("AT+CNMI=2,2,0,0,0");
  delay(100);
/*
  if (check_AT_OK())
  {
    Serial.println(F("AT+CNMI 2 2 0 0 0 command OK"));
  }
  else
  {
    Serial.println(F("AT+CNMI 2 2 0 0 0 command returned error"));
    return FALSE;
  }  
*/
  return TRUE;
}

void loop()
{
  http_post();
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
  //Serial.println("123");
  //gprsSerial.println("AT+HTTPTERM");  // initialize http service
  //delay(100);
  //Serial.println("123");
  //Serial.println("httpinit");
  gprsSerial.println("AT+HTTPINIT");  // initialize http service
  delay(100);
  //toSerial();
  //Serial.println("321");
/*  if (check_AT_OK())
  {
   Serial.println(F("AT+HTTPINIT command OK")); 
  }
  else
  {
    Serial.println(F("AT+HTTPINIT command returned error"));
    //return FALSE;
  } 
*/  
  //Serial.println("456");
  gprsSerial.println("AT+HTTPPARA=\"CID\",1");
  delay(100);
  //toSerial();
  //Serial.println("654");
/*  
 *   if (check_AT_OK())
  {
   Serial.println(F("AT+HTTPPARA command OK")); 
  }
  else
  {
    Serial.println(F("AT+HTTPPARA command returned error"));
    //return FALSE;
  } 
*/
  return TRUE;
}

boolean http_post_url()
{
  gprsSerial.print("AT+HTTPPARA=\"URL\",\"http://kush.esy.es/vtur/updateDriverLocationAdmin.php?subscriptionkey=Deba&regdno=1234&lat=1729.22&long=7780.22&x_axis=0&y_axis=0&speed=10&alcohol=0&UUID=1122334455667788990"); // set http param value
  delay(2000);
  gprsSerial.print("\"\r\n");
  delay(100);
  toSerial();
  gprsSerial.print(0x1A);
  delay(2000);
  Serial.print("at_httppara");
/*  if (check_AT_OK())
  {
   Serial.println(F("AT+HTTPPARA URL command OK")); 
  }
  else
  {
    Serial.println(F("AT+HTTPPARA URL command returned error"));
    return FALSE;
  } 
*/
  gprsSerial.println("AT+HTTPACTION=0"); //set http action type 0 = GET, 1 = POST, 2 = HEAD
  delay(500);
  toSerial();
  Serial.println("AT_httpaction");
  //gprsSerial.println("AT+HTTPREAD");
  //delay(100);
  //toSerial();
  //Serial.print("at_httphead");
  gprsSerial.println("AT+HTTPTERM");
/*  if (gprsSerial.find("+HTTPACTION: 0,200") )
  {
   Serial.println(F("AT+HTTPACTION command 200 OK")); 
  }
  else if (gprsSerial.find(",601") )
  {
//   check_gprs_status_and_reinit();
   Serial.println(F("AT+HTTPACTION command OK")); 
  }
  else
  {
    Serial.println(F("AT+HTTPACTION command returned error"));
//    return FALSE;
  } 
*/
  //Serial.println("123");
  //gprsSerial.println("AT+HTTPTERM");  // initialize http service
  //delay(100);
  //toSerial();
  //Serial.println("123");
  
  return TRUE;
}

void http_post()
{
 int http_post_url_val = 0;

  http_init();  
  
  do {
    http_post_url_val = http_post_url();
/*    if(0 == http_post_url_val)
      check_gprs_status_and_reinit();
*/
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

/*  if(check_AT_OK())
  {
    Serial.println(F("restart_gsm:at+cfun 1,1 is OK"));   
    do
    {
      init_gsm_value = Init_gsm_modem();
    } while(init_gsm_value!=1);
  }
  else
  {
    Serial.println(F("restart_gsm:at+cfun 1,1 is NOT OK"));
    return FALSE; //Take care of this false condition
  }
*/
  return TRUE;
}
