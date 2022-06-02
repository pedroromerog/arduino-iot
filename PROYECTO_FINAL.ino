#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

const char *ssid = "Pedro Romero";
const char *password = "";
int dhtPort = 8;

const char *host = "192.168.43.128";
float tempMax = 29.20;
String estadoPuerta = "false";
WiFiClient client;
//DHT dht(dhtPort, DHT11);
HTTPClient http;

void setup()
{
  //ESP.wdtDisable();
  Serial.begin(115200);
  delay(1000);
  Serial.println(" Start.....");
 dht.begin();
  WiFi.begin(ssid, password);
  delay(1000);
 /* WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
*/
  Serial.println(" ");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
void sendEmail(String subject, String text)
{
  String Link = "http://api-iot-co.herokuapp.com/emails/send";
  http.begin(client, Link);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "subject=" + subject + "&text=" + text + "";
  int httpResponseCode = http.POST(httpRequestData);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  Serial.print(subject);
  Serial.println("Email Enviado!!!  ");
  http.end();
  delay(500);
}


void sendTemperature(String temp, String hum)
{
  String Link = "http://api-iot-co.herokuapp.com/temperatura";
  http.begin(client, Link);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "temperatura=" + temp + "&humedad=" + hum + "";
  int httpResponseCode = http.POST(httpRequestData);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  Serial.println(" Temperatura Enviada");
  http.end();
  delay(1000);
}
void getMaxTemp()
{
  String Link = "http://api-iot-co.herokuapp.com/temperatura/max";
  http.begin(client, Link); // Specify request destination

  int httpCode = http.GET();         // Send the request
  String payload = http.getString(); // Get the response payload
  if (payload)
  {
    float newTemp = payload.toFloat();
    if (tempMax != newTemp) {
      tempMax = newTemp;
      Serial.println("Nueva Temperatura Maxima");
      Serial.println(tempMax);
      sendEmail("Nueva Temperatura Maxima", "Se estableció una nueva temperatura maxima" + String(tempMax));
    }
  }
  http.end();  // Close connection
  delay(500); // GET Data at every 5 seconds
}

void getTemperature()
{
  Serial.println("TEMMMPERATURA");
  float humedad = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.print("ºC Humedad: ");
  Serial.print(humedad);
  Serial.println("%");
  delay(1000);

  // Comprobamos si ha habido algún error en la lectura
  if (isnan(humedad) || isnan(temp))
  {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
  else
  {
    sendTemperature(String(temp), String(humedad));
  }

  if (temp >= tempMax)
  {
    sendEmail("Ventilador Encendido", "Se encendió el sistema de ventilación");
    Serial.print("Ventilador activo. Temperatura: ");
    Serial.print(temp);
    Serial.print(" ºC ");
    delay(1000);
  }
  else
  {
    Serial.print("Ventilador Apagado. Temperatura: ");
    sendEmail("Ventilador Apagado", "Se apagó el sistema de ventilación");
    // digitalWrite(ventilador, LOW);
  }

  getMaxTemp();
}



void getEmailList()
{
  String Link = "http://api-iot-co.herokuapp.com/emails/list";
  http.begin(client, Link);          // Specify request destination
  int httpCode = http.GET();         // Send the request
  String payload = http.getString(); // Get the response payload
  Serial.println("Code");
  Serial.println(httpCode); // Print HTTP return code
  Serial.println("PayLoad");
  Serial.println(payload); // Print request response payload
  http.end();              // Close connection
  delay(5000);
}


void getEstadoPuerta()
{
  String Link = "http://api-iot-co.herokuapp.com/temperatura/puerta";
  http.begin(client, Link);          // Specify request destination
  int httpCode = http.GET();         // Send the request
  String payload = http.getString(); // Get the response payload
  Serial.println("PayLoad");
  Serial.println(payload); // Print request response payload
  if ( estadoPuerta != payload) {
    sendEmail("Estado Puerta", "Se cambio el estado de la puerta");
  }
  estadoPuerta = payload;
  http.end();              // Close connection
  delay(1000);
}




void loop()
{

  //getTemperature();
  getEstadoPuerta();
}
