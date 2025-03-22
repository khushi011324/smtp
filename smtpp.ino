#include <WiFiNINA.h>
#include <Wire.h>
#include <BH1750.h>
#include <ArduinoHttpClient.h>

// WiFi Credentials
const char* ssid = "Khushi";
const char* password = "12345671";

// SMTP Server Details
const char* smtpServer = "smtp.gmail.com";
const int smtpPort = 587; // Use 465 for SSL, 587 for TLS
const char* senderEmail = "khushi131721@gmail.com";
const char* senderPassword = "xqgq lohj rxtw rrhu"; // Use App Password
const char* recipientEmail = "harshranaharsh365@gmail.com";

// Initialize BH1750 Light Sensor
BH1750 lightMeter;

// WiFi and SMTP Clients
WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, smtpServer, smtpPort);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize BH1750 Sensor
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Error initializing BH1750!");
    while (1);
  }

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

void sendEmail(float lux) {
  Serial.println("Connecting to SMTP Server...");

  // Email message body
  String emailBody = "Subject: BH1750 Light Sensor Data\r\n";
  emailBody += "From: " + String(senderEmail) + "\r\n";
  emailBody += "To: " + String(recipientEmail) + "\r\n";
  emailBody += "Content-Type: text/plain\r\n";
  emailBody += "Current Light Intensity: " + String(lux) + " lx\r\n\r\n";

  // Connect to SMTP server
  if (client.connect(smtpServer, smtpPort)) {
    Serial.println("Connected to SMTP Server");

    // Send EHLO command
    client.println("EHLO arduino");
    delay(500);

    // Start TLS
    client.println("STARTTLS");
    delay(500);

    // Send AUTH LOGIN
    client.println("AUTH LOGIN");
    delay(500);

    // Send base64 encoded sender email
    client.println("a2h1c2hpMTMxNzIxQGdtYWlsLmNvbQ=="); // Base64 of sender email
    delay(500);

    // Send base64 encoded sender password
    client.println("eGdnIExvaGogcnh0dyBycmh1"); // Base64 of App Password
    delay(500);

    // Send MAIL FROM command
    client.println("MAIL FROM: <" + String(senderEmail) + ">");
    delay(500);

    // Send RCPT TO command
    client.println("RCPT TO: <" + String(recipientEmail) + ">");
    delay(500);

    // Send DATA command
    client.println("DATA");
    delay(500);

    // Send email body
    client.println(emailBody);
    client.println(".");
    delay(500);

    Serial.println("✅ Email Sent Successfully!");
  } else {
    Serial.println("❌ Failed to Connect to SMTP Server!");
  }

  client.stop();
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Light Intensity: ");
  Serial.print(lux);
  Serial.println(" lx");

  sendEmail(lux);  // Send email with sensor data
  delay(60000);    // Send email every 60 seconds
}
