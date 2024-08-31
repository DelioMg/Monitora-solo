## Descrição do Projeto

Este projeto descreve o desenvolvimento de um sistema de monitoramento da umidade do solo utilizando o microcontrolador ESP32 e o sensor de umidade K6440, integrados através do circuito condicionador de sinal SMX. O sistema foi projetado para coletar dados de umidade do solo periodicamente, condicionar e traduzir esses dados para uma faixa de tensão adequada, que é lida pelo conversor analógico-digital (ADC) do ESP32. Os dados são então transmitidos via Wi-Fi para um sistema supervisório na nuvem, onde são armazenados e visualizados em forma de gráficos.

### Componentes Principais

- **ESP32**: Microcontrolador com capacidade de conexão Wi-Fi.
- **Sensor de Umidade K6440**: Sensor utilizado para medir a umidade do solo.
- **Circuito Condicionador de Sinal SMX**: Interface que converte os sinais analógicos do sensor K6440 em sinais de tensão, corrente ou frequência.

### Funcionamento do Circuito SMX

O circuito SMX é uma interface para sensores que medem a resistência elétrica de substratos úmidos. Ele utiliza excitação AC para prevenir a corrosão do sensor e isola galvanicamente o sensor de correntes subterrâneas indesejadas. O SMX fornece três tipos de saída:
- **Tensão**: Varia de 0,2 a 1,0 volts, adequada para leitura pelo ADC do ESP32.
- **Corrente**: Varia de 0,2 a 1,0 mA.
- **Frequência**: Varia de 50 Hz (solo seco) a 7 kHz (solo úmido).

### Interconexão Elétrica

A interconexão elétrica entre o ESP32, o circuito SMX e o sensor K6440 é configurada da seguinte maneira:
- O GND do SMX e do ESP32 são conectados.
- O VCC do SMX é conectado ao pino de 5V (Vin) do ESP32.
- O SIG-OUT do SMX, que corresponde à saída de tensão, é conectado ao pino 35 do ESP32.
- Um capacitor de 100 µF é colocado entre o GND e o pino 35 do ESP32 para estabilização.

### Plataforma de Programação

A plataforma utilizada para programar o ESP32 é o ESP-IDF (Espressif IoT Development Framework). O ESP-IDF fornece um ambiente de desenvolvimento robusto e uma vasta gama de bibliotecas e ferramentas para facilitar a programação e o desenvolvimento de aplicações IoT com o ESP32.
Em que foi utilizado como base guia de programação ESP-IDF: https://docs.espressif.com/projects/esp-idf/en/v5.3/esp32/api-reference/peripherals/adc_oneshot.html

### Objetivo do Projeto

O objetivo deste projeto é proporcionar aos agricultores informações precisas e em tempo real sobre a umidade do solo, facilitando a tomada de decisões informadas no manejo das plantações, otimizando o uso da água e melhorando a produtividade agrícola.

### Como Utilizar

1. **Montagem**: Conecte o sensor K6440 ao circuito SMX e este ao ESP32 conforme descrito na interconexão elétrica.
2. **Programação**: Utilize o ESP-IDF para programar o ESP32, permitindo que ele leia os dados do sensor e envie as informações via Wi-Fi.
3. **Supervisão**: Configure o sistema supervisório na nuvem para receber, armazenar e visualizar os dados de umidade do solo.

Para mais detalhes sobre a configuração e o código-fonte, consulte a documentação completa no repositório.
