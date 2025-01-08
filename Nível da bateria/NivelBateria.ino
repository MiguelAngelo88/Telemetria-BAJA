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

/*####################################################*/

#define sensorPin A0

void setup() {
Serial.begin(115200);
}

void loop() {
//measuring voltage using "B25 0 to 25V" Voltage Sensor

float voltage = analogRead(sensorPin)*5*5.0/1023;     //PV panel voltage

Serial.println(voltage);    //send the voltage to serial port

delay(1000); //wait 1s before repeating
}
