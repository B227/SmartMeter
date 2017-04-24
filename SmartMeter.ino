unsigned long time = millis();
//Sets the sample rate in ms
long Sample_Speed = 1;

//Set sim speed
long Sim_Speed = 1;

//sete max and min
int Max=401;
int Min=395;

void setup() {
  // put your setup code here, to run once:
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

char Protocol(){
  Serial.println(lan);
  Serial.println(lan.length()+1);
  char a[lan.length()+1];
  lan.toCharArray(a,lan.length()+1);
  Serial.println(a);
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

void loop() {
  // put your main code here, to run repeatedly:
  emu_data = emulator(Sim_Speed, Sample_Speed);
}
