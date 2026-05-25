#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid        = "Aadi";
const char* password    = "123456789";
const char* mqtt_server = "10.200.155.193"; 
const int   mqtt_port      = 8883;

// ✅ UNIQUE ID: So it doesn't fight with ESP1
const char* mqtt_client_id = "esp32-client-2";

// ✅ NEW TOPICS: For Zone 2
const char* topic_temp     = "ot/level1/sensor/esp32-2/temperature";
const char* topic_hum      = "ot/level1/sensor/esp32-2/humidity";

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ✅ YOUR MASTER ROOT CA
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

// ✅ YOUR DHT2 CLIENT CERTIFICATE
const char* client_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDSDCCAjACFFShmEgw3e4FXtZG81Au62NQT+j0MA0GCSqGSIb3DQEBCwUAMGIx
CzAJBgNVBAYTAklOMRQwEgYDVQQIDAtNYWhhcmFzaHRyYTENMAsGA1UEBwwEUHVu
ZTETMBEGA1UECgwKRmFjdG9yeSBJVDEZMBcGA1UEAwwQWmVyb1RydXN0LVJvb3RD
QTAeFw0yNjAzMjcxMDMzMzhaFw0yNzAzMjcxMDMzMzhaMF8xCzAJBgNVBAYTAklO
MRQwEgYDVQQIDAtNYWhhcmFzaHRyYTENMAsGA1UEBwwEUHVuZTERMA8GA1UECgwI
T1QgRmxvb3IxGDAWBgNVBAMMD0VTUDMyLURIVC1Ob2RlMjCCASIwDQYJKoZIhvcN
AQEBBQADggEPADCCAQoCggEBAMgnMOXzfBWt7oIQdrQI0KlUL0NKcF3QZQ2oeCWV
1XTobXTGdu2AUsdD5ej61MULscp4UcUb9tELmS/3w7MGwHkmD7/NLTe4uM0eYR7x
0Dkmotwz3KQyCQgFaGlzdCpjwVnEwl1sdgpr1dtAFLPcN+d8xwIF0T1BQAacPLis
wn8K6CTKVR3bmxQucpg2H4Ktv0m6fuf9tyXaXsnzuH62yfGGJwvZ3gNoK+DuZgtj
Nb4L3FGqbW1ldUKjzNcbb5VL72b8jV5gM4CBJapgyxJ83nGI3Rd/RfCKZ8SVROOT
JPZHsKj21E08agjVv7+o5WkcF7ptXiIjO4RitCo6tyydOJMCAwEAATANBgkqhkiG
9w0BAQsFAAOCAQEAnv06i7nBqLNIFmxVqX81vOqOp1lN68tOgHf8ivE917y3tA6Z
vBBmH7J5IyTnNQnLv1WQsj33JCFAx3bFAcITToI7nel/38IlOdzjOkDRGQY/5wzM
r57uhWmhwOg5jOe1fw//Rb6AVCy14/rP2Zjj5CSYVsNeLw+TgvcIZeUoa+vOrP83
XyAGSDq9ZoSm4cW/q0hpGeyI+SuzogGDi4OlAy66a53lpx/AkbX3R6QeoN7vSnTV
ihtwBqmbOYNih3jBOpqhxNLe1syGTFc+3ZkWk5t9I8ed50QDbxzLhVgmoxsev3UA
gTleTZeeNiB8n71i2WA8wpGAGuyCiTrUbhonCw==
-----END CERTIFICATE-----
)EOF";

