#include <WiFi.h>
#include <WiFiUdp.h>

// --- Configuración ---
const char* ssid = "Dickphone";
const char* password = "Cyberdick_12";
const char* PC_IP = "---"; // Verifícala con 'ipconfig' en tu PC
const int UDP_PORT = 5005;

WiFiUDP udp;
bool carPaused = false;

void setup() {
  Serial.begin(115200);
  delay(1000); // Pequeña pausa para estabilizar
  
  Serial.println("\n--- Iniciando Sistema ---");
  
  // 1. Conexión WiFi con Feedback
  Serial.print("Conectando a: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[OK] WiFi Conectado!");
  Serial.print("IP de la ESP32: ");
  Serial.println(WiFi.localIP());
  Serial.println("--------------------------");

  udp.begin(UDP_PORT);
  Serial.print("Escuchando puerto UDP: ");
  Serial.println(UDP_PORT);
}

void loop() {
  // --- MODO TEST (Simular cámara desde el monitor serie) ---
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0 && !carPaused) {
      Serial.print("[TEST] Enviando manualmente: ");
      enviarDeteccion(input);
    }
  }

  // --- LÓGICA DE DETECCIÓN (UART MaixCam) ---
  // (Asumiendo que la MaixCam está en el Serial principal o en Serial2)
  // Si usas pines específicos para la cámara, usa Serial2.
  
  // --- ESCUCHAR RESPUESTA "RESUME" DEL PC ---
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char buffer[32];
    int len = udp.read(buffer, sizeof(buffer) - 1);
    buffer[len] = '\0'; // Asegurar fin de cadena
    
    String msg = String(buffer);
    Serial.print("[UDP] Mensaje recibido del PC: ");
    Serial.println(msg);

    if (msg.indexOf("RESUME") >= 0) {
      Serial.println("[✓] Comando RESUME recibido. Reanudando marcha.");
      carPaused = false;
      reanudarCarro();
    }
  }
}

void enviarDeteccion(String objeto) {
  carPaused = true;
  pausarCarro();
  
  Serial.print("[UDP] Enviando '");
  Serial.print(objeto);
  Serial.print("' a la IP: ");
  Serial.println(PC_IP);

  udp.beginPacket(PC_IP, UDP_PORT);
  udp.print(objeto);
  
  if (udp.endPacket()) {
    Serial.println("[OK] Paquete enviado correctamente.");
  } else {
    Serial.println("[ERROR] No se pudo enviar el paquete.");
  }
}

void pausarCarro() {
  Serial.println("[MOTOR] Carro DETENIDO");
  // Tu lógica real aquí
}

void reanudarCarro() {
  Serial.println("[MOTOR] Carro EN MARCHA");
  // Tu lógica real aquí
}
