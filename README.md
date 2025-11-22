# 🎓 SkillBridge-KODA (IoT)


##Participantes##
#Márcio Gastaldi - RM98811
#Arthur Bessa Pian - RM99215
#Davi Desenzi - RM550849

O **SkillBridge Focus Station** é uma solução de IoT desenvolvida para monitorar a qualidade ambiental de locais de estudo. O sistema utiliza um ESP32 para coletar telemetria em tempo real e interagir com um Broker MQTT, permitindo a gestão de sessões de foco (Smart Session).

## 📋 Funcionalidades

- **Monitoramento Ambiental:** Leitura de Temperatura, Umidade (DHT22) e Luminosidade (LDR).
- **Smart Focus Mode:** Botão físico para alternar o status de estudo ("Foco" vs "Descanso"), com feedback visual (LED) e sonoro (Buzzer).
- **Alertas de Ambiente:** Notifica via MQTT se o ambiente estiver impróprio (ex: baixa luminosidade).
- **Resiliência de Rede:** Sistema de reconexão automática para WiFi e Broker MQTT (Watchdog de conexão).

## 🛠️ Hardware Necessário

| Componente | GPIO (Pino) | Função | Observação |
| :--- | :---: | :--- | :--- |
| **ESP32 DevKit V1** | - | Microcontrolador | - |
| **Sensor DHT22** | `D15` | Temp/Umidade | - |
| **LDR (Fotorresistor)** | `D34` | Luminosidade | Ligar no 3.3V (Input Only) |
| **Push Button** | `D4` | Controle de Modo | Pull-up interno ativado |
| **LED** | `D2` | Indicador de Status | - |
| **Buzzer** | `D5` | Feedback Sonoro | Buzzer Ativo |

## 🔌 Topologia e Conexão

O projeto utiliza o protocolo **MQTT** para comunicação. 

- **Broker:** `broker.emqx.io` (Público)
- **Porta:** `1883` (TCP/ESP32)

### Tópicos MQTT (API)

| Tipo | Tópico | Descrição | Exemplo de Payload |
| :--- | :--- | :--- | :--- |
| **Saída** | `Koda/davi/station1/telemetry` | Dados dos sensores e status | `{"temp":25.0, "lum":2048, "mode":"ON"}` |
| **Saída** | `Koda/davi/station1/alerts` | Alertas de qualidade do ar/luz | `{"alert": "low_light"}` |
| **Entrada** | `koda/davi/station1/cmd` | Comandos remotos | `focus_on` / `focus_off` |

## 💻 Como Executar

### Pré-requisitos
- IDE do Arduino ou VS Code (PlatformIO).
- Bibliotecas:
  - `PubSubClient` (Nick O'Leary)
  - `DHT sensor library` (Adafruit)

### Simulação (Wokwi)
Este projeto foi validado no simulador Wokwi.
1. Configure o `diagram.json` conforme a tabela de hardware.
2. Carregue o código `SkillBridge_Station.ino`.
3. Conecte-se ao Broker EMQX para visualizar os dados.
