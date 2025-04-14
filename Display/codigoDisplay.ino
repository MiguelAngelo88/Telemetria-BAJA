/* For Software serial */
#include <SoftwareSerial.h>
const byte rxPin = 16;  //rx2
const byte txPin = 17;  //tx2
SoftwareSerial mySerial(rxPin, txPin);

#define velocidade 0x61
#define rpmMotor 0x62
#define tempFreio 0x63
#define tempCVT 0x64
#define bateria 0x65
#define combustivel 0x66

unsigned char vel[8] = { 0x5a, 0xa5, 0x05, 0x82, velocidade, 0x00, 0x00, 0x00 };
unsigned char rpm[8] = { 0x5a, 0xa5, 0x05, 0x82, rpmMotor, 0x00, 0x00, 0x00 };
unsigned char freio[8] = { 0x5a, 0xa5, 0x05, 0x82, tempFreio, 0x00, 0x00, 0x00 };
unsigned char cvt[8] = { 0x5a, 0xa5, 0x05, 0x82, tempCVT, 0x00, 0x00, 0x00 };
unsigned char bat[8] = { 0x5a, 0xa5, 0x05, 0x82, bateria, 0x00, 0x00, 0x00 };
unsigned char comb[8] = { 0x5a, 0xa5, 0x05, 0x82, combustivel, 0x00, 0x00, 0x00 };

unsigned long tempo = millis();
int t = 0;  // Inicializa a variável t com 0

void setup() {

  Serial.begin(115200);
  mySerial.begin(115200);

}

void loop() {
  if ((millis() - tempo) >= 100) {
    SendToDisplay();
    tempo = millis();

    // Incrementa t e reseta quando chegar a 5000
    t++;
    if (t > 150) {
      t = 0;
    }
  }
}

//substituir t pelo valor real a ser enviado
int SendToDisplay() {

  //envia a velocidade
  vel[6] = highByte(t);
  vel[7] = lowByte(t);
  mySerial.write(vel, 8);

   //envia o rpm
  rpm[6] = highByte(t);
  rpm[7] = lowByte(t);
  mySerial.write(rpm, 8);

   //envia  freio
  freio[6] = highByte(t);
  freio[7] = lowByte(t);
  mySerial.write(freio, 8);

   //envia a cvt
  cvt[6] = highByte(t);
  cvt[7] = lowByte(t);
  mySerial.write(cvt, 8);

   //envia a bat
  bat[6] = highByte(t);
  bat[7] = lowByte(t);
  mySerial.write(bat, 8);

   //envia a comb
  comb[6] = highByte(t);
  comb[7] = lowByte(t);
  mySerial.write(comb, 8);
  
  return t;
}
