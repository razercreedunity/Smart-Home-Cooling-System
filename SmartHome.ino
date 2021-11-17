#include <LiquidCrystal.h>

//wifi variable
String ssid     = "Simulator Wifi";  // SSID to connect to
String password = ""; // Our virtual wifi has no password (so dont do your banking stuff on this network)
String host     = "api.thingspeak.com"; // Open Weather Map API
const int httpPort   = 80;
String url     = "/update?api_key=8CMAGACVTD1NIUZ2&field1=";// API key

//general variable
LiquidCrystal lcd(12,11,8,7,6,5);
int tempPin = A0;   // Output pin from LM35 (temperature)
int fan = 10;       // fan motor pin
int AC = 9;			// AC motor pin
int motion = 13;	// motion sensor pin
int temp = 0;
int fanSpeed = 0;
int fanLCD = 0;
int AcSpeed = 0;
int AcLCD = 0;
int stop = 2;		// pin Interrupt 
int val = 0;
int state = 0;
int loopcount = 0;
int starttime = 0;
int endtime = 0;
int time = 6000; //determine how many second of no movement to off the system


// Start our ESP8266 Serial Communication
void setupESP8266(void) {
  
  Serial.begin(115200);   // Serial connection over USB to computer
  Serial.println("New Connection...");   // Serial connection on Tx / Rx port to ESP8266
  delay(50);
  Serial.println("ESP8266 OK!!!\n");
    
  // Connect to Simulator Wifi
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  delay(50);
  Serial.println("Connected to WiFi!!!\n");
  
  
  //ESP8266 connects to the server as a TCP client 
  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(50);
  Serial.println("ESP8266 Connected to server!!!\n") ;
 
}

// function to update data to Thingspeak
void anydata(void) {
  
  temp = readTemp();
  int motion_sensor = digitalRead(motion); 
  
  if(temp < 17 && temp != 0) {
    AcSpeed = 18;					// the actual speed of AC 		
    AcLCD = 1; 					// speed of AC to display on LCD 		
    analogWrite(AC, AcSpeed); 	// spin the AC at the AcSpeed speed		 
   	  
    fanSpeed = 0;					// the actual speed of fan 
    fanLCD = 0; 					// speed of fan to display on LCD 
    analogWrite(fan, fanSpeed);	// spin the fan at the fanSpeed speed
  } 
  // IF temp range 17-23
  if(temp >= 17 && temp <= 23) { 		 
    AcSpeed = 36;						
    AcLCD = 2; 						 
    analogWrite(AC, AcSpeed); 		 
   	  
    fanSpeed = 0;
    fanLCD = 0; 
    analogWrite(fan, fanSpeed);
  } 
  // IF temp range 24-30
  if(temp >= 24 && temp <=30) {
    AcSpeed = 54;					
    AcLCD = 3; 					
    analogWrite(AC, AcSpeed); 	
   	  
    fanSpeed = 36;
    fanLCD = 2; 
    analogWrite(fan, fanSpeed);
  } 
  // IF temp range 31-40
  if(temp >= 31 && temp <=40) {
    AcSpeed = 72;							
    AcLCD = 4; 							
    analogWrite(AC, AcSpeed); 			 
   	  
    fanSpeed = 72;
    fanLCD = 4; 
    analogWrite(fan, fanSpeed);
  } 
  
  // IF temp exceed 40
  if(temp > 40) {
    AcSpeed = 90;							
    AcLCD = 5; 							
    analogWrite(AC, AcSpeed); 			 
   	  
    fanSpeed = 90;
    fanLCD = 5; 
    analogWrite(fan, fanSpeed);
  }
  
  // Construct our HTTP call
  //String httpPacket = "GET " + url + String(temp) + "&field2="+ String(motion_sensor) + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
  String httpPacket = "GET " + url + String(temp) + "&field2="+ String(motion_sensor) + "&field3="+ String(fanLCD) + "&field4="+ String(AcLCD) +" HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
  //String httpPacket = "GET " + url + String(temp) + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
  int length = httpPacket.length();
  
  // Send our message length & http request
  Serial.print("AT+CIPSEND=");
  Serial.println(length);
  delay(10);
  Serial.print(httpPacket);
  delay(10);
  Serial.println("ESP8266 sends data to the server\n");    
}

