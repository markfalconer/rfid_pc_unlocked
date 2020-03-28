//keychain: 2B E2 AB 1B
 //card: 93 96 1E 1A

#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "Ethernet.h"
#include <Time.h>

//#define RST_PIN PB9
//#define SDA_PIN PB6
//#define SCK_PIN PA5
//#define MSI_PIN PA7
//#define MSO_PIN PA6
//#define LED_PIN PC13

//#define RST_PIN 9
//#define SDA_PIN 10
//#define SCK_PIN 13
//#define MSI_PIN 11
//#define MSO_PIN 12

#define RST_PIN 0
#define SDA_PIN 2
#define SCK_PIN 14
#define MSI_PIN 13
#define MSO_PIN 12

boolean present = false;
boolean locked = false;

MFRC522 mfrc522(SDA_PIN, RST_PIN);   // Create MFRC522 instance.

//Internet connection stuff
WiFiClient client;
MySQL_Connection conn((Client *)&client);
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)

// Network credentials
const char* ssid = "";
const char* password = "";
char* page = "";


//SQL Database stuff
IPAddress server_addr();
char* user = "";
char* pass = "";
char query[1024];
MySQL_Cursor cur_mem = MySQL_Cursor(&conn);
  
 
void setup(){  
   Serial.begin(115200);
   while(!Serial);
   WiFi.mode(WIFI_STA); // SETS TO STATION MODE!
   WiFi.begin(ssid, password); //begin WiFi connection

   Serial.print("Attempting to connect to ");
   Serial.print(ssid);
   Serial.print(" with password ");
   Serial.println(password);

   // Wait for connection
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }
   
   Serial.print("\nConnected to ");
   Serial.println(ssid);
   Serial.print("IP Address: ");
   Serial.println(WiFi.localIP());

   while (!conn.connect(server_addr, 3306,user,pass)) {
     delay(1000);
     Serial.print (".");
   }
   Serial.println("");
   Serial.println("Connected to SQL Server!");
   
   
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

}
void loop(){
  char SQL_command[1024];
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()){
    present = false;
    cardRemoved();
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()){
    Serial.println("no serial");
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = getID();
  content.replace(" ", "");
  char num[8];
  for(int i = 0; i < 11; i++){
    num[i] = content[i];
  }
  // TODO: Implement method of adding new cards
  char *message = "UPDATE rfid.cards SET last_used = NOW() WHERE card_num = \"%s\";";;
  
 
  sprintf(query, message, num);

  Serial.print("Sending statement: ");
  Serial.println(query);
  cur_mem.execute(query);

  delay(500);
} 

void cardPlaced(){
//  if(locked){
//    Serial.println("Unlocking computer");
//    Keyboard.press(Enter);                     
//    Keyboard.release(Enter);                   
//    delay(500);
//    Keyboard.print("ft19");                    
//    Keyboard.releaseAll();
//    delay(100);
//    Keyboard.press(Enter);
//    Keyboard.releaseAll();
//    locked = false;
//  }
}

void cardRemoved(){
//  if(!locked){
//    Serial.println("Locking computer");
//    Keyboard.press(KEY_LEFT_GUI);              //Press the left windows key.
//    Keyboard.press('l');                       //Press the "l" key.
//    Keyboard.releaseAll();                     //Release all keys.
//    locked = true;
//  }
}

String getID(){
  mfrc522.PICC_ReadCardSerial();
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  return content;
}
