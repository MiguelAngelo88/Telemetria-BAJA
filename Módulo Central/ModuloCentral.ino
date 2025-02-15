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

#define velocidade 0x61
#define rpmMotor 0x62
#define tempFreio 0x63
#define tempCVT 0x64

unsigned char vel[8] = { 0x5a, 0xa5, 0x05, 0x82, velocidade, 0x00, 0x00, 0x00 };
unsigned char rpm[8] = { 0x5a, 0xa5, 0x05, 0x82, rpm, 0x00, 0x00, 0x00 };
unsigned char freio[8] = { 0x5a, 0xa5, 0x05, 0x82, freio, 0x00, 0x00, 0x00 };
unsigned char cvt[8] = { 0x5a, 0xa5, 0x05, 0x82, cvt, 0x00, 0x00, 0x00 };


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

void setup() {
  Serial.begin(115200);

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

      if (CAN.packetId() == 0x15){// Lê os dois bytes do RPM
      int velocidadeHighByte = CAN.read();  // Primeiro byte (mais significativo)
      int velocidadeLowByte = CAN.read();   // Segundo byte (menos significativo)

      // Reconstrói o valor da Velocidade
      unsigned int velocidade = (velocidadeHighByte << 8) | velocidadeLowByte;

      // Exibe a velocidade no monitor serial
      Serial.print("Velocidade recebido: ");
      Serial.println(velocidade);

      envia_informacoes_lora(velocidade,0,0,0,0,0);
      } else if(CAN.packetId() == 0x16){
      int freioHighByte = CAN.read();  // Primeiro byte (mais significativo)
      int freioLowByte = CAN.read();   // Segundo byte (menos significativo)

      // Reconstrói o valor da Temp do freio
      unsigned int tempFreio = (freioHighByte << 8) | freioLowByte;

      // Exibe o RPM no monitor serial
      Serial.print("TempFreio recebido: ");
      Serial.println(tempFreio);

      envia_informacoes_lora(0,tempFreio,0,0,0,0);
      } else if(CAN.packetId() == 0x17){
      int bateriaHighByte = CAN.read();  // Primeiro byte (mais significativo)
      int bateriaLowByte = CAN.read();   // Segundo byte (menos significativo)

      // Reconstrói o valor do Nivel da Bateria
      unsigned int NivelBateria = (bateriaHighByte << 8) | bateriaLowByte;

      // Exibe o RPM no monitor serial
      Serial.print("NivelBateria recebido: ");
      Serial.println(NivelBateria);

      envia_informacoes_lora(0,0,NivelBateria,0,0,0);
      } else if(CAN.packetId() == 0x18){
      int rpmHighByte = CAN.read();  // Primeiro byte (mais significativo)
      int rpmLowByte = CAN.read();   // Segundo byte (menos significativo)

      // Reconstrói o valor do RPM
      unsigned int rpm = (rpmHighByte << 8) | rpmLowByte;

      // Exibe o RPM no monitor serial
      Serial.print("RPM recebido: ");
      Serial.println(rpm);

      envia_informacoes_lora(0,0,0,rpm,0,0);
      } else if(CAN.packetId() == 0x19){
      int cvtHighByte = CAN.read();  // Primeiro byte (mais significativo)
      int cvtLowByte = CAN.read();   // Segundo byte (menos significativo)

      // Reconstrói o valor do Nivel da Bateria
      unsigned int nivelCVT = (cvtHighByte << 8) | cvtLowByte;

      // Exibe o RPM no monitor serial
      Serial.print("NivelCVT recebido: ");
      Serial.println(nivelCVT);

      envia_informacoes_lora(0,0,0,0,nivelCVT,0);
    } else if(CAN.packetId() == 0x20){
      int nivelCombustivel = CAN.read(); 

      // Exibe o RPM no monitor serial
      Serial.print("NivelCombustivel recebido: ");
      Serial.println(nivelCombustivel);

      envia_informacoes_lora(0,0,0,0,0,nivelCombustivel);     
    } else {
      Serial.println("Pacote com tamanho ou formato inválido.");
    }
  }
 }
}

void envia_informacoes_lora(unsigned int velocidade, unsigned int temperaturaFreio, unsigned int nivelBateria, unsigned int rpm, unsigned int nivelCVT, unsigned int combustivel){
 
  TDadosLora dados_lora;

  dados_lora.velocidadeLoRa = velocidade;
  dados_lora.freioLoRa = temperaturaFreio;
  dados_lora.bateriaLoRa = nivelBateria;
  dados_lora.rpmLoRa = rpm;
  dados_lora.cvtLoRa = nivelCVT;
  dados_lora.combustivelLoRa = combustivel;

  LoRa.beginPacket();
  LoRa.write((uint8_t*)&dados_lora, sizeof(TDadosLora));
  LoRa.endPacket();

// Exibe no monitor serial os dados enviados
  Serial.print("LoRa enviou dados: ");
  Serial.print("Velocidade: ");
  Serial.print(velocidade);
  Serial.print(", TempFreio: ");
  Serial.print(temperaturaFreio);
  Serial.print(", NivelBateria: ");
  Serial.print(nivelBateria);
  Serial.print(", RPM: ");
  Serial.print(rpm);
  Serial.print(", NivelCVT: ");
  Serial.print(nivelCVT);
  Serial.print(", Combustivel: ");
  Serial.println(combustivel);
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

int send_Display() {
  //Envia o valor da velocidade para o display
  vel[6] = highByte(t);
  vel[7] = lowByte(t);
  mySerial.write(vel, 8);
 
}
