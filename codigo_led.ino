#include <WiFi.h>
#include <WebServer.h>

// Identidad de la red Wi-Fi (Grupo 01)
const char* NOMBRE_RED = "ESP32_LED_G01";
const char* CLAVE_RED = "Mecatronica2026";

// Coincidiendo exactamente con tu diagram.json (GPIO 23)
const int PIN_LED = 23;

WebServer servidor(80);
bool ledEncendido = false;

// Variables para el control de tiempo no bloqueante (Reto 3: 10 segundos = 10000 ms)
unsigned long tiempoEncendido = 0;
const unsigned long DURACION_AUTOAPAGADO = 10000; 

// Genera la página web con un solo botón (Reto 2)
void mostrarPagina() {
  String pagina = R"HTML(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Control LED ESP32 - Retos</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin: 40px; background: #f5f7fb; }
    button { font-size: 22px; padding: 18px 30px; margin: 15px; cursor: pointer; background-color: #007bff; color: white; border: none; border-radius: 8px; }
    button:active { background-color: #0056b3; }
  </style>
</head>
<body>
  <h1>Control Inteligente de LED (Grupo 01)</h1>
  <p>Estado actual: <strong>%ESTADO%</strong></p>
  
  <!-- Reto 2: Ruta para alternar con un solo botón -->
  <form action="/alternar" method="POST">
    <button type="submit">Cambiar estado</button>
  </form>
</body>
</html>
)HTML";

  pagina.replace("%ESTADO%", ledEncendido ? "ENCENDIDO (Auto-apagado en curso)" : "APAGADO");
  servidor.send(200, "text/html; charset=utf-8", pagina);
}

// Función que alterna el estado y gestiona el tiempo
void alternarLED() {
  ledEncendido = !ledEncendido; // Invierte el estado actual

  if (ledEncendido) {
    digitalWrite(PIN_LED, HIGH);
    tiempoEncendido = millis(); // Reto 3: Marca el inicio del cronómetro
    Serial.println("Orden recibida: LED ENCENDIDO (Inicia temporizador de 10s)");
  } else {
    digitalWrite(PIN_LED, LOW);
    Serial.println("Orden recibida: LED APAGADO manualmente");
  }

  servidor.sendHeader("Location", "/");
  servidor.send(303, "text/plain", "");
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  WiFi.mode(WIFI_AP);
  bool redCreada = WiFi.softAP(NOMBRE_RED, CLAVE_RED);

  if (!redCreada) {
    Serial.println("ERROR: No se pudo crear la red Wi-Fi");
    while (true) { delay(1000); }
  }

  Serial.println("Red Wi-Fi creada correctamente");
  Serial.print("Nombre: ");
  Serial.println(NOMBRE_RED);
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.softAPIP());

  // Rutas del servidor web
  servidor.on("/", HTTP_GET, mostrarPagina);
  servidor.on("/alternar", HTTP_POST, alternarLED);
  
  servidor.begin();
  Serial.println("Servidor HTTP listo");
}

void loop() {
  servidor.handleClient();

  // Reto 3: Autoapagado automático a los 10 segundos sin bloquear la simulación
  if (ledEncendido && (millis() - tiempoEncendido >= DURACION_AUTOAPAGADO)) {
    digitalWrite(PIN_LED, LOW);
    ledEncendido = false;
    Serial.println("Tiempo cumplido (10s): LED apagado automáticamente.");
  }
}
