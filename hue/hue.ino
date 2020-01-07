

#include <SPI.h>
#include <MQTT.h>
#include <WiFiNINA.h>
#include <HttpClient.h>


/* We need objects to handle WiFi connectivity
*/
WiFiClient client ;
//HttpClient http;
IPAddress server(192,168,1,131);

/* We need objects to handle:
 *  1. WiFi connectivity
 *  2. MQTT messages
 */
WiFiClient wifi_client ;
MQTTClient mqtt_client ;

/*MQTT */ 

const char* mqtt_host = "max.isasecret.com" ;
/*const uint16_t mqtt_port =  1723 ;*/
const char* username = "majinfo2019";
const char* password = "Y@_oK2";
/* Time between two sensings and values sent. */
#define DELTA_T 1000



/* In which pins is my buttons plugged? */
#define POTENTIOMETER_PIN A4 // TODO: uncomment this line if the potentiometer is connected to analogic pin 4, or adapt...
#define BUTTON_PIN A3



/*We create an ID to know what to listen from the MQTT broker */
String ID = "Projet_2020" ; 
String topic1  = "API2ARDUINO";
/*Type of message we expect to receive: Projet_2020/topic1/"room_id":8 ,"light_id": 8 , "level": 100, "a":2 ,"b": 1, "status": ON;*/
String topic2 = "ARDUINO2API";

/* And associated variables to tell:
    1. which WiFi network to connect to
    2. what are the MQTT broket IP address and TCP port
*/
const char* wifi_ssid     = "lab-iot-1"; 
const char* wifi_password = "lab-iot-1"; 

/* Some variables to handle measurements. */
int potentiometerValue;
int buttonState = 0;         // variable for reading the pushbutton status
int L;
uint32_t t0, t ;



void setup() {
  WiFi.begin(wifi_ssid, wifi_password) ;
  mqtt_client.begin(mqtt_host,1723, wifi_client) ;
  mqtt_client.onMessage(callback) ;
  
  Serial.begin(9600) ;
  delay(100) ;
  Serial.println("Initializing...\n") ;

  // initialize the Potentiometer and button pin as inputs:
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  // Time begins now!
  t0 = t = millis() ;
}

void loop() {
  //We try to connect to the broker and launch the client loop.

  mqtt_client.loop() ;
  if ( ! mqtt_client.connected() ) {
    reconnect() ;
  }
  t = millis() ;
  if ( (t - t0) >= DELTA_T ) {
    t0 = t ;

    // read the states:
    buttonState = digitalRead(BUTTON_PIN);
    potentiometerValue = analogRead(POTENTIOMETER_PIN); // TODO; uncomment this line if it is analog
    int L = potentiometerValue/4;

    Serial.print(ID + "/" + topic2 +"/Luminance: " +L + " button state: " + buttonState );
    Serial.print("\n");

    
    if (client.connect(server, 80)) {
      Serial.println("connecting...");

      client.println("PUT /api/8ZaeVTb1KXefBTExFlphwBxV3QeBTDzfSL6gnXM4/lights/14/state HTTP/1.1");
      client.println("Host: 192.168.1.131");
      client.println("Connection: close");
      client.println("Content-Type: application/json");
      client.println("Content-Length: 12\r\n");
      client.print(String("{\"bri\": ")+L +"}");
      client.println();
  } else {
    // if you couldn't make a connection:
      Serial.println("Couldn't not make a connection...\n") ;
      }
    }
}
/* ################################################################### */
/* This function handles the connection/reconnection to
 * the MQTT broker.
 * Each time a connection or reconnection is made, it
 * publishes a message on the topic+"/HELLO" containing 
 * the number of milli seconds since uController start.
 */
void reconnect() {
  Serial.print("Connecting to ");
  Serial.print(wifi_ssid);
  Serial.print("\n") ;
 
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH) ;
    delay(500);
    digitalWrite(LED_BUILTIN, LOW) ;
    Serial.print(".");
    delay(500);
  }

  Serial.print("\n");
  Serial.print("WiFi connected\n");
  Serial.print("IP address: \n");
  Serial.print(WiFi.localIP());
  Serial.print("\n") ;


  Serial.print("MQTT: trying to connect to host ") ;
  Serial.print(mqtt_host) ;
  Serial.print(" on port ") ;
  /*Serial.print(mqtt_port) ; */
  Serial.println(" ...") ;

  while ( !mqtt_client.connect("Projet_2020", username, password) ) {
    digitalWrite(LED_BUILTIN, HIGH) ;
    delay(500);
    digitalWrite(LED_BUILTIN, LOW) ;
    Serial.print(".");
    delay(500);
  }
  Serial.println("MQTT: connected") ;
  Serial.print("MQTT: root topic is \"") ;
  Serial.print(topic1) ;
  Serial.println("\"") ;
    
 // mqtt_client.publish(String(topic2 + "/HELLO").c_str(), String(millis())) ;

  /* If you want to subscribe to topics, you have to do it here. */
  mqtt_client.subscribe(topic1);
  Serial.print("MQTT: subsribed to \"");
  Serial.print(topic1);
  Serial.print("\n");
}


/* ################################################################### */
/* This function handles the sending of all the values
 * collected by the sensors.
 * Values are sent on a regular basis (every DELTA_T_SEND_VALUES 
 * milliseconds).
 */
void sendValues() {
  if ( mqtt_client.connected() ) {
    mqtt_client.publish(String(topic2 + "/L").c_str(), String(L).c_str());
  }
}



/* ################################################################### */
/* The main callback to listen to topics.
 */
void callback(String &intopic1, String &payload)
{

//  Serial.println("incoming: " + topic1 + " - " + payload);
//  if (payload == "DESC"){
//    mqtt_client.publish(topic2, "Sensors available are: TEMP, HMDT, LUMI") ;
//  }
//  else if (payload == "IDENT"){
//    digitalWrite(LED_BUILTIN, HIGH);
//    delay(10000);
//    digitalWrite(LED_BUILTIN, LOW);
//  }
//  else if (payload = "GETA"){
//    if ( lum != -1 )
//     mqtt_client.publish(String(topic2 + "/LUMI").c_str(), String(lum).c_str()) ;

//  }
}
