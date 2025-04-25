#include <CAN.h>
#include <WiFi.h>
#include <Wire.h>
#include <LoRa.h>
#include <SPI.h>
#include <SoftwareSerial.h>

/* Definicoes para comunicação com o display */
const byte rxPin = 16;  //rx2
const byte txPin = 17;  //tx2
SoftwareSerial mySerial(rxPin, txPin);
#define DISP_ID_VELOCIDADE 0x61
#define DISP_ID_RPM 0x62
#define DISP_ID_FREIO 0x63
#define DISP_ID_CVT 0x64
#define DISP_ID_BATERIA 0x65
#define DISP_ID_COMBUSTIVEL 0x66
unsigned char vel[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_VELOCIDADE, 0x00, 0x00, 0x00 };
unsigned char rpm[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_RPM, 0x00, 0x00, 0x00 };
unsigned char freio[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_FREIO, 0x00, 0x00, 0x00 };
unsigned char cvt[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_CVT, 0x00, 0x00, 0x00 };
unsigned char bat[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_BATERIA, 0x00, 0x00, 0x00 };
unsigned char comb[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_COMBUSTIVEL, 0x00, 0x00, 0x00 };

/* Definicoes para comunicação com radio LoRa */
#define SCK_LORA           5
#define MISO_LORA          19
#define MOSI_LORA          27
#define RESET_PIN_LORA     14
#define SS_PIN_LORA        18
#define HIGH_GAIN_LORA     20  /* dBm */
#define BAND               915E6  /* 915MHz de frequencia */

/* typedefs */
typedef struct __attribute__((__packed__))
{
  unsigned int rpmLoRa;
  unsigned int velocidadeLoRa;
  unsigned int combustivelLoRa;
  unsigned int bateriaLoRa;
  unsigned int freioLoRa;
  unsigned int cvtLoRa;
} TDadosLora;

TDadosLora dados_lora_atual = {0};
TDadosLora dados_lora_anterior = {0};

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);

  initializeCAN();  // Configura a comunicação CAN

  /* Tenta, até obter sucesso, comunicacao com o chip LoRa */
  while (init_comunicacao_lora() == false);
}

void loop() {
  // Tenta analisar o pacote
  int packetSize = CAN.parsePacket();

  if (packetSize) {  // Se um pacote for recebido
    Serial.print("Pacote recebido com ID 0x");
    Serial.println(CAN.packetId(), HEX);

    // Verifica se o pacote contém dados
    if (!CAN.packetRtr() && packetSize == 2) {  // Deve conter exatamente 2 bytes

      if (CAN.packetId() == 0x15){ //ID 0x15->Velocidade
      int velocidadeHighByte = CAN.read();  // Primeiro byte (mais significativo)
      int velocidadeLowByte = CAN.read();   // Segundo byte (menos significativo)

      // Reconstrói o valor da Velocidade
      unsigned int velocidade = (velocidadeHighByte << 8) | velocidadeLowByte;

      // Exibe a velocidade no monitor serial
      Serial.print("Velocidade recebido: ");
      Serial.println(velocidade);

      //Envia o valor da velocidade para o display
      envia_para_display(vel, velocidade);

      //Atualiza o valor da velocidade para a comunicação LoRa
      dados_lora_atual.velocidadeLoRa = velocidade;

      } else if(CAN.packetId() == 0x16){ //ID15->Temperatura do freio
      int freioHighByte = CAN.read();  // Primeiro byte (mais significativo)
      int freioLowByte = CAN.read();   // Segundo byte (menos significativo)

      // Reconstrói o valor da Temp do freio
      unsigned int tempFreio = (freioHighByte << 8) | freioLowByte;

      // Exibe a temp do freio no monitor serial
      Serial.print("TempFreio recebido: ");
      Serial.println(tempFreio);

      //Envia o valor da temp do freio para o display
      envia_para_display(freio, tempFreio);

      //Atualiza o valor da temp do freio para a comunicação LoRa
      dados_lora_atual.freioLoRa = tempFreio;

      } else if(CAN.packetId() == 0x17){ //ID17->Nível da bateria
      int bateriaHighByte = CAN.read();  // Primeiro byte (mais significativo)
      int bateriaLowByte = CAN.read();   // Segundo byte (menos significativo)

      // Reconstrói o valor do Nivel da Bateria
      unsigned int NivelBateria = (bateriaHighByte << 8) | bateriaLowByte;

      // Exibe o nivel da bateria no monitor serial
      Serial.print("NivelBateria recebido: ");
      Serial.println(NivelBateria);

      //Envia o valor do nivel da bateria para o display
      envia_para_display(bat, NivelBateria);

      //Atualiza o valor do nivel da bateria para a comunicação LoRa
      dados_lora_atual.bateriaLoRa = NivelBateria;

      } else if(CAN.packetId() == 0x18){ //ID18->RPM
      int rpmHighByte = CAN.read();  // Primeiro byte (mais significativo)
      int rpmLowByte = CAN.read();   // Segundo byte (menos significativo)

      // Reconstrói o valor do RPM
      unsigned int RPM = (rpmHighByte << 8) | rpmLowByte;

      // Exibe o RPM no monitor serial
      Serial.print("RPM recebido: ");
      Serial.println(RPM);

      //Envia o valor do RPM para o display
      envia_para_display(rpm, RPM);

      //Atualiza o valor do RPM para a comunicação LoRa
      dados_lora_atual.rpmLoRa = RPM;

      } else if(CAN.packetId() == 0x19){ //ID19->Temperatura da CVT
      int cvtHighByte = CAN.read();  // Primeiro byte (mais significativo)
      int cvtLowByte = CAN.read();   // Segundo byte (menos significativo)

      // Reconstrói o valor do Nivel da Bateria
      unsigned int tempCVT = (cvtHighByte << 8) | cvtLowByte;

      // Exibe o RPM no monitor serial
      Serial.print("tempCVT recebido: ");
      Serial.println(tempCVT);

      //Envia o valor do RPM para o display
      envia_para_display(cvt, tempCVT);

      //Atualiza o valor da temp da cvt para a comunicação LoRa
      dados_lora_atual.cvtLoRa = tempCVT;

    } else if(CAN.packetId() == 0x20){ //ID20->Nível de combustível
      int nivelCombustivel = CAN.read(); 

      // Exibe o nivel de combustivel no monitor serial
      Serial.print("NivelCombustivel recebido: ");
      Serial.println(nivelCombustivel);

      //Envia o nivel de combustivel para o display
      envia_para_display(comb, nivelCombustivel);

      //Atualiza o valor do nivel de combustivel para a comunicação LoRa
      dados_lora_atual.combustivelLoRa = nivelCombustivel; 

    } else {
      Serial.println("Pacote com tamanho ou formato inválido.");
    }
  }
 }
  envia_dados_lora();
}

