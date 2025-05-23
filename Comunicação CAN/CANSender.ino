#include <CAN.h> //Inclui a biblioteca CAN

void setup() {
  Serial.begin(9600); //inicia a serial para debug
  while (!Serial);

  Serial.println("Transmissor CAN");

  // Inicia o barramento CAN a 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN"); //caso não seja possível iniciar o controlador
    while (1);
  }
}

void loop() {
  // Usando o CAN 2.0 padrão
  //Envia um pacote: o id tem 11 bits e identifica a mensagem (prioridade, evento)
  //o bloco de dados deve possuir até 8 bytes
  Serial.println("Enviando pacote...");

  CAN.beginPacket(0x12); //id 18 em hexadecimal
  CAN.write('h'); //1º byte
  CAN.write('e'); //2º byte
  CAN.write('l'); //3º byte
  CAN.write('l'); //4º byte
  CAN.write('o'); //5º byte
  CAN.endPacket(); //encerra o pacote para envio

  Serial.println("Enviado.");

  delay(1000);

  //Usando CAN 2.0 Estendido
  //Envia um pacote: o id tem 29 bits e identifica a mensagem (prioridade, evento)
  //o bloco de dados deve possuir até 8 bytes
  
  Serial.println("Enviando pacote estendido...");

  CAN.beginExtendedPacket(0xabcdef); //id 11259375 decimal ( abcdef em hexa) = 24 bits preenchidos até aqui
  CAN.write('w'); //1º byte
  CAN.write('o'); //2º byte
  CAN.write('r'); //3º byte
  CAN.write('l'); //4º byte
  CAN.write('d'); //5º byte
  CAN.endPacket(); //encerra o pacote para envio

  Serial.println("Enviado.");

  delay(1000);
}

