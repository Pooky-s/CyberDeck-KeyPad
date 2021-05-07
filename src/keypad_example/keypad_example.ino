/*TODO :
- add function to convert website input to values as HID_KEY_... by code
- add function to convert keys read from storage to a value readable by human
- add multikey macros capability 
- optimize website 
- add comments
*/

#include "Arduino.h"
#include "hidkeyboard.h"
#include "Keypad.h"
#include "WiFi.h"
#include "AsyncTCP.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "FastLED.h"

/* LED vars */
#define NUM_LEDS 1
#define DATA_PIN 18
CRGB leds[NUM_LEDS];

/* WIFI vars*/

const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

/* Setup vars */
//Used to check if the keypad has been initialized.
int initialized = 0;
int tempKeys[12];
//Used to check changes made to setup. Default is true to get saved Setup.
bool written_to = true;


//Default keys setup
//Check <arduino-esp32-dir>\tools\sdk\esp32s2\include\tinyusb\tinyusb\src\class\hid\hid.h for more keys.
int keysVars[12] = {
  HID_KEY_1,
  HID_KEY_2,
  HID_KEY_3,
  HID_KEY_4,
  HID_KEY_5,
  HID_KEY_6,
  HID_KEY_7,
  HID_KEY_8,
  HID_KEY_9,
  HID_KEY_NONE,
  HID_KEY_0,
  HID_KEY_KEYPAD_MULTIPLY
};
//Used to call files.
char filename[50];

/* Webserver vars */

AsyncWebServer server(80);

// HTML web page to edit setup.
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script>
  <style>
      html 
      { 
        font-family: Helvetica;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      body
      {
        margin-top: 50px;
      } 
      h1 
      {
        color: #444444;
        margin: 50px auto 30px;
      }
      h3 
      {
        color: #444444;
        margin-bottom: 50px;
      }
      input
      {
        display: block;
        background-color: #ffffff;
        border: none;
        padding: 10px 10px;
        text-decoration: none;
        margin: 5px 10px 10px;
        cursor: vertical-text;
        border-radius: 10px;
      }
      #keypad
      {
        display: block;
        background-color: #f0f0ff;
        padding: 10px 10px;
        margin: 20px 20px auto;
        border-radius: 10px;
      }
      .button 
      {
        display: block;
        background-color: #3498db;
        border: none;
        color: white;
        padding: 20px 10px;
        text-decoration: none;
        font-size: 25px;
        margin: 5px auto 35px;
        cursor: pointer;
        border-radius: 4px;
      }
      .button:hover
      {
        background-color: #2980b9;
      }
      p 
      {
        font-size: 14px;
        color: #888;
        margin-bottom: 10px;
      }
      table
      {
        display: flex;
        flex-wrap: wrap;
        justify-content: space-around;
      }
  </style>
</head><body>
  <h1>Cyberdeck Keypad</h1>
  <div id="keypad">
  <form action="/get" target="hidden-form">
    <!--inputString (current value %inputString%): <input type="text" name="inputString">
    <input class="button" type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    inputInt (current value %inputInt%): <input type="number" name="inputInt">
    <input class="button" type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    inputFloat (current value %inputFloat%): <input type="number" name="inputFloat">...-->
  <datalist id="keys">
    <option value="CONTROL">
    <option value="SHIFT">
    <option value="UP">
    <option value="DOWN">
    <option value="ALT">
  </datalist>
  <table id=#allkeys>
    <tr>
      <td>Key 1 (%key1%):<input list="keys" name="key1"></td>
      <td>Key 2 (%key2%):<input list="keys" name="key2"></td>
      <td>Key 3 (%key3%):<input list="keys" name="key3"></td>
    </tr>
    <tr>
      <td>Key 4 (%key4%):<input list="keys" name="key4"></td>
      <td>Key 5 (%key5%):<input list="keys" name="key5"></td>
      <td>Key 6 (%key6%):<input list="keys" name="key6"></td>
    </tr>
    <tr>
      <td>Key 7 (%key7%):<input list="keys" name="key7"></td>
      <td>Key 8 (%key8%):<input list="keys" name="key8"></td>
      <td>Key 9 (%key9%):<input list="keys" name="key9"></td>
    </tr>
    <tr>
      <td>Key 10 (%keyA%):<input list="keys" name="keyA"></td>
      <td>Key 11 (%keyB%):<input list="keys" name="keyB"></td>
      <td>Key 12 (%keyC%):<input list="keys" name="keyC"></td>
    </tr>
  </table>
    <input class="button" type="submit" value="Submit" onclick="submitMessage()">
  </form>
  </div>
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";

/* HID vars */

//Matrix initialisation.
const byte ROWS = 3; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','4','7','#'},
  {'2','5','8','0'},
  {'3','6','9','*'}
};

