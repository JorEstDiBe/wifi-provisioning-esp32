#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

// ==========================
// Configuración general
// ==========================

#define RESET_BUTTON_PIN 27
#define RESET_HOLD_TIME_MS 4000

const byte DNS_PORT = 53;

const char* AP_PASSWORD = "config1234";  // Mínimo 8 caracteres para WPA/WPA2

WebServer server(80);
DNSServer dnsServer;
Preferences preferences;

String apSSID;
bool configMode = false;

unsigned long buttonPressStart = 0;
unsigned long lastReconnectAttempt = 0;

// ==========================
// Funciones auxiliares
// ==========================

String getChipId() {
  uint64_t chipid = ESP.getEfuseMac();
  String id = String((uint32_t)(chipid & 0xFFFF), HEX);
  id.toUpperCase();
  return id;
}

String getAPName() {
  return "ESP32-SETUP-" + getChipId();
}

bool loadCredentials(String &ssid, String &password) {
  preferences.begin("wifi", true);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  preferences.end();

  return ssid.length() > 0;
}

void saveCredentials(const String &ssid, const String &password) {
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
}

void clearCredentials() {
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();
}

String getStatusJson() {
  String mode = configMode ? "AP_CONFIG" : "STA";
  bool connected = WiFi.status() == WL_CONNECTED;

  String ssid;
  String ip;

  if (configMode) {
    ssid = apSSID;
    ip = WiFi.softAPIP().toString();
  } else {
    ssid = WiFi.SSID();
    ip = connected ? WiFi.localIP().toString() : "0.0.0.0";
  }

  String json = "{";
  json += "\"mode\":\"" + mode + "\",";
  json += "\"connected\":" + String(connected ? "true" : "false") + ",";
  json += "\"ssid\":\"" + ssid + "\",";
  json += "\"ip\":\"" + ip + "\",";
  json += "\"freeHeap\":" + String(ESP.getFreeHeap());
  json += "}";

  return json;
}

// ==========================
// HTML del portal
// ==========================

String getConfigPage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 WiFi Provisioning</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background: #f4f6f8;
      margin: 0;
      padding: 0;
      color: #222;
    }

    .container {
      max-width: 420px;
      margin: 50px auto;
      background: white;
      padding: 28px;
      border-radius: 14px;
      box-shadow: 0 4px 18px rgba(0,0,0,0.12);
    }

    h1 {
      font-size: 24px;
      margin-bottom: 8px;
      text-align: center;
    }

    p {
      text-align: center;
      color: #555;
      font-size: 14px;
      margin-bottom: 24px;
    }

    label {
      font-weight: bold;
      display: block;
      margin-top: 16px;
      margin-bottom: 6px;
    }

    input {
      width: 100%;
      padding: 12px;
      border: 1px solid #ccc;
      border-radius: 8px;
      box-sizing: border-box;
      font-size: 15px;
    }

    button {
      width: 100%;
      margin-top: 24px;
      padding: 13px;
      background: #0d6efd;
      color: white;
      border: none;
      border-radius: 8px;
      font-size: 16px;
      font-weight: bold;
      cursor: pointer;
    }

    button:hover {
      background: #084dbf;
    }

    .footer {
      margin-top: 20px;
      text-align: center;
      font-size: 12px;
      color: #777;
    }

    .link {
      margin-top: 18px;
      text-align: center;
    }

    .link a {
      color: #0d6efd;
      text-decoration: none;
      font-size: 14px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Configuración WiFi ESP32</h1>
    <p>Ingrese las credenciales de la red WiFi a la que desea conectar el dispositivo.</p>

    <form action="/save" method="POST">
      <label for="ssid">SSID</label>
      <input type="text" id="ssid" name="ssid" placeholder="Nombre de la red WiFi" required>

      <label for="password">Contraseña</label>
      <input type="password" id="password" name="password" placeholder="Contraseña de la red">

      <button type="submit">Guardar credenciales</button>
    </form>

    <div class="link">
      <a href="/api/status">Ver estado del dispositivo</a>
    </div>

    <div class="footer">
      ESP32 WiFi Provisioning Lab
    </div>
  </div>
</body>
</html>
)rawliteral";

  return html;
}

// ==========================
// Handlers HTTP
// ==========================

void handleRoot() {
  server.send(200, "text/html", getConfigPage());
}

