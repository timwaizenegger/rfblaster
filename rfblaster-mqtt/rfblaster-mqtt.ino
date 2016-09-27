

/*
  RFBLASTER - part of the TCL IOT project
  Tim Waizenegger (c) 2015
  MIT License
  ESP8266 on arduino, pubsub mqtt client

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "hive_iot";
const char* password = "parailoo";
const char* mq_org = "tju8ax";
const char* mq_type = "ESP8266";
const char* mq_id = "rf_sender";
const char* mq_user = "use-token-auth";
const char* mq_authtoken = "u1hOG+o2El3-dotuUE";
const char* mq_clientId = "d:tju8ax:ESP8266:rf_sender"; //"d:"+mq_org+":"+mq_type+":"+mq_id;
const char* mq_serverUrl = "tju8ax.messaging.internetofthings.ibmcloud.com";



WiFiClient espClient;
PubSubClient client(espClient);
#define RFPIN 12
#define RF_indicator_PIN 13
#define MQTTPIN 14

#define sigOn 0
#define sigOff 1


const long interval = 2000; // millis
unsigned long previousMillis = 0;


/////////////////////////////////////////////////////////////////
// help
int to_int(char const *s)
{
  if ( s == NULL || *s == '\0' )
    return -1;

  bool negate = (s[0] == '-');
  if ( *s == '+' || *s == '-' )
    ++s;

  if ( *s == '\0')
    return -1;

  int result = 0;
  while (*s)
  {
    if ( *s >= '0' && *s <= '9' )
    {
      result = result * 10  - (*s - '0');  //assume negative number
    }
    else
      return -1;
    ++s;
  }
  return negate ? result : -result; //-result is positive!
}

/////////////////////////////////////////////////////////////////
// RF CODES WE KNOW

#define codesForType1Length 12
#define codesForType1Count 16
const char codesForType1[][codesForType1Length + 1] = {
  "022202222222", // 1-1-on
  "022202222220", // 1-1-off
  "022220222222", // 1-2-on
  "022220222220", // 1-2-off
  "022222022222", // 1-3-on
  "022222022220", // 1-3-off
  "022222202222", // 1-4-on
  "022222202220", // 1-4-off
  "222002222222", // 4-1-on
  "222002222220", // 4-1-off
  "222020222222", // 4-2-on
  "222020222220", // 4-2-off
  "222022022222", // 4-3-on
  "222022022220", // 4-3-off
  "222022202222", // 4-4-on
  "222022202220"  // 4-4-off
};

#define codesForType2Length 33
#define codesForType2Count 6
const char codesForType2[][codesForType2Length + 1] = {
  "213222123221322122232123132132221", // 1-on
  "213222123221322122232123132222221", // 1-off
  "213222123221322122232123132132212", // 2-on
  "213222123221322122232123132222212", // 2-off
  "213222123221322122232123132132131", // 3-on
  "213222123221322122232123132222131" // 3-off
};


#define codesForType3Length 51
#define codesForType3Count 2
const char codesForType3[][codesForType3Length + 1] = {
  "0000101101111110101100000s0000001100110110001100000", // 1-on
  "0000110101100101100100000s0000011110110010101000000" // 1-off
};



/////////////////////////////////////////////////////////////////
// RF CODE STUFF

void setSigOn() {
  digitalWrite(RFPIN, sigOn);
  digitalWrite(RF_indicator_PIN, 1);
}
void setSigOff() {
  digitalWrite(RFPIN, sigOff);
  digitalWrite(RF_indicator_PIN, 0);
}


// this code has 1 long packet, followed by 3 short packets
// the long packet has a header, followed by 3 short suffixes
// the 3 suffixes and the 3 short packets are the same (except the last bit of each packet is a longer tailing pulse).
// the delay between the packets is 14c
// the delay between the suffixes in the first packet is 4c
void sendCodeType3(const char code[])  {
  const int cycleLength = 500;
  
  Serial.println(code);

  for (char i = 0; i < 25; i++) {
     switch (code[i]) {
      case '0':
        setSigOn();
        delayMicroseconds(cycleLength);
        setSigOff();
        delayMicroseconds(2 * cycleLength);
        break;
      case '1':
        setSigOn();
        delayMicroseconds(2*cycleLength);
        setSigOff();
        delayMicroseconds(cycleLength);      
        break;
     }
        
  }
  delayMicroseconds(3 * cycleLength); // pause is 4c, 1c was already spent in the header so we wait 3c here
  for (char i = (0+25+1); i < (25+25+1); i++) { // skip the separator and continue with chars 26..EOL
    switch (code[i]) {
      case '0':
        setSigOn();
        delayMicroseconds(cycleLength);
        setSigOff();
        delayMicroseconds(2 * cycleLength);
        break;
      case '1':
        setSigOn();
        delayMicroseconds(2*cycleLength);
        setSigOff();
        delayMicroseconds(cycleLength);      
        break;
     }
  }
  for (char i = (0+25+1); i < (25+25+1); i++) { // skip the separator and continue with chars 26..EOL
    switch (code[i]) {
      case '0':
        setSigOn();
        delayMicroseconds(cycleLength);
        setSigOff();
        delayMicroseconds(2 * cycleLength);
        break;
      case '1':
        setSigOn();
        delayMicroseconds(2*cycleLength);
        setSigOff();
        delayMicroseconds(cycleLength);      
        break;
     }
  }
  for (char i = (0+25+1); i < (24+25+1); i++) { // skip the separator and continue with chars 26..EOL
    switch (code[i]) {
      case '0':
        setSigOn();
        delayMicroseconds(cycleLength);
        setSigOff();
        delayMicroseconds(2 * cycleLength);
        break;
      case '1':
        setSigOn();
        delayMicroseconds(2*cycleLength);
        setSigOff();
        delayMicroseconds(cycleLength);      
        break;
     }
  }
  // send the tailing pulse
  setSigOn();
  delayMicroseconds(6*cycleLength);
  setSigOff();
  delayMicroseconds(14*cycleLength);   // delay to next short packet   
  // looks like we don't need to send any more data... receivers already react at this point!
  // the protocol would have (a minimum of) 3 additional packets of repeat-codes here...
}






void sendCodeType2(const char code[]) {
  const int cycleLengthBpulse = 240; 
  const int cycleLengthBwait = 300;
  const int cycleLengthBhold = 1360;
  
  // init pulse
  setSigOn();
  delayMicroseconds(cycleLengthBpulse);
  setSigOff();
  delayMicroseconds(2 * cycleLengthBhold);

  //Serial.println("sending code...");
  for (char i = 0; i < codesForType2Length; i++) {
    //Serial.println(i);
    //Serial.println(code[i]);
    switch (code[i]) {
      case '3':
        setSigOn();
        delayMicroseconds(cycleLengthBpulse);
        setSigOff();
        delayMicroseconds(cycleLengthBwait);
      case '2':
        setSigOn();
        delayMicroseconds(cycleLengthBpulse);
        setSigOff();
        delayMicroseconds(cycleLengthBwait);
      case '1':
        setSigOn();
        delayMicroseconds(cycleLengthBpulse);
        setSigOff();
        delayMicroseconds(cycleLengthBwait);
        break;
    }
    delayMicroseconds(cycleLengthBhold);
  }

}



void sendCodeType1(const char code[]) {
  //Serial.println("sending code...");
  const int cycleLength = 120;
  for (char i = 0; i < codesForType1Length; i++) {
    //Serial.println(i);
    //Serial.println(code[i]);
    switch (code[i]) {
      case '0':
        setSigOn();
        delayMicroseconds(4 * cycleLength);
        setSigOff();
        delayMicroseconds(12 * cycleLength);
        setSigOn();
        delayMicroseconds(4 * cycleLength);
        setSigOff();
        delayMicroseconds(12 * cycleLength);
        break;
      case '1':
        setSigOn();
        delayMicroseconds(12 * cycleLength);
        setSigOff();
        delayMicroseconds(4 * cycleLength);
        setSigOn();
        delayMicroseconds(12 * cycleLength);
        setSigOff();
        delayMicroseconds(4 * cycleLength);
        break;
      case '2':
        setSigOn();
        delayMicroseconds(4 * cycleLength);
        setSigOff();
        delayMicroseconds(12 * cycleLength);
        setSigOn();
        delayMicroseconds(12 * cycleLength);
        setSigOff();
        delayMicroseconds(4 * cycleLength);
        break;
    }
  }
  // sync pulse
  setSigOn();
  delayMicroseconds(4 * cycleLength);
  setSigOff();
}


void sendType1(const char code[]) {
  Serial.println("sending code type 1...");
  for (char i = 0; i < 4; i++) {
    sendCodeType1(code);
    delay(15);
  }
  Serial.println("sending code type 1... done");
}
void sendType2(const char code[]) {
  Serial.println("sending code type 2...");
  for (char i = 0; i < 4; i++) {
    sendCodeType2(code);
    delay(15);
  }
  Serial.println("sending code type 2... done");
}
void sendType3(const char code[]) {
  Serial.println("sending code type 3...");
  for (char i = 0; i < 4; i++) {
    sendCodeType3(code);
    delay(15);
  }
  Serial.println("sending code type 3... done");
}


char currentCode1 = 0;
char currentCode2 = 0;
void sendDemo() {
  Serial.println("sendDemo currentCode");
  Serial.println(int(currentCode1));
  Serial.println(int(currentCode2));

  for (char i = 0; i < 10; i++) {
    sendType1(codesForType1[currentCode1]);
    delay(15);
  }
  for (char i = 0; i < 10; i++) {
    sendType2(codesForType2[currentCode2]);
    delay(15);
  }
  if (++currentCode1 == codesForType1Count) currentCode1 = 0;
  if (++currentCode2 == codesForType2Count) currentCode2 = 0;
}



/////////////////////////////////////////////////////////////////
// WLAN/MQTT

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(500);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = String("");
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(msg);

  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }

  if (root.containsKey("type1")) {
    const char* code = root["type1"];
    int c = to_int(code);
    if ((c < 0) || (c >= codesForType1Count)) return;
    sendType1(codesForType1[c]);
    return;
  }

  if (root.containsKey("type2")) {
    const char* code = root["type2"];
    int c = to_int(code);
    if ((c < 0) || (c >= codesForType2Count)) return;
    sendType2(codesForType2[c]);
    return;
  }
  
  if (root.containsKey("type3")) {
    const char* code = root["type3"];
    Serial.println(code);
    int c = to_int(code);
    Serial.println(c);
    if ((c < 0) || (c >= codesForType3Count)) return;
    sendType3(codesForType3[c]);
    return;
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    digitalWrite(MQTTPIN, 0);
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mq_clientId, mq_user, mq_authtoken)) {
      Serial.println("connected");
      digitalWrite(MQTTPIN, 1);
      // ... and resubscribe
      client.subscribe("iot-2/cmd/send/fmt/json");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}




/////////////////////////////////////////////////////////////////
// ARDUINO API
void setup() {
  Serial.begin(115200);
  pinMode(RFPIN, OUTPUT);
  pinMode(RF_indicator_PIN, OUTPUT);
  pinMode(MQTTPIN, OUTPUT);
  setSigOff();
  digitalWrite(MQTTPIN, 0);
  setup_wifi();
  client.setServer(mq_serverUrl, 1883);
  client.setCallback(callback);
}


// the loop routine runs over and over again forever:
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();



//    unsigned long currentMillis = millis();
//  
//    // loop for 2 secs to make sure transmission has occured
//    if (currentMillis - previousMillis >= 500) {
//      previousMillis = currentMillis;
//      sendDemo();
//  
//    }

}
