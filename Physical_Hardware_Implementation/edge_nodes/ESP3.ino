#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

const char* ssid        = "Aadi";
const char* password    = "123456789";
const char* mqtt_server = "10.200.155.193"; 
const int   mqtt_port      = 8883;

// ✅ UNIQUE ID: So it doesn't fight with the sensors
const char* mqtt_client_id = "esp32-servo-actuator";

// ✅ THE COMMAND TOPIC: The board will listen to this specific channel
const char* topic_servo    = "ot/level1/actuator/esp32-3/servo";

Servo myServo;
const int servoPin = 18; // Connect the yellow servo wire here

// ✅ 1. YOUR MASTER CA CERTIFICATE
const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDpTCCAo2gAwIBAgIUC65qc2T7ywdDlL/1eKklTk+t00IwDQYJKoZIhvcNAQEL
BQAwYjELMAkGA1UEBhMCSU4xFDASBgNVBAgMC01haGFyYXNodHJhMQ0wCwYDVQQH
DARQdW5lMRMwEQYDVQQKDApGYWN0b3J5IElUMRkwFwYDVQQDDBBaZXJvVHJ1c3Qt
Um9vdENBMB4XDTI2MDMyNzEwMzMzMVoXDTI3MDMyNzEwMzMzMVowYjELMAkGA1UE
BhMCSU4xFDASBgNVBAgMC01haGFyYXNodHJhMQ0wCwYDVQQHDARQdW5lMRMwEQYD
VQQKDApGYWN0b3J5IElUMRkwFwYDVQQDDBBaZXJvVHJ1c3QtUm9vdENBMIIBIjAN
BgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAv3A24JQbn3vLd/08NGz3SjCFcFBP
uiQcFTlEImAyRfvAV7qzwnp9VFhqBqZsetqdx18G8Hr75lLFSjFM/LJAkegqjz8P
Eg2OOlQXnN3QuIdShpT/5kcia60aQGJ4pxV/eZjOcXf16kcxOnlzEnmAI+zl0Doy
CI2zpnSPDDdxOLtB6bORLBvqMh/fWQ8bOdnBCKF7m+pNww04iJ8LKLvSbyXWrsJp
myWTr9bXy7gZFL2PRaFKopKt5yVw6K/4g1dPpPsqecuLDYdVslUhwZ2WGTFBtJsw
Oz99HmTj8p4ogpY9VklN1S0nq5eFvFPCTivhHbWBwbt76FH0Em+uXO+wrwIDAQAB
o1MwUTAdBgNVHQ4EFgQU5Q91r2UHssidVD5lpxHSmrbSOyowHwYDVR0jBBgwFoAU
5Q91r2UHssidVD5lpxHSmrbSOyowDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0B
AQsFAAOCAQEAJA/FzmUmCcoHG4oz0YqN74BSbAetiAFtmPwlJVZmVsCpKC/J53jB
q1yNe6O9kKvAqr6wrAilslzzStix3/WkGoslCLSoVoqym0LclPA6fTdpaJhsNbwR
KIxke8YA1GcOLSPbOUW4K6AYnmj2ABNgj+ZLQ6pPbdRnvqMGlEwsruZOF8PcNcw2
hrjzvXpD3R+QLu22lMQWPIhed9M8WfBcOdcbAsvqcZv8s+lsOH0gc5bnEpqATbz8
AJ9Ag0BPmf42o9gmcRJbNePpOmJXuBHxu6hPRsHx4ruFLIOdYppYM/RFR3xbAr8k
I6xF7goL4rUe55c1NbYnsH534yg2D4r08Q==
-----END CERTIFICATE-----
)EOF";

