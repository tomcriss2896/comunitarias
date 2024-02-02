#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// WiFi AP SSID
#define WIFI_SSID "FIEC-AUTO" //Editar FIEC-AUTO
// WiFi password
#define WIFI_PASSWORD "FIEC-c4r2021" //Editar FIEC-c4r2021

#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "urdGTuo26UDYDiJkG_93Nc-yNqDpl0Q40WbxvCbEiRS259ayjtV5rYQ9digCw4ZU1MxlyRSHoVjJMd25-BFzEg=="
#define INFLUXDB_ORG "f773c8c9b46f12f0"
#define INFLUXDB_BUCKET "esp32"

// Time zone info
#define TZ_INFO "UTC-5"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Declare Data point
Point sensor("pressure_value_2");

void wifi_and_connection_init(); //Función para establecer conexión WIFI y con la base de datos
void send_to_DB(float value); //Función para enviar el valor a la base de datos

const int PinPresion = 32;
const int retardo = 100;
int lectura;

void setup(){
  Serial.begin(115200);

  wifi_and_connection_init();

}

void loop(){
  //lectura= analogRead(potenciometro);
  //Serial.println(lectura);
  
  float wuajaja = analogRead(PinPresion)-410;
  float presion4 = map(wuajaja-10,0,4095,0,86); //aprobado2

  Serial.print("ValorOrg = ");
  Serial.print(analogRead(PinPresion)); //500 //3500
  Serial.print("\t\t");

  Serial.print("Valor = ");
  Serial.print(wuajaja);
  Serial.print("\t\t");
  
  Serial.print("map4 (psi) = "); //aprobado2
  Serial.print(presion4);
  Serial.println("\t\t");

  send_to_DB(presion4); //Enviar dato a DB (CAMBIAR SI NO ES LA VARIABLE A ENVIAR)

  delay(retardo);
}  


/*

  //PRUEBAS PRUEBOSAS

  //const int potenciometro=32

  //float valor = analogRead(PinPresion)-409;
  //float presion1 = map(valor,0,4095,0,500);
  //float presion2 = map(wuajaja,0,4095,0,100); //aprobado
  //float presion3 = map(wuajaja-10,0,4095,0,100);
  //float presion5 = map(wuajaja-10,500,3500,0,60);
  //float presion = valor/4095*500;

  Serial.print("map2 (psi) = "); //aprobado
  Serial.print(presion2);
  Serial.print("\t\t");

  Serial.print("map3 (psi) = ");
  Serial.print(presion3);
  Serial.print("\t\t");

  Serial.print("map5 (psi) = "); 
  Serial.print(presion5);
  Serial.println("\t\t");

  Serial.print("cuak = "); //cuak
  Serial.print(cuak);
  Serial.print("\t\t");

  Serial.print("cuak1 = "); //cuak
  Serial.print(cuak1);
  Serial.print("\t\t");

  delay(50);
*/

void wifi_and_connection_init() { //Función para establecer conexión WIFI y con la base de datos
  // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Accurate time is necessary for certificate validation and writing in batches
  // We use the NTP servers in your area as provided by: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  sensor.addTag("device", DEVICE);
}

void send_to_DB(float value) { //Función para enviar el valor a la base de datos
  // Clear fields for reusing the point. Tags will remain the same as set above.
  sensor.clearFields();

  // Store the sensor into the point
  sensor.addField("pressure_value_2", value);

  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  // Check WiFi connection and reconnect if needed
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }

  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}
