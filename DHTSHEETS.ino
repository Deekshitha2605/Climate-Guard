#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

#define DHTPIN D2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "AP_ZTE 5GHz";
const char *password = "aP@zt1234";
const char *host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client;
const char *fingerprint = "E5 8C 1C C4 91 3B 38 63 4B E9 10 6E E3 AD 8E 6B 9D D9 81 4A";
String GAS_ID = "AKfycby8orKZaL-LuhV3gC56WPOpiooXx9v7jMTindLAWuvQjbPTJukz2U2ZFfvWNvLP_pkP";
int it;
int ih;

void setup()
{
  dht.begin();
  Serial.begin(9600);
  Serial.println();

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Temp = ");
  Serial.print(t);
  Serial.print(" HUM = ");
  Serial.println(h);

  it = (int)t;
  ih = (int)h;
  sendData(it, ih);

  delay(4000);
}

void sendData(int tem, int hum)
{
  client.setInsecure();
  Serial.print("Connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort))
  {
    Serial.println("Connection failed");
    return;
  }

  if (client.verify(fingerprint, host))
  {
    Serial.println("Certificate matches");
  }
  else
  {
    Serial.println("Certificate doesn't match");
  }

  String string_temperature = String(tem, DEC);
  String string_humidity = String(hum, DEC);
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature + "&humidity=" + string_humidity;
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Request sent");

  while (client.connected())
  {
    String line = client.readStringUntil('\n');
    if (line == "\r")
    {
      Serial.println("Headers received");
      break;
    }
  }

  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\""))
  {
    Serial.println("ESP8266/Arduino CI successful!");
  }
  else
  {
    Serial.println("ESP8266/Arduino CI has failed");
  }

  Serial.println("Reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");

  Serial.println("Closing connection");
  client.stop();
}

