#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Ethernet.h>
#define green D2
#define red D3
#define buzz D4
bool war=0;
const char* ssid     = "ESP8266";
char readdata;
String datas;
float t = 36.6;
String limit="9999";
float lim;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
unsigned long previousMillis = 0;
const long interval = 1000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP8266 DHT Server</h2>
  <p>
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 1000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with OUR values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);}
  return String();
  
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  pinMode(green,OUTPUT);
  pinMode(red,OUTPUT);
  pinMode(buzz,OUTPUT);
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
    if(request->hasParam("limit"))
      AsyncWebParameter* p = request->getParam("limit");
      int args = request->args();
        for(int i=0;i<args;i++)
        {
          //Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
          limit=request->arg(i).c_str();
        }
  });
  // Start server
  server.begin();
}
 
void loop(){  
  lim=limit.toFloat();
  /*if(Serial.available()==0)
  {
   Serial.println(lim); 
  }*/
  if (Serial.available() > 0 ) 
  {
    readdata=Serial.read();
    datas=datas+readdata;
    if(readdata == '\n')
    {
      if(isnan(t))
      {Serial.println("Blad pobierania");}
      else
      {
        t=datas.toFloat();
        datas="";
        //Serial.println(t);         
      }
    }
   }
   if(lim>t && war==0){
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
   }
   else if(lim>t && war==1){
    digitalWrite(green, HIGH);
    digitalWrite(red, LOW);
    war=0;
   }
   else if(lim<=t && war==0){
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    play();
    war=1;   
    }
    else play();
}
void play()
{
  tone(buzz, 200);  
    delay(100);
  noTone(buzz);
    delay(200);    
  tone(buzz, 200);
    delay(100);
  noTone(buzz);
    delay(200);    
  tone(buzz, 200);
    delay(100);
  noTone(buzz);
    delay(2000);
  }
