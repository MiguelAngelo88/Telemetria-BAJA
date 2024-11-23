#include <CAN.h> //Inclui a biblioteca CAN

void setup() {
  Serial.begin(9600); //inicia a serial para debug
  while (!Serial);

  Serial.println("Receptor CAN");

  // Inicia o barramento CAN a 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN"); //caso não seja possível iniciar o controlador
    while (1);
  }
}

void loop() {
  // Tenta verificar o tamanho do pacote recebido
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    Serial.println("Recebido pacote.");

    if (!CAN.packetRtr()) { // Somente processa pacotes com dados
      uint8_t rpmHigh = CAN.read(); // Primeiro byte (MSB)
      uint8_t rpmLow = CAN.read();  // Segundo byte (LSB)

      // Reconstruir o valor do RPM
      uint16_t rpmInt = (rpmHigh << 8) | rpmLow;
      float RPM = (float)rpmInt;

      Serial.print("RPM recebido: ");
      Serial.println(RPM);
    }
  }
}
