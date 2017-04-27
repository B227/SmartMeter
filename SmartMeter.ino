#include <avr/pgmspace.h>
#include "voltage.h"
#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>

#define nodeID 1

uint32_t displayTimer = 0;

RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

//unsigned long time = millis();
//Sets the sample rate in ms, lowest is 5 and other set functions are a multiple of 5 seconds
#define Sample_Speed  1

//Set sim speed, 1 is real time
#define Sim_Speed  1
//samp 120 and sim 600 is a good rate for quick test

//set max and min
#define Max  5
#define Min  1

int index = 0;

struct data_types {
  //Effect in watt
  uint16_t Effect;
  //Effect in watt hours
  uint16_t Effect_Hour;
  //Voltage in milli volt
  uint16_t Voltage;
  //Amp in amp
  uint16_t Ampere;
  //Time in ms
  uint16_t Time_Stamp;
};
data_types emu_data;

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Random seed for true random
  randomSeed(analogRead(0));
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();

  Serial.print("Sample rate at: ");
  Serial.println(Sample_Speed);
  Serial.print("Simulator speed at: ");
  Serial.println(Sim_Speed);
  Serial.print("Minimum at: ");
  Serial.println(Min);
  Serial.print("Maximum at: ");
  Serial.println(Max);
}



struct data_types emulator(unsigned long time) {
  float volt_temp;
  float amp_temp;
  float effect_temp;
  if (index == 8640) {
    index = 0;
  }

    //for calcs
    data_types temp;
    //for database comp

    //real life time stamps not affected by sim speed
    Serial.print("Sample at: ");
    Serial.println(time / 1000);

    //save time
    temp.Time_Stamp = (time * Sim_Speed)/1000;


    //12 hours cycle
    temp.Voltage = pgm_read_word_near(volt_array + index);

//Serial.println(temp.Voltage);
    index++;
    volt_temp=temp.Voltage;
    //10 times bigger
    //Serial.println(volt_temp/10,1);

    //12 hours cycle
    temp.Ampere = (((sin(((temp.Time_Stamp) * 3.14159) / 21600) * ((Max - Min) / 2)) + ((Max + Min) / 2)) * 1000);//+random(0,500);
//Serial.println(temp.Ampere);
    amp_temp=temp.Ampere;
    //1000 times bigger

    //effects
    temp.Effect = ((amp_temp * volt_temp) * 5)/10000;
//Serial.println(temp.Effect);
    temp.Effect_Hour = (temp.Effect / ((3.6) / Sample_Speed));
//Serial.println(temp.Effect_Hour);
    //1000 bigger than what it really is
    effect_temp = temp.Effect_Hour;

    //info print
    Serial.print("Emulated effect: ");
    Serial.print(effect_temp/1000, 3);
    Serial.print(" Wh at :");
    Serial.println(temp.Time_Stamp);

    return temp;

}

void loop() {
  mesh.update();
  if (displayTimer + ((Sample_Speed * 5000) / Sim_Speed) <= millis()) {
    displayTimer = millis();
    emu_data = emulator(displayTimer);
    if (!mesh.write(&emu_data, 'M', sizeof(emu_data))) {

      // If a write fails, check connectivity to the mesh network
      if ( ! mesh.checkConnection() ) {
        //refresh the network address
        Serial.println("Renewing Address");
        mesh.renewAddress();
      } else {
        Serial.println("Send fail, Test OK");
      }
    } else {
      Serial.print("Send OK: "); Serial.println(displayTimer);
    }
  }

  while (network.available()) {
    RF24NetworkHeader header;
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.print("Received packet #");
    Serial.print(payload.counter);
    Serial.print(" at ");
    Serial.println(payload.ms);
  }
}
