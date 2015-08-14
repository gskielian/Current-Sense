#include <Wire.h>
#define SLAVE_ADDR 0x40
#define CALIBRATION_ADDR 0x05
#define CURRENT_REG_ADDR 0x04

#define R_SHUNT 0.003
#define MAX_EXP_CURRENT 20.0
#define CURRENT_LSB ((MAX_EXP_CURRENT / 32768.0))
#define BUS_VOLTAGE_LSB 1.25
#define CALIBRATION ((0.00512) / (CURRENT_LSB * R_SHUNT))

#define WRITE_MASK 0x80

char ch;

uint16_t luxLsb;
uint16_t luxMsb;
uint16_t bus_voltage;
uint16_t current;

long time1;
long time2;

long start_time;
long end_time;
long pulse_time=900;
long init_delay=2000;

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output

  unsigned int calibration = (unsigned int) CALIBRATION;
  byte calMsb = (unsigned int)(calibration) >> 8;
  byte calLsb = (unsigned int)(calibration) & 0xff;
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(byte(CALIBRATION_ADDR | WRITE_MASK));
  Wire.write(calMsb);
  Wire.write(calLsb);
  Wire.endTransmission();
  //INSTRUCTIONS
  print_instructions();
}

void loop() {


  //wait for input
  while(Serial.available() == 0){}

  //get serial input
  ch = Serial.read();

  if ( ch == 'r' ) { 
    start_procedure();
    Serial.println("--------------------------------------------------------------------------------");
    test_procedure();
  } else if (ch == 'i') {    
    pulse_time+=50;
    Serial.print("increased, pulse_time is now "); Serial.println(pulse_time);
  } else if (ch == 'c') {    
    for (int i=0; i<100;i++) 
      Serial.println("");
  } else if (ch == 'd') {    
    pulse_time-=50;
    Serial.print("decreased, pulse_time is now "); Serial.println(pulse_time);
  } else if (ch == 't') {    
    Serial.print("pulse_time ="); Serial.println(pulse_time);
  } else {
    //INSTRUCTIONS
    print_instructions();
    delay(10);
  }


  Serial.println("--------------------------------------------------------------------------------");

}


void start_procedure() {

  start_time=millis();
  end_time = start_time + init_delay;
  while (millis() < end_time) {

    time1=millis();

    Wire.beginTransmission(0x40);
    Wire.write(4);
    Wire.endTransmission();

    Wire.requestFrom(0x40,2); //requesting 2 bytes from the lux sensor
    //MSB was first so we'll need to shift byte to the left 
    luxMsb = Wire.read();
    luxLsb = Wire.read();
    current = ( ( luxMsb << 8) | luxLsb) ;

    time2=millis();

    Wire.beginTransmission(0x40);
    Wire.write(2);
    Wire.endTransmission();

    Wire.requestFrom(0x40,2); //requesting 2 bytes from the lux sensor
    //MSB was first so we'll need to shift byte to the left 
    luxMsb = Wire.read();
    luxLsb = Wire.read();
    bus_voltage = ( ( luxMsb << 8) | luxLsb) ;


    Serial.print(time1);
    Serial.print(",");
    char str_temp[20];
    Serial.print(dtostrf(((double)(current) * CURRENT_LSB / 10.0),6,4,str_temp));
    Serial.print(",");
    Serial.print(time2);
    Serial.print(",");
    Serial.println((double)(bus_voltage)*BUS_VOLTAGE_LSB/1000);
    delay(1);
  }
}

void test_procedure() {
  start_time=millis();
  end_time = start_time + pulse_time;

  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);
  while (millis() < end_time){


    time1=millis();

    Wire.beginTransmission(0x40);
    Wire.write(4);
    Wire.endTransmission();

    Wire.requestFrom(0x40,2); //requesting 2 bytes from the lux sensor
    //MSB was first so we'll need to shift byte to the left 
    luxMsb = Wire.read();
    luxLsb = Wire.read();
    current = ( ( luxMsb << 8) | luxLsb) ;

    time2=millis();

    Wire.beginTransmission(0x40);
    Wire.write(2);
    Wire.endTransmission();

    Wire.requestFrom(0x40,2); //requesting 2 bytes from the lux sensor
    //MSB was first so we'll need to shift byte to the left 
    luxMsb = Wire.read();
    luxLsb = Wire.read();
    bus_voltage = ( ( luxMsb << 8) | luxLsb) ;


    Serial.print(time1);
    Serial.print(",");
    char str_temp[20];
    Serial.print(dtostrf(((double)(current) * CURRENT_LSB / 10.0),6,4,str_temp));
    Serial.print(",");
    Serial.print(time2);
    Serial.print(",");
    Serial.println((double)(bus_voltage)*BUS_VOLTAGE_LSB/1000);
  }


  digitalWrite(11, LOW);
}


void print_instructions() {
  Serial.println("Instructions:");

  Serial.println("");

  Serial.println("enter \"i\" to increase pulse_time");
  Serial.println("enter \"d\" to reduce pulse_time");
  Serial.println("enter \"t\" to print pulse_time");
  Serial.println("enter \"r\" run procedure");
  Serial.println("enter \"c\" to clear screen");

  Serial.println("");
}
