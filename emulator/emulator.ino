#include <avr/pgmspace.h>
#include "voltage.h"

unsigned long time = millis();
//Sets the sample rate in ms, lowest is 5 and other set functions are a multiple of 5 seconds
long Sample_Speed = 1;

//Set sim speed, 1 is real time
long Sim_Speed = 1;
//samp 120 and sim 600 is a good rate for quick test

//set max and min
int Max = 5;
int Min = 1;

int index = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Random seed for true random
  //randomSeed(analogRead(0));

  Serial.print("Sample rate at: ");
  Serial.println(Sample_Speed);
  Serial.print("Simulator speed at: ");
  Serial.println(Sim_Speed);
  Serial.print("Minimum at: ");
  Serial.println(Min);
  Serial.print("Maximum at: ");
  Serial.println(Max);
}

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

struct data_int {
  //Effect in watt
  uint32_t Effect;
  //Effect in watt hours
  uint32_t Effect_Hour;
  //Voltage in milli volt
  uint32_t Voltage;
  //Amp in amp
  uint32_t Ampere;
  //Time in ms
  uint32_t Time_Stamp;
};
data_int emu_int;

struct data_int emulator(int sim, int sample) {
  float volt_temp;
  float amp_temp;
  float effect_temp;
  if (index == 8640) {
    index = 0;
  }
  if (time + ((Sample_Speed * 5000) / sim) <= millis()) {
    //for calcs
    data_types temp;
    //for database comp
    data_int temp_int;
    
    //real life time stamps not affected by sim speed
    Serial.print("Sample at: ");
    Serial.println(time / 1000);
    
    //save time
    temp.Time_Stamp = (time * sim);
    temp_int.Time_Stamp = temp.Time_Stamp/1000;
Serial.println(temp_int.Time_Stamp);

    //12 hours cycle
    temp.Voltage = pgm_read_word_near(volt_array + index);
    temp_int.Voltage = temp.Voltage;
    
Serial.println(temp_int.Voltage);
    index++;
    volt_temp=temp.Voltage;
    //10 times bigger
    //Serial.println(volt_temp/10,1);

    //12 hours cycle
    temp.Ampere = ((sin(((temp.Time_Stamp / 1000) * 3.14159) / 21600) * ((Max - Min) / 2)) + ((Max + Min) / 2)) * 1000;
    temp_int.Ampere = temp.Ampere/100;
Serial.println(temp_int.Ampere);
    amp_temp=temp.Ampere;
    //1000 times bigger
    //Serial.println(amp_temp/1000,3);

    //effects
    temp.Effect = (amp_temp * volt_temp) * 5;
    temp_int.Effect = temp.Effect/10000;
Serial.println(temp_int.Effect);    
    temp.Effect_Hour = (temp.Effect / ((3600) / sample)); 
    temp_int.Effect_Hour = temp.Effect_Hour/100;
Serial.println(temp_int.Effect_Hour); 
    //10000 bigger than what it really is
    effect_temp = temp.Effect;
    effect_temp = (effect_temp / ((3600) / sample)) / 10000;

    //info print
    Serial.print("Emulated effect: ");
    Serial.print(effect_temp, 3);
    Serial.print(" Wh at :");
    Serial.println(temp.Time_Stamp / 1000);
    time = millis();
    return temp_int;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  emu_int = emulator(Sim_Speed, Sample_Speed);
}
