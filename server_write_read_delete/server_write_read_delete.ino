#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "Ethernet.h"


WiFiClient client;
MySQL_Connection conn((Client *)&client);
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)

// Network credentials
const char* ssid = "";
const char* password = "";
char* page = "";

//SQL Database credentials
IPAddress server_addr();
char* user = "";
char* pass = "";

char query[128];

int led = 16;
int ledState = 0;

int c = 0;

MySQL_Cursor cur_mem = MySQL_Cursor(&conn);
  
void setup(void) {
   //the HTML of the web page
   page = "<h1>NodeMCU Test Web Server</h1><p><a href=\"SerialPrint\"><button>Serial Print</button></a>&nbsp;</p>";

   Serial.begin(115200);
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
   
   server.on("/", []() {
      server.send(200, "text/html", page);
   });
   server.on("/SerialPrint", []() {
    Serial.println("Request recieved");
    server.send(200, "text/html", page);
    delay(500);
   });
   server.begin();
   Serial.println("Web server started!");

   //connect to server
   Serial.println("Connecting to database");

   while (!conn.connect(server_addr, 3306,user,pass)) {
     delay(1000);
     Serial.print (".");
   }
   Serial.println("");
   Serial.println("Connected to SQL Server!");

  pinMode(led, OUTPUT);

  Serial.println("Setup Complete.");
}

void loop(void) {
  
  
  server.handleClient();

  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);


  char* query = "INSERT INTO test.temp (name, age) VALUES (%s, %d);";
  char INSERT_SQL[1024];
  
  sprintf(INSERT_SQL,query,"\"Mark Falconer\"", c);
  Serial.print("Sending command... ");
  Serial.println(INSERT_SQL);
  // Execute the query
  cur_mem.execute(INSERT_SQL);


  query = "SELECT * FROM test.temp;";
  sprintf(INSERT_SQL, query);
  Serial.print("Sending command... ");
  Serial.println(INSERT_SQL);
  // Execute the query
  cur_mem.execute(INSERT_SQL);


  

  
  column_names *cols = cur_mem.get_columns();
  row_values *row = NULL;
  do {
    row = cur_mem.get_next_row();
    if (row != NULL) {
      for(int i = 0; i < cols->num_fields; i++){
        Serial.print(row->values[i]);
        if (i < cols->num_fields-1) Serial.print(", ");
      }
      Serial.println();
    }
  } while (row != NULL);


  

   //Dont fill database too much
   if(c == 10){
    query = "DELETE FROM test.temp WHERE name = 'Mark Falconer';";
    sprintf(INSERT_SQL,query,"\"Mark Falconer\"", 21);
    Serial.print("Sending command... ");
    Serial.println(INSERT_SQL);
    // Execute the query
    cur_mem.execute(INSERT_SQL);
    c = 1;
   }else{
    c++;
   }
    
   // Deleting the cursor also frees up memory used
   cur_mem.close();

   Serial.println();
   delay(1000);
}
