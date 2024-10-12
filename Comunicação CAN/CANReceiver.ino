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
  // Tenta verificar o tamanho do acote recebido
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    // Se temos um pacote
    Serial.println("Recebido pacote. ");

    if (CAN.packetExtended()) { //verifica se o pacote é estendido
      Serial.println("Estendido");
    }

    if (CAN.packetRtr()) {
      //Verifica se o pacote é um pacote remoto (Requisição de dados), neste caso não há dados
      Serial.print("RTR ");
    }

    Serial.print("Pacote com id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {                      //se o pacote recebido é de requisição, indicamos o comprimento solicitado
      Serial.print(" e requsitou o comprimento ");
      Serial.println(CAN.packetDlc()); //obtem o DLC (Data Length Code, que indica o comprimento dos dados)
    } else {
      Serial.print(" e comprimento "); // aqui somente indica o comprimento recebido
      Serial.println(packetSize);

      //Imprime os dados somente se o pacote recebido não foi de requisição
      while (CAN.available()) {
        Serial.print((char)CAN.read());
      }
      Serial.println();
    }

    Serial.println();
  }
}

