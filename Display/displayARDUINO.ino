/* For Software serial */
#include <SoftwareSerial.h>
const byte rxPin = 16;  //rx2
const byte txPin = 17;  //tx2
SoftwareSerial mySerial(rxPin, txPin);

#define temperatura 0x61
unsigned char temp[8] = { 0x5a, 0xa5, 0x05, 0x82, temperatura, 0x00, 0x00, 0x00 };

unsigned long tempoSensor = millis();
int t = 0;  // Inicializa a variável t com 0

void setup() {

  Serial.begin(115200);
  mySerial.begin(115200);

}

void loop() {
  if ((millis() - tempoSensor) >= 500) {
    Send_Sensor();
    tempoSensor = millis();

    // Incrementa t e reseta quando chegar a 60
    t++;
    if (t > 60) {
      t = 0;
    }
  }
}

int Send_Sensor() {

  temp[6] = highByte(t);
  temp[7] = lowByte(t);
  mySerial.write(temp, 8);
  Serial.println(t);
  return t;
 
}
