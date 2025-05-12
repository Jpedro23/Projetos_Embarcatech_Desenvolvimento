🌐 Projeto Web de Monitoramento de Temperatura e rosa dos venttos utilizando um Joystick, e leittura de estado de Botões

Este projeto é uma aplicação interativa que utiliza um joystick analógico, um sensor de temperatura e botões físicos conectados a uma Raspberry Pi Pico (ou RP2040), com comunicação via HTTP + WebSocket para atualizar dinamicamente uma interface web.

🔧 Tecnologias Utilizadas

Microcontrolador: Raspberry Pi Pico / RP2040

Servidor Backend: Python (Flask + Flask-SocketIO)

Frontend: HTML, CSS, Tailwind, JavaScript

Comunicação: HTTP Requests + WebSockets (Socket.IO)

Sensores e Componentes:

Sensor de temperatura (interno da Pico)

Joystick analógico (Eixos X e Y)

2 Botões físicos (BUTTON_LEFT e BUTTON_RIGHT)

LEDs RGB para feedback

📆 Estrutura do Projeto

├── include/
|    ├──example_http_client_util.h
|    ├──joystick.h
|    ├──lwipopts_examples_common.h
|    ├──lwipopts.h
|    ├──mbedtls_config_examples_common.h
|    ├──wifi.h # Configuração Wi-Fi para Pico
|    ├──read_temp.c
|    └──mbedtls_config.h
├── src/
|    ├──exemaple_http_client_uttil.c
|    ├──joystick.c
|    ├──wifi.c
|    ├──read_temp.c # Função de leitura e média de temperatura
|    └──picow_http_verify.c
├── web/
│   ├── static/
│   │   ├── style.css         # Estilo da página
│   │   └── script.js         # Lógica da UI com fetch + socket
│   ├── templates/
│   │   └── index.html        # Página web principal
│   └── server.py             # Servidor Flask + SocketIO
└── picow_http_client.c       # Código principal do microcontrolador

🎮 Módulo do Joystick (Rosa dos Ventos)

O joystick é conectado aos pinos analógicos da Pico (ADC0 e ADC1) e usado para detectar a direção em que ele está sendo empurrado:

Eixos X/Y: analisados para identificar direções como norte, sul, leste, oeste e diagonais.

A direção é enviada ao servidor via requisição HTTP:

GET /direction?dir=norte

O servidor emite via WebSocket para o frontend, que rotaciona o ponteiro da bússola (compass-pointer) na tela em tempo real.

🌡️ Sensor de Temperatura

Utiliza o sensor interno de temperatura da Raspberry Pi Pico.

A leitura da temperatura é feita ciclicamente no código em C e enviada ao servidor pela rota:

GET /temperature?value=25.38

O servidor armazena a última temperatura e a disponibiliza pela rota:

GET /ultima_temperatura

O frontend atualiza automaticamente o valor a cada 3 segundos, com animações visuais suaves.

🔘 Estado dos Botões Físicos

Dois botões físicos são monitorados:

BUTTON_LEFT envia /pressed_button_a

BUTTON_RIGHT envia /pressed_button_b

Nenhum botão pressionado envia /unpressed

O servidor emite o estado do botão via Socket.IO com o campo action, que é tratado no frontend para atualizar dinamicamente o texto de um botão na interface.

Exemplo:

socket.on('command', (data) => {
  if (data.action === 'pressed_button_a') atualizarStatus('Botão A Pressionado');
  if (data.action === 'pressed_button_b') atualizarStatus('Botão B Pressionado');
  if (data.action === 'unpressed') atualizarStatus('Solto');
});

🖥️ Interface Web

A página HTML mostra:

A temperatura atual e o horário da última atualização.

A bússola com o ponteiro rotacionando com base na entrada do joystick.

O status atual dos botões físicos (exibido em um botão estilizado na parte inferior).

🚀 Como Rodar

Clone este repositório.

No host do servidor, instale as dependências:

pip install flask flask-socketio eventlet

Execute o servidor:

python server.py

Programe a Raspberry Pi Pico com o firmware .uf2 compilado a partir do main.c (usando SDK do Pico).

Acesse a interface pelo navegador via IP local:

http://<IP_DO_SERVIDOR>:5000

📌 Notas

Certifique-se de que a Raspberry Pi Pico está conectada à mesma rede Wi-Fi do servidor.

A comunicação usa requisições HTTP GET simples para facilitar a compatibilidade.

Todos os eventos em tempo real usam WebSockets via Socket.IO.
