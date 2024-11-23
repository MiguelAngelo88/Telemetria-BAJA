#include <CAN.h>

#define sensorPin 14

const int n = 2; // Número de pontos detectáveis
float RPM = 0;
unsigned long Ti = 0, Tf = 0; // Tempo inicial e final

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Inicia o barramento CAN a 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN");
    while (1);
  }

  pinMode(sensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), rpmCalc, FALLING);
}

void loop() {
  // Envia o valor do RPM como um pacote CAN
  Serial.println("Enviando RPM...");
  Serial.println(RPM);

  CAN.beginPacket(0x01); // ID do pacote CAN
  CAN.write((byte*)&RPM, sizeof(RPM)); // Envia o valor do RPM (float) como 4 bytes
  CAN.endPacket();

  delay(1000); // Envia a cada segundo
}

void rpmCalc() {
  Tf = micros();
  unsigned long delta = Tf - Ti;
  RPM = (60) / ((float(delta) / 1000000) * n); // RPM = 60*n / T
  Ti = Tf;
}
