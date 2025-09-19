/******************************************************************************************/
// This is a simple room thermometer that uses a stepper motor to display the temperature //
// Author  : Ashish Adhikari                                                              //
// Email   : tarantula3@gmail.com                                                         //
// YouTube : https://www.youtube.com/@CrazyCoupleDIY                                      //
// Blog    : http://diy-projects4u.blogspot.com/                                          //
/******************************************************************************************/

/******************************************************************************************/
#include <TM1637Display.h>                // TM1637 Library
#include <Stepper.h>                      // Stepper Motor Library
#include <DHT.h>                          // DHT Library

#include <ESP8266WiFi.h>                  // URL: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
#include <WiFiClient.h>                   // Wifi Library
#include <ESP8266HTTPClient.h>            // ESP8266 HTTP Client Library
/******************************************************************************************/
const char* WIFI_SSID    = "XXXXXX";      // Your SSID
const char* WIFI_PWD     = "XXXXXX";      // Your Password
int StatusCounter        = 0;
unsigned long timerDelay = 1800000;       // Timer set to 30 minutes (1800000) or 1 Minute (60000)
unsigned long lastTime   = 0;             // Previous state

IPAddress local_IP(192, 168, 0, 201);     // Static IP Address for ESP8266
IPAddress subnet(255, 255, 255, 0);       // Subnet Mask
IPAddress gateway(192, 168, 0, 1);        // Default Gateway

String URLUpdateStatus  = "http://192.168.0.7/Arduino/Weather/UpdateStatus.php";
String URLUpdateTemp    = "http://192.168.0.7/Arduino/Weather/UpdateTemperature.php";
/******************************************************************************************/
#define DHTPIN  D5                        // Define the DHT Pin 
#define DHTTYPE DHT22                     // In this example I am going to use DHT22.
DHT dht          = DHT(DHTPIN, DHTTYPE);  // Create a DHT object
int HUM          = 0;
int TEMP         = 0;
int Current_Temp = 0;                     // Variable to store the current value of the temperature 
int Current_Hum  = 0;                     // Variable to store the current value of the humidity
/******************************************************************************************/
#define CLK     D4                        // Single clock pin for both displays 
#define DIO_TMP D6                        // Define the Temperature Display Pin
TM1637Display display_TMP(CLK, DIO_TMP);  // Create an instance of the TM1637Display

#define DIO_HUM D7                        // Define the Humidity Display Pin 
TM1637Display display_HUM(CLK, DIO_HUM);  // Create an instance of the TM1637Display

// Create the °C Symbol
const uint8_t Celsius[] = {
  SEG_A | SEG_B | SEG_F | SEG_G,          // Circle °
  SEG_A | SEG_D | SEG_E | SEG_F           // C
};

// Create the H Symbol
const uint8_t Letter_H[] = {
  SEG_F | SEG_E | SEG_G | SEG_B | SEG_C   // H
};
/******************************************************************************************/
#define SWITCH D8                         // Define the Reset Switch Pin 
/******************************************************************************************/
const int stepsPerRevolution = 2038;      // Change this to fit the number of steps per revolution of your motor
Stepper myStepper(stepsPerRevolution, D0, D2, D1, D3);// Initialize the stepper library on pins D0 through D3
/******************************************************************************************/

/************************************ SETUP SECTION ***************************************/
void setup() {  
  Serial.begin(115200);                   // Initialize the serial port
  
  /*********** Setup a WiFi connection ***********/
  if (WiFi.config(local_IP, gateway, subnet)) { Serial.println("Static IP Configured"); }
  else                                        { Serial.println("Static IP Configuration Failed"); };
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PWD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  };
  Serial.println("\nWiFi connected");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());

  SendIamAlive();                         // Send the initial wakeup message to the server
  /**********************************************/
  
  display_TMP.setBrightness(7);           // Set the display brightness (0-7)
  display_HUM.setBrightness(7);           // Set the display brightness (0-7)
  
  dht.begin();                            // Setup the DHT sensor

  pinMode(SWITCH, INPUT);                 // Declare the switch pin as input
};
/******************************************************************************************/

