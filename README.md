# Arquitetura Interna

![croqui telemetria BAJA](https://github.com/user-attachments/assets/cad6ea5b-cae7-4df8-ae72-bb1294c1feb5)

# Arquitetura Externa
![Arquitetura externa - telemetria BAJA](https://github.com/user-attachments/assets/bad7541f-d757-440a-9046-76423acb5b15)

# Pinout Lora32 V2
![image](https://github.com/user-attachments/assets/a2757a7a-5f3b-4c13-9c52-783dc83b022d)

# Pinout ESP32 DEVKIT V1
![image](https://github.com/user-attachments/assets/86cefb8a-a655-4f22-8672-2a8caeacce06)

# Cronograma de Atividades
https://docs.google.com/spreadsheets/d/1f6BjYHv89a4gd36owJ_0MwVJ5d3ZieXN16XCeML3Rqs/edit?usp=sharing

# Link do Planejamento
https://docs.google.com/document/d/10cPqrGxGNATz-zq_f9W72cQVOdOiKb-08O4iLef-5Vg/edit?usp=sharing

# Pasta do Google Drive
https://drive.google.com/drive/folders/12pf-uwY25VX4iIIwu48RXH_IE0EbaV6g?usp=sharing
  
# Módulo frontal pinout  
| Pino do ESP32  | Componente |
| ------------- | ------------- |
| GPIO4  | CAN RX  |
| GPIO15  | CAN TX  |
| GPIO13  | Sensor indutivo  |
| GPIO12  | Sensor de tensão  |
| GPIO19  | MAX6675 SO |
| GPIO23  | MAX6675 CS  |
| GPIO5  | MAX6675 SCK  |

# Módulo central pinout  
| Pino do ESP32  | Componente |
| ------------- | ------------- |
| GPIO4  | CAN RX  |
| GPIO15  | CAN TX  |
| GPIO5  | SCK_LORA  |
| GPIO19  | MISO_LORA  |
| GPIO27  | MOSI_LORA  |
| GPIO14  | RESET_PIN_LORA  |
| GPIO18  | SS_PIN_LORA  |
| GPIO3  | DISPLAY TX  |
| GPIO1  | DISPLAY RX  |

# Módulo traseiro pinout  
| Pino do ESP32  | Componente |
| ------------- | ------------- |
| GPIO4  | CAN RX  |
| GPIO15  | CAN TX  |
| GPIO21  | MLX90614 SDA|
| GPIO22  |  MLX90614 SCL |

# Ideias futuras
- Múltiplas cenas no Display, controladas via botão ou encoder
- Mapeamento da pista e cálculo da melhor trajetória utilizando Visão Computacional
- Comunicação via radio frequência entre o piloto e o box (capacete com mic e speaker)
- Sistema de Recuperação de Energia para otimizar o consumo
- Implementar um Controle de Tração para detectar patinação e ajustar a distribuição de potência nas rodas
- Sistema de alertas automáticos no display
