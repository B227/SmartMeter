
/** RF24Mesh_Example.ino by TMRh20
 *
 * This example sketch shows how to manually configure a node via RF24Mesh, and send data to the
 * master node.
 * The nodes will refresh their network address as soon as a single write fails. This allows the
 * nodes to change position in relation to each other and the master node.
 */


#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
//#include <printf.h>


/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**
 * User Configuration: nodeID - A unique identifier for each radio. Allows addressing
 * to change dynamically with physical changes to the mesh.
 *
 * In this example, configuration takes place below, prior to uploading the sketch to the device
 * A unique value from 1-255 must be configured for each node.
 * This will be stored in EEPROM on AVR devices, so remains persistent between further uploads, loss of power, etc.
 *
 **/
#define nodeID 1


uint32_t displayTimer = 0;

struct data_types {
  //Effect in watt
  unsigned long Effect;
  //Effect in watt hours
  unsigned long Effect_Hour;
  //Voltage in milli volt
  unsigned long Voltage;
  //Amp in amp
  unsigned long Ampere;
  //Time in ms
  unsigned long Time_Stamp;
};
data_types emu_data;

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};

void setup() {

  Serial.begin(115200);
  //printf_begin();
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
}

struct data_types SetTemp(struct data_types stData){
  stData.Effect = 33;
  stData.Effect_Hour = 44;
  stData.Voltage = 432;
  stData.Ampere = 21;
  stData.Time_Stamp = 54;
  return stData;
}

void loop() {
  emu_data = SetTemp(emu_data);
  mesh.update();

  // Send to the master node every second
  if (millis() - displayTimer >= 1000) {
    displayTimer = millis();

    //String dat = "Hej Line";
    //Serial.println(sizeof(dat));
    /*char dat[40]="";
    Serial.println(sizeof(dat));
    for(int i=0;i<=sizeof(dat);i++){
      dat[i]='Q';
    }*/
    Serial.print("E");Serial.print(emu_data.Effect);Serial.print(" P");Serial.print(emu_data.Effect_Hour);Serial.print(" V");Serial.print(emu_data.Voltage);Serial.print(" V");Serial.print(emu_data.Ampere);Serial.print(" T");Serial.println(emu_data.Time_Stamp);
    // Send an 'M' type message containing the current millis()
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