void setup() {
  setupESP8266();
  pinMode(fan, OUTPUT);    //Fan motor 
  pinMode(AC, OUTPUT);	   //Air-Conditioner motor
  pinMode(tempPin, INPUT); //temperature sensor
  lcd.begin(16,2);  
  }

void loop() {      
  anydata();
  val = digitalRead(motion);   //read motion sensor
  
  //When there is motion
  if (val == HIGH)     
  {
    delay(100);           // delay 100 milliseconds
    if (state == LOW) 
    {
    state = HIGH;       // update variable state to HIGH
    temp = readTemp();  // read temperature in Celcius
    }  
  }
  
  //When there is no motion or motion is stopped
  else
  {
    if (temp != 0) {
      starttime = millis();
      Serial.println(starttime);
	  endtime = starttime;
		while ((endtime - starttime) <= time) // do this loop with fixed "time" in mS after detecting motion 
		{
          running();							  //Fan & AC speed method based on temperature	
          loopcount = loopcount+1;
          endtime = millis();
          Serial.println(endtime);
		} 
    } 
    delay(200);
    
    if (state == HIGH)
    {
    	temp = 0;
      	digitalWrite (fan, LOW);
      	digitalWrite (AC, LOW);
    	state = LOW;       			// update variable state to LOW
    }
  }
  Serial.println(state);   //monitoring the "state" in serial monitor
} //end of void loop()

//Function to Display Fan & AC speed 
void running() {
  
  temp = readTemp();
  // IF temp below 17
  if(temp < 17 && temp != 0) {
    AcSpeed = 18;					// the actual speed of AC 		
    AcLCD = 1; 					// speed of AC to display on LCD 		
    analogWrite(AC, AcSpeed); 	// spin the AC at the AcSpeed speed		 
   	  
    fanSpeed = 0;					// the actual speed of fan 
    fanLCD = 0; 					// speed of fan to display on LCD 
    analogWrite(fan, fanSpeed);	// spin the fan at the fanSpeed speed
  } 
  // IF temp range 17-23
  if(temp >= 17 && temp <= 23) { 		 
    AcSpeed = 36;						
    AcLCD = 2; 						 
    analogWrite(AC, AcSpeed); 		 
   	  
    fanSpeed = 0;
    fanLCD = 0; 
    analogWrite(fan, fanSpeed);
  } 
  // IF temp range 24-30
  if(temp >= 24 && temp <=30) {
    AcSpeed = 54;					
    AcLCD = 3; 					
    analogWrite(AC, AcSpeed); 	
   	  
    fanSpeed = 36;
    fanLCD = 2; 
    analogWrite(fan, fanSpeed);
  } 
  // IF temp range 31-40
  if(temp >= 31 && temp <=40) {
    AcSpeed = 72;							
    AcLCD = 4; 							
    analogWrite(AC, AcSpeed); 			 
   	  
    fanSpeed = 72;
    fanLCD = 4; 
    analogWrite(fan, fanSpeed);
  } 
  
  // IF temp exceed 40
  if(temp > 40) {
    AcSpeed = 90;							
    AcLCD = 5; 							
    analogWrite(AC, AcSpeed); 			 
   	  
    fanSpeed = 90;
    fanLCD = 5; 
    analogWrite(fan, fanSpeed);
  }
   
  //print value on LCD
  lcd.print("TEMP: ");
  lcd.print(temp);      // display current temp
  lcd.print("C ");
  lcd.setCursor(0,1);   // new line
  lcd.print("FN: ");
  lcd.print(fanLCD);    // display fan speed
  lcd.setCursor(9,1);
  lcd.print("AC: ");
  lcd.print(AcLCD);
  delay(200);
  lcd.clear();   
}

// function to read temperature and convert to Celsius
int readTemp() {  
  temp = map(((analogRead(tempPin) - 20) * 3.04), 0, 1023, -40, 125);
  
  return temp;
  }