// ✅ 2. YOUR SERVO CLIENT CERTIFICATE
const char* client_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDTzCCAjcCFFShmEgw3e4FXtZG81Au62NQT+j1MA0GCSqGSIb3DQEBCwUAMGIx
CzAJBgNVBAYTAklOMRQwEgYDVQQIDAtNYWhhcmFzaHRyYTENMAsGA1UEBwwEUHVu
ZTETMBEGA1UECgwKRmFjdG9yeSBJVDEZMBcGA1UEAwwQWmVyb1RydXN0LVJvb3RD
QTAeFw0yNjAzMjcxMDMzNTRaFw0yNzAzMjcxMDMzNTRaMGYxCzAJBgNVBAYTAklO
MRQwEgYDVQQIDAtNYWhhcmFzaHRyYTENMAsGA1UEBwwEUHVuZTERMA8GA1UECgwI
T1QgRmxvb3IxHzAdBgNVBAMMFkVTUDMyLVNlcnZvLUNvbnRyb2xsZXIwggEiMA0G
CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDce4Bj7I2jih05Xx8D1MNpXALeOdh0
/iE/6ZTrmgXIfth71v7OXozmSU26aVHvcQaZw2rVHWt3BwAovJtMIgPcSVQ9eRko
VXA1/ZUGfF3Hanax5ydPxjzIOWKYhSxPlfybT/vXDZr5Ltz4SXgArWn9cirAjXgM
k0BzGos8ZqRJ3K1JsbzlQfBmE5VNUQ9TXTXCWYjyjPwKJE0GzZ1U5qlvewfbU018
AqKf2pwxP1LWZqKk+/A+UYH9zlbCVHZknNtrYf1INuZiy0jsfJw4aOtXbQ/pGgJq
FZvZ6HhnYPGphXTBEUiYSpUDZGcqhnkcLN3zDsz7En5Go5BMksVmF875AgMBAAEw
DQYJKoZIhvcNAQELBQADggEBAK5X9CfeQf876NKsW8rsMvQstFZc9MQj3a4GeK1K
tw/KfKjhLhdxXIlQ7sOSj+kEZ1HF34yVOHYhvlZusksZHlp2TP3WDdZ+tUpbcPc7
KRYQSlTtVwFDlXEruGdizd0FzV9TrmNyHi66t7H2GXUYp4UuT3bQ0iDw/JNTydt+
o4ySjsYVRcwuMljSWajhq1SClOB7z3XTeFO0IrhXHOYl1nHbazbbhEtTKRfRCXQY
GIykk39DVIC69/QY95Z7iAPLsEUoZQRZAiG/tUJAGsHvbWu6dUMit/jomTQ12mPZ
rYIHi1LZOSSiuUfJeiIF6RPXVVx7qGJ6gVUZ/057U8mhQQk=
-----END CERTIFICATE-----
)EOF";

