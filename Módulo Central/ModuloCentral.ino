#include <CAN.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("CAN Receiver");

  // Inicia o barramento CAN a 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN");
    while (1);
  }
}

void loop() {
  // Tenta analisar o pacote
  int packetSize = CAN.parsePacket();

  if (packetSize) {  // Se um pacote for recebido
    Serial.print("Pacote recebido com ID 0x");
    Serial.println(CAN.packetId(), HEX);

    // Verifica se o pacote contém dados
    if (!CAN.packetRtr() && packetSize == 2) {  // Deve conter exatamente 2 bytes
      // Lê os dois bytes do RPM
      int rpmHighByte = CAN.read();  // Primeiro byte (mais significativo)
      int rpmLowByte = CAN.read();   // Segundo byte (menos significativo)

      // Reconstrói o valor do RPM
      unsigned int rpm = (rpmHighByte << 8) | rpmLowByte;

      // Exibe o RPM no monitor serial
      Serial.print("RPM recebido: ");
      Serial.println(rpm);
    } else {
      Serial.println("Pacote com tamanho ou formato inválido.");
    }
  }
}
