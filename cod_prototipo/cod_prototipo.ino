#include "DHT.h" // Biblioteca do sensor
#include <SoftwareSerial.h> // Biblioteca para emular conexão serial

#define DHTPIN 7      // Pino conectado ao Sensor DHT
#define DHTTYPE DHT11 // Modelo do sensor DHT

#define RX 6
#define TX 5

#define GREEN_LED 3 
#define RED_LED 4 

String AP = "TESTE"; // Nome da rede onde o protótipo se conectará
String PASS = "0123456789"; // Senha da rede
String API = "R5W7L8Y4T95UR75Y"; // Chave do API Thingspeak
String HOST = "api.thingspeak.com";
String PORT = "80";   //Porta de comunicação com o Thingspeak
  
int countTrueCommand;
int countTimeCommand; 

boolean found = false; 
float valSensorT = 1;
float valSensorH = 1;

DHT dht(DHTPIN, DHTTYPE); //Declarando sensor
SoftwareSerial esp8266(RX,TX);  //Declarando emulador de conexão serial

void setup() {
  dht.begin(); //Iniciando sensor
  Serial.begin(9600); //baud-rate do monitor serial
  esp8266.begin(115200); //baud-rate do esp8266
  // COMANDOS AT
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
 valSensorT = getSensorDataT();
 String getDataT = "GET /update?api_key="+ API +"&"+ "field1" +"="+String(valSensorT);
 delay(1000);
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getDataT.length()+4),5,">");
 delay(1000);
 esp8266.println(getDataT);
 delay(2000);
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 
 valSensorH = getSensorDataH();
 String getDataH = "GET /update?api_key="+ API +"&"+ "field2" +"="+String(valSensorH);
 delay(1000);
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getDataH.length()+4),5,">");
 delay(1000);
 esp8266.println(getDataH);
 delay(2000);
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 countTrueCommand++;
}

float getSensorDataT(){
  delay(2500); // Aguarda 2.5 segundos entre as medições do sensor
  float t = dht.readTemperature(); // lendo temperatura
  
  // Caso haja erro de leitura do sensor de temperatura
  if (isnan(t)) {
    Erro();
    Serial.println(F("Falha ao ler o sensor de temperatura!"));
    return;
  }
  Serial.println("");
  Serial.print(F("===== Temperatura: "));
  Serial.print(t);
  Serial.println(F("°C ====="));
  Serial.println("");
  return t;
}

float getSensorDataH(){

  delay(2500); // Aguarda 2.5 segundos entre as medições do sensor
  float h = dht.readHumidity(); // lendo umidade
  
  // Caso haja erro de leitura do sensor de temperatura
  if (isnan(h)) {
  //  Erro();
    Serial.println(F("Falha ao ler o sensor de umidade!"));
    return;
  }
  Serial.println("");
  Serial.print(F("===== Umidade: "));
  Serial.print(h);
  Serial.println(F("% ====="));
  Serial.println("");
  return h;
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". COMANDO AT => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("[OK]");
    countTrueCommand++;
    countTimeCommand = 0;
    Okay();
  }
  
  if(found == false)
  {
    Serial.println("[FALHA]");
    countTrueCommand = 0;
    countTimeCommand = 0;
    Erro();
  }
  
  found = false;
 }

// LED em caso de erro
void Erro(){                 
    digitalWrite(RED_LED, HIGH);
    delay(1000);
    digitalWrite(RED_LED, LOW); 
  
}
// LED em caso de sucesso
void Okay(){                 
    digitalWrite(GREEN_LED, HIGH);
    delay(1000);
    digitalWrite(GREEN_LED, LOW); 
  
}