// ✅ 3. YOUR SERVO PRIVATE KEY
const char* client_key = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEA3HuAY+yNo4odOV8fA9TDaVwC3jnYdP4hP+mU65oFyH7Ye9b+
zl6M5klNumlR73EGmcNq1R1rdwcAKLybTCID3ElUPXkZKFVwNf2VBnxdx2p2secn
T8Y8yDlimIUsT5X8m0/71w2a+S7c+El4AK1p/XIqwI14DJNAcxqLPGakSdytSbG8
5UHwZhOVTVEPU101wlmI8oz8CiRNBs2dVOapb3sH21NNfAKin9qcMT9S1maipPvw
PlGB/c5WwlR2ZJzba2H9SDbmYstI7HycOGjrV20P6RoCahWb2eh4Z2DxqYV0wRFI
mEqVA2RnKoZ5HCzd8w7M+xJ+RqOQTJLFZhfO+QIDAQABAoIBADwmOS2JgE6IRFa9
e4GZDsJ0hpfHoiSwm7OK7sSjnUZITJjkW1lDdB7zCpB7YQ34gLt8E24We5XaRBHY
+mj3sqPrIV8v5UwamuUvy/McBJ19juoTzKa3xyc8eIhxdFg169cXJ/ZQUOIABep6
aJGOvaaV+dHUBlcv6No9+JQ+0D0RxAZXC+MsyZwINrTG8gieHMWPzutf/r0mrRFQ
DtmZJ1BHE/QFgj0+M/IdMu3r55Pk3Xemq+A8brUGbYkJJF58Q5Y6Ra/ksrkkcBxZ
qGl2Gwh46+6CMBGNQfgOdHeHFPuvpjCsg56gOyBafR3YaOhxfqyIiLcCdNIbSmPY
ywUV0m0CgYEA/tAzNfM1bXoDktaAro52XKjraV0xjk0fBIYOXBDdDp2DBJQTDTO3
XC+u6uyLMrBiL43azSrpCoJkZbrbFe7EUNXAdaxip974h37gTEmJZR1F8R7EwfBd
5XS/giUu08xMZzJBT5oLEAFngin5Ii7rABQC0SIKmyWwzIWJaounoW8CgYEA3YJe
5PUadfAWq+UOkNtZ3N6oqitYKZSQpEx/RxWAnwYNxglqzUesCWTbZNIALZxabUUI
UMgd+5s96yM+BVtypmmeNYS9/7AQppWE/ahN5fsYS3yJ/iubDcMRnYb5TtYu9dAu
MsQlWVP7IGtGRCSe/gtjXanvVV4k1FA4HvX5khcCgYEA1ZSFmC7nFYA0Bpws+CMJ
KtmbmJE4vGIAIbaLyreH0nxMGUZUQbibdHWzU845Tp9xbRKl11qkQt7QcESS5mtf
ubGww/FeL/9AqbG7J7Zrp4lF2rlDipuC2JVnABtSiMyxLb59587ifuPucrfjzrcs
2r/rLIgwA6Mv8f3sFJZCR5kCgYBr83RRus3JeldiMIBXwj7aMD0cqYEOypNvLsS8
nNMRLP8ba018e5hw/5EYYmupztAW/pSd4dkS3MZhSMyaCcYnKcPrqyR5bpG+zZGB
mTtVgHiHmUsa7p9jJpT4I2ZusF00eN966DKXhSEW3JDi9sHxZxVm+UFlZjsmqW2g
URnuwwKBgG37tN/TykkOUtevVGoyMQ/hD8XOjeBA9QQBI4X/3ac4EDmpMTacUXet
+4ZS/FrzAkEwGbdpJv+DUzC35QxUmEWTn0F69fmC4ftovRsMBGyHK4/89EXShaxE
9+PcrsqJ2YU1Z9H316zqKbtAUXYsYymlW30ljhzbcwNoGBj+RF9w
-----END RSA PRIVATE KEY-----
)EOF";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
}

void setClock() {
  Serial.print("Setting manual time for Air-Gapped Network...");
  
  // Hardcode the Unix Epoch time to April 2026 (1775088000)
  struct timeval tv;
  tv.tv_sec = 1775088000; 
  tv.tv_usec = 0;
  settimeofday(&tv, NULL);
  
  Serial.println("\nTime successfully set to 2026!");
}

// ✅ NEW: This function triggers the physical motor when a message arrives
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Command received on [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();

  // If the gateway says "OPEN", swing the arm
  if (messageTemp == "OPEN") {
    Serial.println("Action: Opening Servo (90 degrees)");
    myServo.write(90);
  } 
  // If the gateway says "CLOSE", return the arm
  else if (messageTemp == "CLOSE") {
    Serial.println("Action: Closing Servo (0 degrees)");
    myServo.write(0);
  }
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("connected ✅");
      // Tell the broker we want to subscribe to the servo command topic
      client.subscribe(topic_servo);
    } else {
      Serial.printf("failed rc=%d, retry in 5s\n", client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize the Servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2400);
  myServo.write(0); // Start at 0 degrees (Closed)

  connectWiFi();
  setClock();

  // Load Certificates
  espClient.setCACert(root_ca);
  espClient.setCertificate(client_cert);
  espClient.setPrivateKey(client_key);
  espClient.setHandshakeTimeout(60);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); // Attach the listening function
  
  connectMQTT();
}

void loop() {
  if (!client.connected()) connectMQTT();
  client.loop(); // Needs to run continuously to listen for incoming messages
}