byte rowPins[ROWS] = {10, 11, 12}; //connect to the row pinouts of the kpd
//byte colPins[COLS] = {23, 24, 25, 26}; //connect to the column pinouts of the kpd
byte colPins[COLS] = {3, 2, 1, 0}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//Tasks-per-second calculator vars initialisation.
unsigned long loopCount;
unsigned long startTime;
String msg;

HIDkeyboard dev;

/* HID's functions */

class MyHIDCallbacks: public HIDCallbacks{
  void onData(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    Serial.printf("ID: %d, type: %d, size: %d\n", report_id, (int)report_type, bufsize);  
     for (size_t i = 0; i < bufsize; i++)
    {
        Serial.printf("%d\n", buffer[i]);
    }   
  }
};

void dataCB(uint8_t report_id, uint8_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    for (size_t i = 0; i < bufsize; i++)
    {
        Serial.printf("%d\n", buffer[i]);
        Serial.printf("%c\n", buffer[i]);
    }
}

/*webserver's functions*/

void notFound(AsyncWebServerRequest *request) {
  Serial.printf("Error 404. \n");
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

int populateKeys(int initialized){
  //Serial.printf("Populating keys. \n ");
  for(int j=0;j<12;j++)
  {
    sprintf(filename,"/key%d.txt",j+1);
    tempKeys[j] = readFile(SPIFFS, filename).toInt();
    if(tempKeys[j]==0){initialized+=1;  Serial.print(initialized);}
  }
  return initialized;
}

// Replaces placeholder with stored values
String processor(const String& var){
  //Serial.printf("Processing value. \n");
  //Serial.println(var);
  switch(var[3]){
    case '1': return readFile(SPIFFS, "/key1.txt");
      break;  
    case '2': return readFile(SPIFFS, "/key2.txt");
      break;  
    case '3': return readFile(SPIFFS, "/key3.txt");
      break;  
    case '4': return readFile(SPIFFS, "/key4.txt");
      break;  
    case '5': return readFile(SPIFFS, "/key5.txt");
      break;  
    case '6': return readFile(SPIFFS, "/key6.txt");
      break;        
    case '7': return readFile(SPIFFS, "/key7.txt");
      break;  
    case '8': return readFile(SPIFFS, "/key8.txt");
      break;  
    case '9': return readFile(SPIFFS, "/key9.txt");
      break;  
    case 'A': return readFile(SPIFFS, "/key10.txt");
      break;  
    case 'B': return readFile(SPIFFS, "/key11.txt");
      break;  
    case 'C': return readFile(SPIFFS, "/key12.txt");
      break; 
  }
  return String();
}

/* Storage functions */
//Used to get keys from setup. 
int getKeyMapped(const char key){
    switch(key){
//    case '1': return keysVars[0];
//      break;  
//    case '2': return keysVars[1];
//      break;  
//    case '3': return keysVars[2];
//      break;  
//    case '4': return keysVars[3];
//      break;  
//    case '5': return keysVars[4];
//      break;  
//    case '6': return keysVars[5];
//      break;        
//    case '7': return keysVars[6];
//      break;  
//    case '8': return keysVars[7];
//      break;  
//    case '9': return keysVars[8];
//      break;  
//    case '*': return keysVars[9];
//      break;  
//    case '0': return keysVars[10];
//      break;  
//    case '#': return keysVars[11];
//      break; 
    case '1': return tempKeys[0];
      break;  
    case '2': return tempKeys[1];
      break;  
    case '3': return tempKeys[2];
      break;  
    case '4': return tempKeys[3];
      break;  
    case '5': return tempKeys[4];
      break;  
    case '6': return tempKeys[5];
      break;        
    case '7': return tempKeys[6];
      break;  
    case '8': return tempKeys[7];
      break;  
    case '9': return tempKeys[8];
      break;  
    case '#': return tempKeys[9];
      break;  
    case '0': return tempKeys[10];
      break;  
    case '*': return tempKeys[11];
      break; 
  }
  return 0;
}

/*default functions*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  loopCount = 0;
  startTime = millis();
  msg = "";
  dev.begin();
  dev.setCallbacks(new MyHIDCallbacks());

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //Serial.printf("Sent webpage.");
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    //Serial.printf("Received key changing.");
    String inputMessage;
    int args = request->args();
    for(int i=0;i<args;i++){
      Serial.printf("ARG[%s]: '%s'\n", request->argName(i).c_str(), request->arg(i).c_str());
      if ((request->arg(i)).length() > 0 ){
        //int n = sprintf(filename,"/%s%s",request->argName(i).c_str(),ext);
        //Serial.println(filename);
        inputMessage = request->arg(i);
        sprintf(filename,"/key%d.txt",i+1);
        //Serial.println(filename);
        Serial.printf("Writing %s to %s.",inputMessage.c_str(),filename);
        writeFile(SPIFFS, filename, inputMessage.c_str());
      }
    }
    //Serial.println(inputMessage);
    written_to = true;
    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  delay(500);
}

void loop() {
  if(written_to){
    // To access your stored values on inputString, inputInt, inputFloat
    initialized = 0;
    leds[0] = CRGB::Green;
    FastLED.show();
    
    /*int yourInputString = readFile(SPIFFS, "/key1.txt").toInt();
    Serial.print("*** Your key1: ");
    Serial.println(yourInputString);
    
    int yourInputInt = readFile(SPIFFS, "/key2.txt").toInt();
    Serial.print("*** Your key2: ");
    Serial.println(yourInputInt);
    
    int yourInputFloat = readFile(SPIFFS, "/key3.txt").toInt();
    Serial.print("*** Your key3: ");
    Serial.println(yourInputFloat);*/

    initialized = populateKeys(initialized);
    
//    for(int j=0;j<12;j++)
//    {
//      sprintf(filename,"/key%d.txt",j+1);
//      Serial.println(filename);
//      tempKeys[j] = readFile(SPIFFS, filename).toInt();
//      if(tempKeys[j]==0){initialized+=1;  Serial.print(initialized);}
//    }

    if(initialized!=12){
      Serial.print("Already initialized.");
      written_to = false;
    }
    else{
      Serial.print("Not initialized... Initializing.");
      for(int j=0;j<12;j++)
      {
        char value[20];
        sprintf(filename,"/key%d.txt",j+1);
        Serial.println(filename);
        Serial.println(keysVars[j]);
        sprintf(value,"%d", keysVars[j]);
        Serial.println(value);
        writeFile(SPIFFS, filename, value);
      }
      populateKeys(initialized);
      written_to = false;
    }
    delay(5000);
    leds[0] = CRGB::Black;
    FastLED.show();
  }
  loopCount++;
  if ( (millis()-startTime)>5000 ) {
    Serial.print("Average loops per second = ");
    Serial.println(loopCount/5);
    startTime = millis();
    loopCount = 0;
  }
  if (kpd.getKeys())
  {
    Serial.printf("Reading key presses \n");
    for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
    {
      if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
      {
        switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
          case PRESSED:
          dev.sendString(String(kpd.key[i].kchar));
          dev.sendKey(getKeyMapped(kpd.key[i].kchar));
          msg = " PRESSED.";
        break;
          case HOLD:
          msg = " HOLD.";
        break;
          case RELEASED:
          msg = " RELEASED.";
        break;
          case IDLE:
          msg = " IDLE.";
          
        }
        Serial.print("Key ");
        Serial.print(kpd.key[i].kchar);
        Serial.println(msg);
      }
    }
  }
}
