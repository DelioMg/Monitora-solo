#include <HX711.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

// Pines de conexão do HX711
const int SCK_PIN = D1;
const int DT_PIN = D2;
HX711 balanca;

// Fator de calibração
float fatorCalibracao = -192700;
float pesoAtual = 0.0;

// Variável para armazenar o peso inicial da terra
float pesoInicialTerra = 1.0;  // Valor padrão de 1kg

// Configurações da rede Wi-Fi e IP fixo
const char* ssid = "Seu_SSID";
const char* password = "Sua_Senha";
IPAddress ip(192, 168, 0, 150);         // IP fixo
IPAddress gateway(192, 168, 0, 1);      // Gateway da sua rede
IPAddress subnet(255, 255, 255, 0);     // Máscara de sub-rede

// Inicia o servidor web
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Inicializa a balança
  balanca.begin(DT_PIN, SCK_PIN);
  balanca.set_scale(fatorCalibracao);
  balanca.tare(); // Zera o peso atual

  // Configura o IP fixo antes de conectar ao Wi-Fi
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Conectado ao Wi-Fi!");
  Serial.print("IP fixo atribuído: ");
  Serial.println(WiFi.localIP());

  // Servir a página HTML com formulário para inserir o peso inicial e exibir peso e umidade
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><head><title>Peso e Umidade</title></head>";
    html += "<body>";
    html += "<h1>Peso Atual: <span id='peso'>0.00</span> kg</h1>";
    html += "<h2>Umidade do Solo: <span id='umidade'>0.00</span> %</h2>";
    
    // Exibir o valor inicial da terra fora do campo
    html += "<h3>Peso Inicial da Terra: <span id='pesoInicialDisplay'>" + String(pesoInicialTerra, 2) + " kg</span></h3>";

    // Formulário para alterar o peso inicial da terra
    html += "<input type='number' step='0.01' id='pesoInicial' value='" + String(pesoInicialTerra, 2) + "'>";
    html += "<button onclick='atualizarPesoInicial()'>Atualizar</button>";

    // JavaScript para atualizar os valores de peso e umidade e para enviar o peso inicial sem sair da página
    html += "<script>";
    
    // Função para atualizar peso e umidade a cada 1 segundo
    html += "setInterval(function() {";
    html += "var xhr = new XMLHttpRequest();";
    html += "xhr.open('GET', '/dados', true);";
    html += "xhr.onload = function() {";
    html += "if (xhr.status == 200) { ";
    html += "var dados = JSON.parse(xhr.responseText);";
    html += "document.getElementById('peso').innerHTML = dados.peso;";
    html += "document.getElementById('umidade').innerHTML = dados.umidade;";
    html += "}";
    html += "};";
    html += "xhr.send();";
    html += "}, 1000);";  // Atualiza a cada 1 segundo
    
    // Função para enviar o valor do peso inicial sem recarregar a página
    html += "function atualizarPesoInicial() {";
    html += "var pesoInicial = document.getElementById('pesoInicial').value;";
    html += "var xhr = new XMLHttpRequest();";
    html += "xhr.open('POST', '/setPesoInicial', true);";
    html += "xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');";
    html += "xhr.onload = function() {";
    html += "if (xhr.status == 200) { ";
    html += "alert('Peso Inicial atualizado para ' + pesoInicial + ' kg'); ";
    html += "document.getElementById('pesoInicialDisplay').innerHTML = pesoInicial + ' kg';"; // Atualiza o valor na página
    html += "}";
    html += "};";
    html += "xhr.send('pesoInicial=' + pesoInicial);";
    html += "}";
    
    html += "</script></body></html>";
    request->send(200, "text/html", html);
  });

  // Rota para enviar o valor do peso e umidade
  server.on("/dados", HTTP_GET, [](AsyncWebServerRequest *request) {
    float umidade = ((pesoAtual - pesoInicialTerra) / pesoInicialTerra) * 100;
    String json = "{\"peso\": \"" + String(pesoAtual, 2) + "\", \"umidade\": \"" + String(umidade, 2) + "\"}";
    request->send(200, "application/json", json);
  });

  // Rota para receber o valor do peso inicial da terra via POST (AJAX)
  server.on("/setPesoInicial", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("pesoInicial", true)) {
      String pesoInicialStr = request->getParam("pesoInicial", true)->value();
      pesoInicialTerra = pesoInicialStr.toFloat();
      Serial.println("Peso Inicial da Terra atualizado: " + String(pesoInicialTerra, 2) + " kg");
    }
    request->send(200, "text/plain", "Peso Inicial atualizado com sucesso!");
  });

  // Inicia o servidor
  server.begin();
}

void loop() {
  // Atualiza o peso a cada 1 segundo
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    pesoAtual = balanca.get_units();
    Serial.println("Peso Atual: " + String(pesoAtual, 2) + " kg");
    lastUpdate = millis();
  }

  // Adiciona um pequeno delay para evitar watchdog reset
  delay(10);
}