void envia_dados_lora() {
  if (memcmp(&dados_lora_atual, &dados_lora_anterior, sizeof(TDadosLora)) == 0) {
    return; // Sem mudanças, não envia
  }

  LoRa.beginPacket();
  LoRa.write((uint8_t*)&dados_lora_atual, sizeof(TDadosLora));
  LoRa.endPacket();

  dados_lora_anterior = dados_lora_atual;

  Serial.printf("[LoRa Enviado] vel: %u, freio: %u, bat: %u, rpm: %u, cvt: %u, comb: %u\n",
                dados_lora_atual.velocidadeLoRa,
                dados_lora_atual.freioLoRa,
                dados_lora_atual.bateriaLoRa,
                dados_lora_atual.rpmLoRa,
                dados_lora_atual.cvtLoRa,
                dados_lora_atual.combustivelLoRa);
}

// Inicializa a comunicação CAN
void initializeCAN() {
  CAN.setPins(15, 4);
  Serial.println("Inicializando o Transmissor CAN");
  if (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN");
  }
}

bool init_comunicacao_lora(void){
  bool status_init = false;
  Serial.println("[LoRa Sender] Tentando iniciar comunicacao com o radio LoRa...");
  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_PIN_LORA);
  LoRa.setPins(SS_PIN_LORA, RESET_PIN_LORA, LORA_DEFAULT_DIO0_PIN);

  if (!LoRa.begin(BAND))
  {
    Serial.println("[LoRa Sender] Comunicacao com o radio LoRa falhou. Nova tentativa em 1 segundo...");
    delay(1000);
    status_init = false;
  }
  else
  {
    /* Configura o ganho do receptor LoRa para 20dBm, o maior ganho possível (visando maior alcance possível) */
    LoRa.setTxPower(HIGH_GAIN_LORA);
    Serial.println("[LoRa Sender] Comunicacao com o radio LoRa ok");
    status_init = true;
  }

  return status_init;
}

void envia_para_display(unsigned char* pacote, unsigned int valor) {
  pacote[6] = highByte(valor);
  pacote[7] = lowByte(valor);
  mySerial.write(pacote, 8);
}

/*OUTRA ABORDAGEM MAIS LEVE
montar o pacote dentro da própria função, recebendo apenas o ID do display e o valor. Assim, você não precisa declarar um array diferente para cada variável. Exemplo:

void envia_para_display(uint8_t id_display, unsigned int valor) {
  unsigned char pacote[8] = { 0x5a, 0xa5, 0x05, 0x82, id_display, 0x00, highByte(valor), lowByte(valor) };
  mySerial.write(pacote, 8);
}

E aí você chamaria assim:
envia_para_display(DISP_ID_VELOCIDADE, velocidade);

*/
