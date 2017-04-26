unsigned long time = millis();
//Sets the sample rate in ms
long Sample_Speed = 1;

//Set sim speed
long Sim_Speed = 1;

//sete max and min
int Max=401;
int Min=395;

void setup() {
  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}

  //Random seed for true random
  randomSeed(analogRead(0));

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
data_types emu_data;

struct data_types emulator(int sim, int sample) {
  if (time + ((Sample_Speed * 1000) / sim) <= millis()) {
    data_types temp;
    Serial.print("Sample at: ");
    Serial.println(time);
    temp.Time_Stamp = (time * sim);
    temp.Voltage = (sin(((temp.Time_Stamp/1000)*3.14159)/43200)*((Max-Min)/2)+((Max+Min)/2))*1000;
     Serial.println(temp.Voltage);
    //Serial.println(temp.Voltage);
    temp.Ampere = random(1, 1);
    //Serial.println(temp.Ampere);
    temp.Effect = temp.Ampere * temp.Voltage;
    temp.Effect_Hour = (temp.Effect / (3.6 / sample));
    Serial.print("Emulated effect: ");
    Serial.print(temp.Effect_Hour);
    Serial.print(" Wh at :");
    Serial.println(temp.Time_Stamp);
    time = millis();
    return temp;
  }
}

char *Protocol(struct data_types stData){
  //int lngth;
  String data;
  char *cData;
  data += 'P'; //effekt
  data += stData.Effect;
  data += 'H'; //effekt timer
  data += stData.Effect_Hour;
  data += 'V';
  data += stData.Voltage;
  data += 'A';
  data += stData.Ampere;
  data += 'T';
  data += stData.Time_Stamp;
  cData=new char[data.length()+1];
  data.toCharArray(cData,data.length()+1);
  return cData;
}

struct data_types SetTemp(struct data_types stData){
  stData.Effect = 33;
  stData.Effect_Hour = 44;
  stData.Voltage = 432;
  stData.Ampere = 21;
  stData.Time_Stamp = 54;
  return stData;
}

int Lngth(struct data_types stData){
  int lgt=0;
  lgt += sizeof(stData.Effect)+1;
  lgt += sizeof(stData.Effect_Hour)+1;
  lgt += sizeof(stData.Voltage)+1;
  lgt += sizeof(stData.Ampere)+1;
  lgt += sizeof(stData.Time_Stamp)+1;
  return lgt;  
}

void loop() {
  emu_data = SetTemp(emu_data);
  char *data;
  data = new char[Lngth(emu_data)+1];
  data = Protocol(emu_data);
  Serial.println(data);
  delay(1000);
}
