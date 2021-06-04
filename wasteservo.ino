#include <ESP8266WiFi.h>  
#include <Servo.h>
const int IR       = D3;                           // output from IR sensor  
const int trigPin1 = D5;                           // Trigger to Ultrasonic sensor in bin 1
const int echoPin1 = D6;                           // Echo from Ultrasonic sensor in bin 1 
const int trigPin2 = D7;                           // Trigger to Ultrasonic sensor in bin 2
const int echoPin2 = D8;                           // Echo from Ultrasonic sensor in bin 2 
Servo myservo; 
long duration1,duration2; 
int distance1,distance2;  

WiFiClient client;
WiFiServer server(80);  
const char* MY_SSID = "MY_SSID";                   // WIFI network ID
const char* MY_PWD = "MY_PWD";                     // password of your wifi network
const char* TS_SERVER = "api.thingspeak.com"; 
String TS_API_KEY ="XXXXXXXXXXXXXXXX";             // API key of your channel from thingspeak

void connectWifi() 
{ 
  Serial.print("Connecting to "+ *MY_SSID);  
  WiFi.begin(MY_SSID, MY_PWD);  
  while (WiFi.status() != WL_CONNECTED) 
  {     
    delay(1000);    
    Serial.print("."); 
  }
  Serial.println("");  
  Serial.println("WiFi Connected"); 
  Serial.println("");
  server.begin();   
  Serial.println("Server started"); 
  Serial.print("Use this URL to connect: ");
  Serial.print("http://"); 
  Serial.print(WiFi.localIP());
  Serial.println("/");   
}

void sendDataTS(void)
{   
  int water_value = analogRead(A0);                    //reading value from moisture sensor
  Serial.print("Water_Value:  ");   
  Serial.println(water_value); 
  if(digitalRead(IR)==LOW && water_value>=1024)        //checking values of moisture and IR sensors. Condition for dry waste.
  {
    myservo.write(180);
    Serial.println(" dry ");
    delay(2000);
    myservo.write(90);
    delay(2000);   
  }
  else if(digitalRead(IR)==LOW && water_value<1024)    //condition for wet waste.
  {
   myservo.write(0);
   Serial.println(" wet ");
   delay(2000);
   myservo.write(90);
   delay(2000);   
  } 
  digitalWrite(trigPin1, LOW);  
  delayMicroseconds(2);  
  digitalWrite(trigPin1, HIGH); 
  delayMicroseconds(10);  
  digitalWrite(trigPin1, LOW);   
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1*0.034/2;                      // indicator of space left in bin 1
  Serial.print("Distance_1: ");       
  Serial.println(distance1);    
  delay(1000); 
  digitalWrite(trigPin2, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trigPin2, HIGH);  
  delayMicroseconds(10);  
  digitalWrite(trigPin2, LOW);   
  duration2 = pulseIn(echoPin2, HIGH);  
  distance2 = duration2*0.034/2; 
  Serial.print("Distance_2: ");                       // indicator of space left in bin 2
  Serial.println(distance2);    
  Serial.print("Water_Value:  ");   
  Serial.println(water_value); 

 if (client.connect(TS_SERVER, 80))    
 {
          String postStr = TS_API_KEY;    
          postStr += "&field1="; 
          postStr += String(distance1);    
          postStr += "&field2=";  
          postStr += String(distance2);
          postStr += "\r\n\r\n";
          client.print("POST /update HTTP/1.1\n"); 
          client.print("Host: api.thingspeak.com\n");   
          client.print("Connection: close\n");  
          client.print("X-THINGSPEAKAPIKEY: " + TS_API_KEY + "\n");  
          client.print("Content-Type: application/x-www-form-urlencoded\n");  
          client.print("Content-Length: ");    
          client.print(postStr.length());  
          client.print("\n\n");   
          client.print(postStr);   
          delay(100); 
}
client.stop(); 
}


void setup() 
{
  Serial.begin(9600); 
  myservo.attach(D1);                                   //attaches servopin to D1 of nodemcu
  myservo.write(90);
  delay(100);   
  connectWifi(); 
  myservo.attach(D1);
  pinMode(trigPin1, OUTPUT);                           // Sets the trigPin as Output   
  pinMode(echoPin1, INPUT);                            // Sets the echoPin as Input  
  pinMode(trigPin2, OUTPUT);                           // Sets the trigPin as Output   
  pinMode(echoPin2, INPUT);                            // Sets the echoPin as Input  
  pinMode(IR,INPUT);                                   // Sets the IR pin as Output
}  

 
void loop()  
{  
 sendDataTS();
 delay(3000);
}
