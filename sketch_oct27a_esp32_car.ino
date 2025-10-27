#include <WiFi.h>
#include <WebServer.h>

// ===== MOTOR PINS =====
const int IN1 = 26; // Left motors
const int IN2 = 25;
const int IN3 = 33; // Right motors
const int IN4 = 32;

// ===== WiFi =====
const char* ssid = "HS";          
const char* password = "2025@#$%1";

WebServer server(80);

// ===== MOTOR FUNCTIONS =====
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Stop");
}

void forward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  Serial.println("Forward");
  server.send(200, "text/plain", "Forward");
}

void backward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  Serial.println("Backward");
  server.send(200, "text/plain", "Backward");
}

void left() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  Serial.println("Left");
  server.send(200, "text/plain", "Left");
}

void right() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  Serial.println("Right");
  server.send(200, "text/plain", "Right");
}

// ===== WEB PAGE =====
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>"
                "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                "<title>ESP32 Car Control</title>"
                "<style>button{width:120px;height:50px;font-size:18px;margin:10px;}</style>"
                "<script>"
                "function cmd(c){fetch('/'+c);}"
                "function btn(id,c){"
                "let b=document.getElementById(id);"
                "b.onmousedown=()=>cmd(c); b.onmouseup=()=>cmd('S');"
                "b.ontouchstart=()=>cmd(c); b.ontouchend=()=>cmd('S');"
                "}"
                "window.onload=()=>{"
                "btn('F','F'); btn('B','B'); btn('L','L'); btn('R','R');"
                "};"
                "</script></head><body>"
                "<h2>ESP32 Car Control</h2>"
                "<p>WiFi Status: " + String(WiFi.status()==WL_CONNECTED?"Connected":"Not Connected") + "</p>"
                "<p>IP Address: " + WiFi.localIP().toString() + "</p>"
                "<button id='F'>FORWARD</button><br>"
                "<button id='L'>LEFT</button>"
                "<button id='R'>RIGHT</button><br>"
                "<button id='B'>BACKWARD</button>"
                "</body></html>";
  server.send(200, "text/html", html);
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  stopMotors(); // Safety

  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    attempt++;
    if(attempt>20){
      Serial.println("\nFailed to connect. Restart ESP32.");
      return;
    }
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // HTTP routes
  server.on("/", handleRoot);
  server.on("/F", forward);
  server.on("/B", backward);
  server.on("/L", left);
  server.on("/R", right);
  server.on("/S", [](){ stopMotors(); server.send(200,"text/plain","Stop"); });

  server.begin();
  Serial.println("Web Server Ready");
}

// ===== LOOP =====
void loop() {
  server.handleClient();
}