// ✅ YOUR DHT2 PRIVATE KEY
const char* client_key = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAyCcw5fN8Fa3ughB2tAjQqVQvQ0pwXdBlDah4JZXVdOhtdMZ2
7YBSx0Pl6PrUxQuxynhRxRv20QuZL/fDswbAeSYPv80tN7i4zR5hHvHQOSai3DPc
pDIJCAVoaXN0KmPBWcTCXWx2CmvV20AUs9w353zHAgXRPUFABpw8uKzCfwroJMpV
HdubFC5ymDYfgq2/Sbp+5/23JdpeyfO4frbJ8YYnC9neA2gr4O5mC2M1vgvcUapt
bWV1QqPM1xtvlUvvZvyNXmAzgIElqmDLEnzecYjdF39F8IpnxJVE45Mk9kewqPbU
TTxqCNW/v6jlaRwXum1eIiM7hGK0Kjq3LJ04kwIDAQABAoIBAQCutO7MuWSlM8YN
h7tlWJByTUZX8UD10nJyoowFGq1Qt5TbKmUi2DDpOE4Vt+RC/vs4pR+S8t7VB8+1
/Am5WfplhSCYJcsDaVz9xje2IBLF4ACmPyqns/kIJFhIO3u5iIt8BPA3RfYfEJOC
JoQrnty5VcQL8vH6CtuNKZEjDcFTy7B3nyjJEazM1eBZktOFMpxU8nKSqEt14AYt
Ou89y/5qht6N78lD5tZzUTqX+mT/f9MapuHFsKMNUjPjW0UBryPSPyHIqEoe1s1R
S9OfLqwPD4vNSlgrG9Lq0jPdeOXWyYjCzL2segIHelKqDBnOfZjsnL5RinDLGDUG
DDV0BEMJAoGBAPDvpd3Jq+glqhs+2UdTX/D4Rj2SUJ/J/6ikNR9fSsHk1FbVky66
mX6Zv2+sfukRt1V/eJLFEJPBXaDIB9LGiskHJKzZ3z/D/A7MX1x1dZ/tf7dcTeMz
Fa3sXhmfe+eiPLFEZYHU9k45BpIlCJnQ/dQli8Bi49ZQy8wXSN5G451fAoGBANSq
yC2YdPc03znR9AueDKncescS1OfuOrPwp9AovPjj1djDzh6GMTjiNT3MHvNhnTj0
wr+DZ8j1hN9MhP0IO4G5OtOZTKE8m4SbIg5hTUZElIIUr5v4c6Eibjpcr+qSMSMB
JDGjpxraFkvb1Zlo31d9kT48zG/E9tSbJPScYP1NAoGBAOOFa1zF1VCkQL752rVA
RpmH2szKHtjGKGdYqdG2ayIutVZK2XKHQ+wJxH3wpufxku9qMLJo5f6h24i5XKnu
U6EC3d9c3zWYHa7b0DG/uhQMc070G5kx5IcZbsNDl/5ldmrv6+tu8CluMIZR8o1O
jmSIEDDdMcpxAgL4+hbtWoSBAoGALQszNP0aoRWQxlVi5GcIdZ3eX3XspGdABcbO
NHe8dtcC/1ORv8Naf5rh/gHS+GfPfjXRXfD+NYFTMFyrulBZEkPiYtkQcwI8Slz1
XKhdTqnN7rNk8pswGowJXtEs7XkjcS+Xs+/ewAA7QrwAROsCiyxsbAtCndXRFfz6
WdkZbe0CgYA4tT4phUHVk3xHgzulomxlmjvDfd5tihqAXgHn8CMHRXrimORK94DN
vJd62QwYg3CzxewcoeYP/3xI5iK55R2m+HItzoJyCjfLxA8kneux7KfTP3FMwmjX
Ox0KGt+6dc6ZeMJmu9Vt7A07KaBc7Fh6yx7d+sxsDUwmnSCDGncaxg==
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

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("connected ✅");
    } else {
      Serial.printf("failed rc=%d, retry in 5s\n", client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Waking up ESP2 DHT22...");
  delay(2000);
  dht.begin();
  
  connectWiFi();
  setClock();

  // Load Certificates for mTLS
  espClient.setCACert(root_ca);
  espClient.setCertificate(client_cert);
  espClient.setPrivateKey(client_key);
  espClient.setHandshakeTimeout(60);

  client.setServer(mqtt_server, mqtt_port);
  connectMQTT();
}

void loop() {
  if (!client.connected()) connectMQTT();
  client.loop();

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT22 read failed");
  } else {
    char t[8], h[8];
    dtostrf(temp, 4, 2, t);
    dtostrf(hum,  4, 2, h);
    client.publish(topic_temp, t);
    client.publish(topic_hum,  h);
    Serial.printf("ESP2 Temp: %s°C  Hum: %s%%\n", t, h);
  }
  delay(5000);
}
