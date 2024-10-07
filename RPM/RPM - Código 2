const int n = 10 // número de dentes
float RPM = 0;
unsigned long Ti =0, Tf = 0; //tempo inicial e final


void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), contagem, FALLING);


}


void loop() {
 Serial.println(RPM);
 delay(1000);
}


void contagem(){
  Tf = micros();
  unsigned long delta = Tf - Ti;
  RPM = (60)/ (float(delta)/1000000*n); // = 60*n / T
  Ti = Tf;
}
