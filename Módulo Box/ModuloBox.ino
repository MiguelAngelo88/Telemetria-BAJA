#include <SPI.h>
#include <LoRa.h>

/* Definições do rádio LoRa */
constexpr int SCK_LORA   = 5;
constexpr int MISO_LORA  = 19;
constexpr int MOSI_LORA  = 27;
constexpr int RESET_LORA = 14;
constexpr int SS_LORA    = 18;
constexpr float BAND     = 915E6;

/* Definições do ID da equipe */
constexpr uint8_t ID_EQUIPE = 58;

/* Estrutura de dados recebida */
typedef struct __attribute__((__packed__)) {
  uint8_t idEquipe;         
  uint16_t rpmLoRa;
  uint16_t velocidadeLoRa;
  uint16_t combustivelLoRa;
  uint16_t bateriaLoRa;
  uint16_t freioLoRa;
  uint16_t cvtLoRa;
} TDadosLora;

TDadosLora dadosRecebidos;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("[LoRa Receiver] Inicializando...");

  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_LORA);
  LoRa.setPins(SS_LORA, RESET_LORA, 26); // DIO0 padrão: 26 no ESP32

  if (!LoRa.begin(BAND)) {
    Serial.println("Falha ao iniciar LoRa!");
    while (1);
  }

  Serial.println("[LoRa Receiver] OK!");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize >= sizeof(TDadosLora)) {
    Serial.println("[LoRa Receiver] Pacote recebido.");

    // Ler dados
    LoRa.readBytes((uint8_t*)&dadosRecebidos, sizeof(TDadosLora));

    // Verificar ID
    if (dadosRecebidos.idEquipe == ID_EQUIPE) {
      Serial.println("[LoRa Receiver] Dados da minha equipe recebidos:");
      Serial.printf("Velocidade: %u RPM: %u Combustível: %u Bateria: %u Freio: %u CVT: %u\n",
                    dadosRecebidos.velocidadeLoRa,
                    dadosRecebidos.rpmLoRa,
                    dadosRecebidos.combustivelLoRa,
                    dadosRecebidos.bateriaLoRa,
                    dadosRecebidos.freioLoRa,
                    dadosRecebidos.cvtLoRa);

      // Aqui você pode enviar via Serial para a dashboard Python!
      Serial.write((uint8_t*)&dadosRecebidos, sizeof(TDadosLora));
    } else {
      Serial.printf("[LoRa Receiver] Pacote de outra equipe (ID recebido: %u)\n", dadosRecebidos.idEquipe);
    }
  }
}