void handleSave() {
  if (!server.hasArg("ssid")) {
    server.send(400, "text/html", "<h2>Error</h2><p>No se recibió el SSID.</p>");
    return;
  }

  String ssid = server.arg("ssid");
  String password = server.arg("password");

  ssid.trim();

  if (ssid.length() == 0) {
    server.send(400, "text/html", "<h2>Error</h2><p>El SSID no puede estar vacío.</p>");
    return;
  }

  saveCredentials(ssid, password);

  String response = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <title>Credenciales guardadas</title>
</head>
<body>
  <h2>Credenciales guardadas</h2>
  <p>El ESP32 se reiniciará e intentará conectarse a la red configurada.</p>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", response);

  delay(1500);
  ESP.restart();
}

void handleStatus() {
  server.send(200, "application/json", getStatusJson());
}

void handleReset() {
  clearCredentials();

  String response = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <title>Configuración borrada</title>
</head>
<body>
  <h2>Configuración borrada</h2>
  <p>El ESP32 se reiniciará en modo configuración.</p>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", response);

  delay(1500);
  ESP.restart();
}

void redirectToPortal() {
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.send(302, "text/plain", "");
}

void handleNotFound() {
  if (configMode) {
    redirectToPortal();
  } else {
    server.send(404, "application/json", "{\"error\":\"Endpoint no encontrado\"}");
  }
}

// ==========================
// Configuración del servidor
// ==========================

void setupServerRoutes() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/reset", HTTP_POST, handleReset);

  // Endpoints típicos de detección de portal cautivo
  server.on("/generate_204", HTTP_GET, redirectToPortal);        // Android
  server.on("/hotspot-detect.html", HTTP_GET, redirectToPortal); // iOS/macOS
  server.on("/fwlink", HTTP_GET, redirectToPortal);              // Windows

  server.onNotFound(handleNotFound);
}

// ==========================
// Modos de operación
// ==========================

bool connectToWiFi(const String &ssid, const String &password) {
  Serial.println();
  Serial.println("Intentando conexión WiFi...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startAttempt = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conexión exitosa.");
    Serial.print("IP asignada: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.println("No fue posible conectarse a la red configurada.");
  return false;
}

void startConfigMode() {
  configMode = true;
  apSSID = getAPName();

  Serial.println();
  Serial.println("Iniciando modo configuración...");
  Serial.print("Red AP: ");
  Serial.println(apSSID);
  Serial.print("Contraseña AP: ");
  Serial.println(AP_PASSWORD);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID.c_str(), AP_PASSWORD);

  delay(500);

  IPAddress apIP = WiFi.softAPIP();

  Serial.print("IP del portal: ");
  Serial.println(apIP);

  dnsServer.start(DNS_PORT, "*", apIP);

  setupServerRoutes();
  server.begin();

  Serial.println("Servidor web iniciado en modo AP.");
}

void startOperationMode() {
  configMode = false;

  setupServerRoutes();
  server.begin();

  Serial.println("Servidor web iniciado en modo Station.");
  Serial.print("Consultar estado en: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/api/status");
}

// ==========================
// Botón de reset
// ==========================

void checkResetButton() {
  int buttonState = digitalRead(RESET_BUTTON_PIN);

  if (buttonState == LOW) {
    if (buttonPressStart == 0) {
      buttonPressStart = millis();
    }

    if (millis() - buttonPressStart >= RESET_HOLD_TIME_MS) {
      Serial.println("Botón presionado durante 4 segundos.");
      Serial.println("Borrando credenciales...");

      clearCredentials();

      delay(500);
      ESP.restart();
    }
  } else {
    buttonPressStart = 0;
  }
}

// ==========================
// Reconexión automática
// ==========================

void checkWiFiReconnect() {
  if (configMode) {
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();

    if (now - lastReconnectAttempt >= 10000) {
      lastReconnectAttempt = now;

      Serial.println("Conexión WiFi perdida. Intentando reconectar...");
      WiFi.reconnect();
    }
  }
}

// ==========================
// Setup y loop
// ==========================

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

  Serial.println();
  Serial.println("====================================");
  Serial.println("ESP32 WiFi Provisioning Lab");
  Serial.println("====================================");

  String ssid;
  String password;

  bool hasCredentials = loadCredentials(ssid, password);

  if (hasCredentials) {
    Serial.println("Credenciales encontradas en memoria NVS.");

    bool connected = connectToWiFi(ssid, password);

    if (connected) {
      startOperationMode();
    } else {
      Serial.println("Entrando en modo configuración por fallo de conexión.");
      startConfigMode();
    }
  } else {
    Serial.println("No hay credenciales guardadas.");
    startConfigMode();
  }
}

void loop() {
  if (configMode) {
    dnsServer.processNextRequest();
  }

  server.handleClient();

  checkResetButton();
  checkWiFiReconnect();
}