/************************************ LOOP SECTION ****************************************/
void loop() {   
  /** Read the temp and humidity info from ther sensor and display it on the 7-segmnet and Gauge **/
  Read_Temp();                            
  
  /** Sending Humidity and temperature every 30 minutes **/
  if((millis() - lastTime) > timerDelay){ Serial.println("Sending Temp and Humidity"); SendTemperatureAndHumidity(); lastTime = millis(); };

  /** Sending I am alive message every minute **/
  if((millis() - StatusCounter) > 60000){ Serial.println("Sending Heartbeat"); SendIamAlive(); StatusCounter = millis(); };  
};
/******************************************************************************************/

/******************************** SUPPORTING FUNCTIONS ************************************/
//// This function is used to send heartbeats to the Raspberry Pi Home Server 
void SendIamAlive(){
  WiFiConnect( URLUpdateStatus, String("?Token=32432dewfd435zft56he5d") + String("&Location=StudyRoom") );
};

//// Function that sends Temperature and Humidity info to the server
void SendTemperatureAndHumidity() {
  WiFiConnect( URLUpdateTemp, String("?HUM=") + HUM + String("&TEMP=") + TEMP + String("&Location=StudyRoom") );
};

//// Function that establishes a wireless connection 
void WiFiConnect( String URL, String Querystring )
{  
  if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      String serverPath = URL + Querystring; 
      Serial.println(serverPath);

      http.begin(client, serverPath.c_str());
      int httpResponseCode = http.GET();      // Send HTTP GET request

      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
      } else {
        Serial.print("Error code: "); Serial.println(httpResponseCode);
        /** Reboot the device if device is unable to contact the server **/
        ESP.restart();
      };
      http.end();                            // End the connection
  } else {
    Serial.println("WiFi Disconnected");
  };
};

//// This function sends the niddle to its home position and then moves it to the starting point
void Return_Home() {
   // Move the niddle counterclockwise until it touches the metal plates
   int SWITCH_STATE = LOW;
   Serial.println("Needle Returning Back Home");
   do{
     myStepper.step(-1);                  // Move counterclockwise until it hits the home stick
     delay(10);
     
     SWITCH_STATE = digitalRead(SWITCH);  // Read the current state of the switch     
     if   ( SWITCH_STATE == HIGH ) { Serial.println("Reached Home"); break; }    
     else { Serial.print("."); };
   } while(true);
};

//// Function that reads and displays the temperature and humidity on the 7-segmnet displays
//// This function will only update the content of the display if there is a change in the new value
void Read_Temp(){
  HUM  = dht.readHumidity();    // Read the humidity
  TEMP = dht.readTemperature(); // Read the temperature

  if( Current_Hum != HUM ){
    Serial.print("Humidity : ");    Serial.println(HUM);
    // Update the value of Humidity on the 7 segment disaplay
    display_HUM.clear();        // Clear the display before repainting
    display_HUM.showNumberDec(HUM, false, 2, 0);
    display_HUM.setSegments(Letter_H, 1, 3);
    Current_Hum = HUM;          // Reset the value of Current_Hum
  };
  
  if( Current_Temp != TEMP ){
    Serial.print("Temperature : "); Serial.println(TEMP);
    // Update the temperature and display it in Celsius
    display_TMP.clear();        // Clear the display before repainting
    display_TMP.showNumberDec(TEMP, false, 2, 0);
    display_TMP.setSegments(Celsius, 2, 2);    
    Move_Niddle(TEMP);          // Move the niddle to the new position
    Current_Temp = TEMP;        // Reset the value of Current_Temp
  };

  // Sending temperature and humidity data to the server on bootup
  if(lastTime == 0) { SendTemperatureAndHumidity(); lastTime = millis(); };
};

//// This function moves the niddle to the correct temperature on the gauge
void Move_Niddle(int TEMP){
   Return_Home();               // Caliberate the niddle to home position 

   myStepper.setSpeed(300);     // Set the stepper speed
   //int STEPS = (5.677 * 3 * TEMP) + 170; // 5.68 (step resolution) * 3 (steps to display each °C) * TEMP + 170 (5.68 * 10 * 3) = since it starts from -10 and not 0)
   //int STEPS = (5.6888 * 3 * TEMP) + 170.6666;
   int STEPS = (5.661 * 3 * TEMP) + 169.833; // 5.661 (step resolution) * 3 (steps to display each °C) * TEMP + 169.833 (5.661 * 10 * 3) = since it starts from -10 and not 0)
   
   int stepCount = 0;
   do{
        myStepper.step(1);
        Serial.print("steps:");
        Serial.println(stepCount);
        stepCount++;
        delay(10);
   } while( stepCount <= STEPS );
};
/******************************************************************************************/
