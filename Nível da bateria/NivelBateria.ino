#define     vmax   12.0                         //tensão máxima admitida (V)
#define     vmin   5.0                         //tensão mínima admitida (V)
float     voltage = 5;                        //armazena tensão lida (V)
float     porcento = 1;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  voltage = (analogRead(A1)*vmax)/1023.0;
  porcento = ((vmax-voltage)/(vmax-vmin))*100;
  Serial.print("nível de bateria: ");
  Serial.println(porcento);
  Serial.println("%");
  delay(500);
}

