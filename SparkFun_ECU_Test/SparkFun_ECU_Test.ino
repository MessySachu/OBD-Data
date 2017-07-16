//Include necessary libraries for compilation
#include <SPI.h>
#include <SD.h>
#include <Canbus.h>

const int chipSelect = 9;
//Declare SD File
File dataFile;

char *EngineRPM;
char buffer[64];

void setup() {
  Serial.begin(9600);
  pinMode(chipSelect, OUTPUT);
  if(Canbus.init(CANSPEED_500))  /* Initialize MCP2515 CAN controller at the specified speed */
  {
    Serial.println("CAN Init Ok");
    delay(1500);
  } 
  else
  {
    Serial.println("Can't init CAN");
    return;
  } 
  if (!SD.begin(chipSelect)) {
    Serial.println("uSD card failed to initialize, or is not present");
    return;
  }
  else{
      Serial.println("uSD card initialized.");
      delay(1500);
  } 
  delay(1000); 
}

void loop(){
  Canbus.ecu_req(ENGINE_RPM,buffer); //Request engine RPM
  EngineRPM = buffer;
  Serial.print("Engine RPM: "); //Uncomment for Serial debugging
  Serial.print(buffer);

  Canbus.ecu_req(ENGINE_COOLANT_TEMP,buffer);
  Serial.print("    Coolant Temp: ");
  Serial.print(buffer);

  Canbus.ecu_req(VEHICLE_SPEED,buffer);
  Serial.print("    Speed: ");
  Serial.print(buffer);

  Canbus.ecu_req(MAF_SENSOR,buffer);
  Serial.print("    MAF Sensor: ");
  Serial.print(buffer);

  Canbus.ecu_req(O2_VOLTAGE,buffer);
  Serial.print("    Voltage:  ");
  Serial.print(buffer);
  
  Canbus.ecu_req(THROTTLE,buffer);
  Serial.print("    Throttle:  ");
  Serial.print(buffer);

  Serial.println();
  
  delay(100);
}
