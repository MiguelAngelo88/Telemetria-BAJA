/*
  Projeto: emissor- medição local, via LoRa e via mqtt de temperatura e umidade
  Autor: Pedro Bertoleti
*/
#include <WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <LoRa.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*
   Defines do projeto
*/

/* Endereço I2C do display */
#define OLED_ADDR 0x3c

/* distancia, em pixels, de cada linha em relacao ao topo do display */
#define OLED_LINE1 0
#define OLED_LINE2 10
#define OLED_LINE3 20
#define OLED_LINE4 30
#define OLED_LINE5 40
#define OLED_LINE6 50

/* Configuração da resolucao do display (este modulo possui display 128x64) */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

/* Definicoes para comunicação com radio LoRa */
#define SCK_LORA           5
#define MISO_LORA          19
#define MOSI_LORA          27
#define RESET_PIN_LORA     14
#define SS_PIN_LORA        18

#define HIGH_GAIN_LORA     20  /* dBm */
#define BAND               915E6  /* 915MHz de frequencia */

/*
   Variáveis e objetos globais
*/

/* objeto do display */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 16);

float contador=0;

/* typedefs */
typedef struct __attribute__((__packed__))
{
  float contador;
} TDadosLora;

void escreve_temperatura_umidade_display(int cont);
void envia_informacoes_lora(float cont);
bool init_comunicacao_lora(void);


void escreve_temperatura_umidade_display(int cont)
{

  display.clearDisplay();
  display.setCursor(0, OLED_LINE1);
  display.print("    -TRANSMISSOR-");
  display.setCursor(0, OLED_LINE3);
  display.print("Temperatura:");
  display.print(contador);
  display.print(" C");
  display.setCursor(0, OLED_LINE5);
  display.println("Enviando Valor...");
  display.display();
}

void envia_informacoes_lora(float cont)
{
 
  TDadosLora dados_lora;

  dados_lora.contador = contador;

  LoRa.beginPacket();
  LoRa.write((uint8_t*)&dados_lora, sizeof(TDadosLora));
  LoRa.endPacket();

  Serial.printf("Dado enviado: %.2f\n", dados_lora.contador);
}


/* Funcao: inicia comunicação com chip LoRa
   Parametros: nenhum
   Retorno: true: comunicacao ok
            false: falha na comunicacao
*/
bool init_comunicacao_lora(void)
{
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

void setup() {
  /* configura comunicação serial (para enviar mensgens com as medições)
    e inicializa comunicação com o sensor.
  */
  Serial.begin(9600);

  /* inicializa display OLED */
  Wire.begin(4, 15);


  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
    Serial.println("Display OLED: falha ao inicializar");
  else
  {
    Serial.println("Display OLED: inicializacao ok");

    /* Limpa display e configura tamanho de fonte */
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, OLED_LINE1);
    display.println("Emissor. Inicializando...");
    display.display();
  }

  /* Tenta, até obter sucesso, comunicacao com o chip LoRa */
  while (init_comunicacao_lora() == false);
}

/*
   Programa principal
*/
void loop() {

  escreve_temperatura_umidade_display(contador);
  envia_informacoes_lora(contador);
  
  contador++;
  /* espera um segundo até a próxima leitura  */
  delay(1000);
}
