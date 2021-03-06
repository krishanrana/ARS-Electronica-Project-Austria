//2016 ARS Electronica Festival - On The Line Project

//This is the Arduino code required to control the Linzer Schnitters using RDS commands over the I2C protocol.
//The default commands which can be sent are 'ON' and 'OFF'
//To control a LinzerSchnitter using the serial interface:
//1. Type a command which will specify which LS unit you would like to control; the syntax for this is as follow:
// >1;    this command will activate the LS with ID 0001
//2. Send an uppercase 'I' to turn on the output or an uppercase 'O' to turn off the output

//LinzerSchnitteID 65535 will control all of the connected LS units simultaneously


#include <Wire.h>
#include <Adafruit_Si4713.h>

#define RESETPIN 12
#define FMSTATION 8950      // 8950 == 89.50 MHz

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);
int LinzerSchnitteID;
int ID;
int byteRead;
boolean StartSerial;
boolean PowerStatus;

void setup() {

  Serial.begin(9600);
  Serial.println("Linzer Schnitte RDS Test");
  ID = 0;
  StartSerial = false;

  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    while (1);
  }

  // Uncomment to scan power of entire range from 87.5 to 108.0 MHz
  /*
    for (uint16_t f  = 8750; f<10800; f+=10) {
    radio.readTuneMeasure(f);
    Serial.print("Measuring "); Serial.print(f); Serial.print("...");
    radio.readTuneStatus();
    Serial.println(radio.currNoiseLevel);
    }
  */


  radio.setTXpower(115);  // dBuV, 88-115 max
  radio.tuneFM(FMSTATION); // 102.3 mhz
  // This will tell you the status in case you want to read it from the chip
  radio.readTuneStatus();


  // begin the RDS/RDBS transmission
  radio.beginRDS();
  radio.setRDSstation("ARS Electronica");
  radio.setRDSbuffer("On_The_Line");
  Serial.println("RDS is Transmitting!");
  radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output

}



void loop() {

  while (Serial.available()==0);
    //read the most recent byte
    byteRead = Serial.read();


    if (byteRead == 62) { //waiting for ASCII input '>' to begin serial line
      StartSerial = true;
    }

    if (byteRead == 73) { // if ASCII input is 'I' to indicate turn on
      PowerStatus = true;
      Serial.println("Turning ON");
    }
    else if (byteRead == 79) {
      PowerStatus = false;
      Serial.println("Turning OFF");
    }

    if (byteRead > 47 && byteRead < 58 && StartSerial == true) {
      //number found
      ID = (ID * 10) + (byteRead - 48);
    }

    if (byteRead == 59 && StartSerial == true) { //waiting for ASCII input ';' to end serial line

      Serial.println("Final ID:");
      Serial.println(ID);
      LinzerSchnitteID = ID;
      ID = 0;
      StartSerial = false;
    }
  
  if (PowerStatus == true) {

    setLinzerSchnitteRDS(0x03, LinzerSchnitteID, 0x0000 ); //turns output on

  }

  if (PowerStatus == false) {

    setLinzerSchnitteRDS(0x04, LinzerSchnitteID, 0x0000 ); //turns out off

  }
}

void setLinzerSchnitteRDS(int command, int address, int data) { //I2C Communication; Obtain command and data information from LinzerSchnitte Wiki


  Wire.beginTransmission(0x63);  //transmit to device 0x63

  Wire.write(0x35); //CMD_TX_RDS_BUFF Register
  Wire.write(0x67);
  Wire.write(0x60);  //B0
  Wire.write(command & 0b00011111);  //B1
  Wire.write((address >> 8) & 0b11111111);   //C0
  Wire.write((address) & 0b11111111);    //C1
  Wire.write((data >> 8) & 0b11111111);   //D0
  Wire.write((data) & 0b11111111);  //D1

  Wire.endTransmission();

  delay(5);

}




