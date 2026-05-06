#include <WiFi.h>
#include <WiFiUdp.h>

// --- Configuración ---
const char* ssid = "Dickphone";
const char* password = "Cyberdick_12";
const char* PC_IP = "10.16.104.115"; // IP del PC
const int UDP_PORT = 5005;

WiFiUDP udp;
bool carPaused = false;

void setup() {
  Serial.begin(115200);   // UART con MaixCam
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  udp.begin(UDP_PORT);
}

void loop() {
  // 1. Leer detección desde MaixCam por UART
  if (Serial.available() && !carPaused) {
    String objeto = Serial.readStringUntil('\n');
    objeto.trim();

    if (objeto.length() > 0) {
      // 2. Pausar el carro
      carPaused = true;
      pausarCarro();

      // 3. Enviar código al PC por WiFi
      udp.beginPacket(PC_IP, UDP_PORT);
      udp.print(objeto);
      udp.endPacket();
    }
  }

  // 4. Escuchar respuesta "RESUME" del PC
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char buffer[32];
    udp.read(buffer, sizeof(buffer));
    String msg = String(buffer);
    if (msg.indexOf("RESUME") >= 0) {
      carPaused = false;
      reanudarCarro();
    }
  }
}

void pausarCarro() {
  // Tu lógica para detener los motores
  // Ejemplo: digitalWrite(MOTOR_EN, LOW);
}

void reanudarCarro() {
  // Tu lógica para reanudar seguidor de línea
  // Ejemplo: digitalWrite(MOTOR_EN, HIGH);
}