
/*
* Code for cross-fading 3 LEDs, red, green and blue (RGB) 
* To create fades, you need to do two things: 
*  1. Describe the colors you want to be displayed
*  2. List the order you want them to fade in
*
* DESCRIBING A COLOR:
* A color is just an array of three percentages, 0-100, 
*  controlling the red, green and blue LEDs
*
* Red is the red LED at full, blue and green off
*   int red = { 100, 0, 0 }
* Dim white is all three LEDs at 30%
*   int dimWhite = {30, 30, 30}
* etc.
*
* Some common colors are provided below, or make your own
* 
* LISTING THE ORDER:
* In the main part of the program, you need to list the order 
*  you want to colors to appear in, e.g.
*  crossFade(red);
*  crossFade(green);
*  crossFade(blue);
*
* Those colors will appear in that order, fading out of 
*    one color and into the next  
*
* In addition, there are 5 optional settings you can adjust:
* 1. The initial color is set to black (so the first color fades in), but 
*    you can set the initial color to be any other color
* 2. The internal loop runs for 1020 interations; the 'wait' variable
*    sets the approximate duration of a single crossfade. In theory, 
*    a 'wait' of 10 ms should make a crossFade of ~10 seconds. In 
*    practice, the other functions the code is performing slow this 
*    down to ~11 seconds on my board. YMMV.
* 3. If 'repeat' is set to 0, the program will loop indefinitely.
*    if it is set to a number, it will loop that number of times,
*    then stop on the last color in the sequence. (Set 'return' to 1, 
*    and make the last color black if you want it to fade out at the end.)
* 4. There is an optional 'hold' variable, which pasues the 
*    program for 'hold' milliseconds when a color is complete, 
*    but before the next color starts.
* 5. Set the DEBUG flag to 1 if you want debugging output to be
*    sent to the serial monitor.
*
*    The internals of the program aren't complicated, but they
*    are a little fussy -- the inner workings are explained 
*    below the main loop.
*
* April 2007, Clay Shirky <clay.shirky@nyu.edu> 
*/ 

// Output
#include <WiFi.h>
#include <WiFiClient.h>
#include "fauxmoESP.h"
#include <ESP_WiFiManager.h>              //https://github.com/khoih-prog/ESP_WiFiManager
#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

#define rainbowWord "rainbow pyramid"
#define fastrainbowWord "fast rainbow"
#define redWord "red pyramid"
#define blueWord "blue pyramid"
#define greenWord "green pyramid"
#define whiteWord "white pyramid"
#define warmWhiteWord "warm white pyramid"
#define cyanWord "cyan pyramid"
#define purpleWord "purple pyramid"
#define pinkWord "pink pyramid"
#define orangeWord "orange pyramid"
#define yellowWord "yellow pyramid"
#define pastelRainbowWord "pastel rainbow pyramid"

fauxmoESP fauxmo;

bool wifiSaved;
String AP_SSID;
String AP_PASS;
String Router_SSID;
String Router_Pass;

const int buttonPin = 23;

int buttonPushCounter = 0;   // counter for the number of button presses
int buttonPushCounterTemp; //TODO saves the push button state when using alexa 
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

int pastelWhiteValue = 0; //white led color value (used to turn on pastel mode)

bool isRainbow = true;
bool isPastelRainbow = false;
bool isRed = false;
bool isBlue = false;
bool isGreen = false;
bool isWhite = false;
bool isWarmWhite = false;
bool isCyan = false;
bool isPurple = false;
bool isPink = false;
bool isYellow = false;
bool isOrange = false;

//red is blue
//green is green


int redPin = 18;   // Red LED,   connected to digital pin 16
int grnPin = 17;  // Green LED, connected to digital pin 17
int bluPin = 16;  // Blue LED,  connected to digital pin 18
int whitePin = 19; // White LED,  connected to digital pin 19


const int resolution = 8;	// 8 bit pwm resolution (0-255)
const int freq = 5000;		// PWM Frequency
const int redPWMChannel = 0;
const int greenPWMChannel = 1;
const int bluePWMChannel = 2;
const int whitePWMChannel = 3;

// Color arrays
int black[3]  = { 0, 0, 0 };
int white[3]  = { 100, 100, 100 };
int red[3]    = { 100, 0, 0 };
int green[3]  = { 0, 100, 0 };
int blue[3]   = { 0, 0, 100 };
int yellow[3] = { 40, 95, 0 };
int dimWhite[3] = { 30, 30, 30 };
// etc.

// Set initial color
int redVal = black[0];
int grnVal = black[1]; 
int bluVal = black[2];

// int wait = 10;      // 10ms internal crossFade delay; increase for slower fades
int wait = 10;
int hold = 0;       // Optional hold when a color is complete, before the next crossFade
int DEBUG = 1;      // DEBUG counter; if set to 1, will write values back via serial
int loopCount = 60; // How often should DEBUG report?
int repeat = 0;     // How many times should we loop before stopping? (0 for no stop)
int j = 0;          // Loop counter for repeat

// Initialize color variables
int prevR = redVal;
int prevG = grnVal;
int prevB = bluVal;






void buttonLogic() {
  //Button Logic
  buttonState = digitalRead(buttonPin);

// TODO implemnt button push wifi reset after 10 seconds
//  WiFiManager wifiManager;
//  //se o botão foi pressionado
//   if ( digitalRead(buttonPin) == HIGH ) {
//      Serial.println("resetar"); //tenta abrir o portal
//      if(!wifiManager.startConfigPortal("ESP_AP", "12345678") ){
//        Serial.println("Falha ao conectar");
//        delay(2000);
//        ESP.restart();
//        delay(1000);
//      }
//      Serial.println("Conectou ESP_AP!!!");
//   }

  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {

      isRainbow = false;
      isPastelRainbow = false;
  	  isRed = false;
  	  isGreen = false;
 	    isBlue = false;
  	  isWhite = false;
  	  isWarmWhite = false;
	    isCyan = false;
	    isPurple = false;
	    isPink = false;
      isOrange = false;
      isYellow = false;


      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      if (buttonPushCounter > 12) {
      	buttonPushCounter = 0;
      }
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);

    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(100);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
  }

void alexaSetup() {
  fauxmo.createServer(true); // not needed, this is the default value
  fauxmo.setPort(80); // This is required for gen3 alexa devices

  // You have to call enable(true) once you have a WiFi connection
  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);
  Serial.println("fauxmo enabled");
  // You can use different ways to invoke alexa to modify the devices state:
  // "Alexa, turn "color" on"
  // "Alexa, turn " color off"
  
  // Add virtual devices
  fauxmo.addDevice(rainbowWord);
  fauxmo.addDevice(pastelRainbowWord);
  fauxmo.addDevice(redWord);
  fauxmo.addDevice(greenWord);
  fauxmo.addDevice(blueWord);
  fauxmo.addDevice(whiteWord);
  fauxmo.addDevice(warmWhiteWord);
  fauxmo.addDevice(cyanWord);
  fauxmo.addDevice(purpleWord);
  fauxmo.addDevice(pinkWord);
  fauxmo.addDevice(orangeWord);
  fauxmo.addDevice(yellowWord);




  Serial.println("devices enabled");
}

void configModeCallback (ESP_WiFiManager *myESP_WiFiManager) {

  Serial.print("Entered config mode with ");
  Serial.println("AP_SSID : " + myESP_WiFiManager->getConfigPortalSSID() + " and AP_PASS = " + myESP_WiFiManager->getConfigPortalPW());

  Serial.println(WiFi.softAPIP());
}

void saveConfigCallback (void) {
//  Serial.println("Should save config");
  wifiSaved = true;
  Serial.println ("Configuration saved");
  Serial.println ("local ip" + WiFi.localIP());
  Serial.println ("ap ip " + WiFi.softAPIP ()); // print the IP of the AP

}


// Wi-Fi Connection
void wifiSetup() {
    Serial.println("\nStarting AutoConnectWithFeedBack");

  // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
  //ESP_WiFiManager ESP_wifiManager;
  // Use this to personalize DHCP hostname (RFC952 conformed)
  ESP_WiFiManager ESP_wifiManager("CameronHighPowerLED");

  //reset settings - for testing
  //ESP_wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  ESP_wifiManager.setAPCallback(configModeCallback);
  ESP_wifiManager.setSaveConfigCallback(saveConfigCallback);

  ESP_wifiManager.setDebugOutput(true);

  //set custom ip for portal
  ESP_wifiManager.setAPStaticIPConfig(IPAddress(42, 42, 42, 42), IPAddress(42, 42, 42, 42), IPAddress(255, 255, 255, 0));

  ESP_wifiManager.setMinimumSignalQuality(-1);
  // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5+
//  ESP_wifiManager.setSTAStaticIPConfig(IPAddress(192, 168, 86, 114), IPAddress(192, 168, 86, 1), IPAddress(255, 255, 255, 0));

  // We can't use WiFi.SSID() in ESP32 as it's only valid after connected.
  // SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
  // Have to create a new function to store in EEPROM/SPIFFS for this purpose
  Router_SSID = ESP_wifiManager.WiFi_SSID();
  Router_Pass = ESP_wifiManager.WiFi_Pass();

  //Remove this line if you do not want to see WiFi password printed
  Serial.println("Stored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);

  if (Router_SSID != "")
  {
    ESP_wifiManager.setConfigPortalTimeout(25); //If no access point name has been previously entered disable timeout.
    Serial.println("Got stored Credentials. Timeout 25s");
  }
  else
  {
    Serial.println("No stored Credentials. No timeout");
  }

  String chipID = String(ESP_getChipId(), HEX);
  chipID.toUpperCase();

  // SSID and PW for Config Portal
  AP_SSID = "HIghPowerLED-" + chipID;
  AP_PASS = "";

  // Get Router SSID and PASS from EEPROM, then open Config portal AP named "ESP_XXXXXX_AutoConnectAP" and PW "MyESP_XXXXXX"
  // 1) If got stored Credentials, Config portal timeout is 60s
  // 2) If no stored Credentials, stay in Config portal until get WiFi Credentials
  ESP_wifiManager.autoConnect(AP_SSID.c_str(), AP_PASS.c_str());
  //or use this for Config portal AP named "ESP_XXXXXX" and NULL password
  //ESP_wifiManager.autoConnect();

  //if you get here you have connected to the WiFi
  Serial.println("WiFi connected");
  
} 
//  // Set WIFI module to STA mode
//  WiFi.mode(WIFI_STA);
//
//  // Connect
//  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
//  WiFi.begin(WIFI_SSID, WIFI_PASS);
//
//  // Wait
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print(".");
//    delay(100);
//  }
//  Serial.println();
//
//  // Connected!
//  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", 
//    WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());



void setup()
{

  pinMode(buttonPin, INPUT);

  Serial.begin(9600);  // ...set up the serial ouput
  
  // Wi-Fi connection
  wifiSetup();
  delay(1000);
  // Alexa setup
  alexaSetup();
  
  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    buttonPushCounter = 0;
    // Callback when a command from Alexa is received. 
    // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
    // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
    // Just remember not to delay too much here, this is a callback, exit as soon as possible.
    // If you have to do something more involved here set a flag and process it in your main loop.
        
    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
      if ( (strcmp(device_name, rainbowWord) == 0) ) {
        if (state) {
        isRainbow = true;
      }
        else {
            isRainbow = false; 
          }
    }
      else {
          isRainbow = false;
        }

      if ( (strcmp(device_name, redWord) == 0) ) {
        if (state) {
        isRed = true;
      }
        else {
            isRed = false; 
          } 
    }
    else {
          isRed = false;
        }

      if ( (strcmp(device_name, greenWord) == 0) ) {
        if (state) {
        isGreen = true;
      }
        else {
            isGreen = false; 
          } 
    }
      else {
          isGreen = false;
        }

      if ( (strcmp(device_name, blueWord) == 0) ) {
        if (state) {
        isBlue = true;
      }
        else {
            isBlue = false; 
          }  
    }
      else {
          isBlue = false;
        }

    if ( (strcmp(device_name, whiteWord) == 0) ) {
        if (state) {
        isWhite = true;
      }
        else {
            isWhite = false; 
          }  
    }
      else {
          isWhite = false;
        }

    if ( (strcmp(device_name, warmWhiteWord) == 0) ) {
      // this just sets a variable that the main loop() does something about
        if (state) {
        isWarmWhite = true;
      }
        else {
          isWarmWhite = false;
        } 
    }
      else {
          isWarmWhite = false;
        }

    if ( (strcmp(device_name, cyanWord) == 0) ) {
      // this just sets a variable that the main loop() does something about
        if (state) {
        isCyan = true;
      }
        else {
          isCyan = false;
        } 
    }
      else {
          isCyan = false;
        }

    if ( (strcmp(device_name, purpleWord) == 0) ) {
      // this just sets a variable that the main loop() does something about
        if (state) {
        isPurple = true;
      }
        else {
          isPurple = false;
        } 
    }
      else {
          isPurple = false;
        }

    if ( (strcmp(device_name, pinkWord) == 0) ) {
      // this just sets a variable that the main loop() does something about
        if (state) {
        isPink = true;
      }
        else {
          isPink = false;
        }
    }
      else {
          isPink = false;
        }

    if ( (strcmp(device_name, orangeWord) == 0) ) {
      // this just sets a variable that the main loop() does something about
        if (state) {
        isOrange = true;
      }
        else {
          isOrange = false;
        }
    }
      else {
          isOrange = false;
        }

    if ( (strcmp(device_name, yellowWord) == 0) ) {
      // this just sets a variable that the main loop() does something about
        if (state) {
        isYellow = true;
      }
        else {
          isYellow = false;
        }
    }
      else {
          isYellow = false;
        }

    if ( (strcmp(device_name, pastelRainbowWord) == 0) ) {
      // this just sets a variable that the main loop() does something about
        if (state) {
        isPastelRainbow = true;
      }
        else {
          isPastelRainbow = false;
        }
    }
      else {
          isPastelRainbow = false;
        }
  });




  // Set up the LED outputs
  ledcSetup(redPWMChannel, freq, resolution);	//Configures PWM Channels for LEDs (1 different pwm channel per led)
  ledcSetup(greenPWMChannel, freq, resolution);  
  ledcSetup(bluePWMChannel, freq, resolution);  
  ledcSetup(whitePWMChannel, freq, resolution);  

  ledcAttachPin(redPin, redPWMChannel);			//Assigns pins to pwm channels
  ledcAttachPin(grnPin, greenPWMChannel);
  ledcAttachPin(bluPin, bluePWMChannel);
  ledcAttachPin(whitePin, whitePWMChannel);




}

// Main program
void loop()
{

  fauxmo.handle();   //Alexa discovery
  

  buttonLogic(); //Button Logic



 
  
//runs color/pattern if booleans are true or button is pressed a certain amount of times
  if (isRainbow == true || buttonPushCounter == 1) {
    isPastelRainbow = false;
	  isRed = false;
	  isGreen = false;
	  isBlue = false;
	  isWhite = false;
	  isWarmWhite = false;
	  isCyan = false;
	  isPurple = false;
	  isPink = false;
    isOrange = false;
    isYellow = false;
	  crossFade(red);
	  crossFade(green);
	  crossFade(blue);
	  crossFade(yellow);

  Serial.println("color set to rainbow");
  }

  if (isPastelRainbow == true || buttonPushCounter == 2) {
    isRainbow = false;
    isRed = false;
    isGreen = false;
    isBlue = false;
    isWhite = false;
    isWarmWhite = false;
    isCyan = false;
    isPurple = false;
    isPink = false;
    isOrange = false;
    isYellow = false;
    pastelCrossFade(red);
    pastelCrossFade(green);
    pastelCrossFade(blue);
    pastelCrossFade(yellow);

  Serial.println("color set to pastel rainbow");
  }
 
  else if (isRed == true || buttonPushCounter == 3) {
	  isRainbow = false;
    isPastelRainbow = false;
	  isGreen = false;
	  isBlue = false;
	  isWhite = false;
	  isWarmWhite = false;
  	isCyan = false;
    isPurple = false;
    isPink = false;
    isOrange = false;
    isYellow = false;
    color("red");
  // Serial.println("color set to red");
  }

  else if (isGreen == true || buttonPushCounter == 4) {
	  isRed = false;
    isPastelRainbow = false;
	  isRainbow = false;
	  isBlue = false;
	  isWhite = false;
	  isWarmWhite = false;
	  isCyan = false;
	  isPurple = false;
	  isPink = false;
    isOrange = false;
    isYellow = false;
	  color("green");
  // Serial.println("color set to green");

  }

  else if (isBlue == true || buttonPushCounter == 5) {
  	isPastelRainbow = false;
	  isRed = false;
	  isGreen = false;
	  isRainbow = false;
	  isWhite = false;
	  isWarmWhite = false;
	  isCyan = false;
	  isPurple = false;
	  isPink = false;
    isOrange = false;
    isYellow = false;
	  color ("blue");
  // Serial.println("color set to blue");

  }

  else if (isWhite == true || buttonPushCounter == 6) {
  	isPastelRainbow = false;
	  isRed = false;
	  isGreen = false;
	  isRainbow = false;
	  isBlue = false;
	  isWarmWhite = false;
	  isCyan = false;
	  isPurple = false;
	  isPink = false;
    isOrange = false;
    isYellow = false;
	  color ("white");

  }

  else if (isWarmWhite == true || buttonPushCounter == 7) {
  	isPastelRainbow = false;
	  isRed = false;
	  isGreen = false;
	  isBlue = false;
	  isRainbow = false;
	  isWhite = false;
	  isCyan = false;
	  isPurple = false;
	  isPink = false;
    isOrange = false;
    isYellow = false;
	  color ("warmwhite");

  }

  else if (isCyan == true || buttonPushCounter == 8) {
  	isPastelRainbow = false;
	  isRed = false;
	  isGreen = false;
	  isRainbow = false;
	  isBlue = false;
	  isWhite = false;
	  isWarmWhite = false;
	  isPurple = false;
	  isPink = false;
    isOrange = false;
    isYellow = false;
	  color ("cyan");

  }

    else if (isPurple == true || buttonPushCounter == 9) {
  	isPastelRainbow = false;
	  isRed = false;
	  isGreen = false;
	  isRainbow = false;
	  isBlue = false;
	  isWhite = false;
	  isWarmWhite = false;
	  isCyan = false;
	  isPink = false;
    isOrange = false;
    isYellow = false;
	  color ("purple");
  }

  else if (isPink == true || buttonPushCounter == 10) {
    isPastelRainbow = false;
	  isRed = false;
	  isGreen = false;
	  isRainbow = false;
	  isBlue = false;
	  isWhite = false;
	  isWarmWhite = false;
	  isCyan = false;
	  isPurple = false;
    isOrange = false;
    isYellow = false;
	  color ("pink");

  }

  else if (isOrange == true || buttonPushCounter == 11) {
    isPastelRainbow = false;
    isRed = false;
    isGreen = false;
    isRainbow = false;
    isBlue = false;
    isWhite = false;
    isWarmWhite = false;
    isCyan = false;
    isPurple = false;
    isYellow = false;
    color ("orange");

  }

  else if (isYellow == true || buttonPushCounter == 12) {
    isPastelRainbow = false;
    isRed = false;
    isGreen = false;
    isRainbow = false;
    isBlue = false;
    isWhite = false;
    isWarmWhite = false;
    isCyan = false;
    isPurple = false;
    isOrange = false;
    color ("yellow");

  }


//display black if none of the booleans are true

  else {
  	color("black");
  }

  if (repeat) { // Do we loop a finite number of times?
    j += 1;
    if (j >= repeat) { // Are we there yet?
      exit(j);         // If so, stop.
    }
  }
}

/* BELOW THIS LINE IS THE MATH -- YOU SHOULDN'T NEED TO CHANGE THIS FOR THE BASICS
* 
* The program works like this:
* Imagine a crossfade that moves the red LED from 0-10, 
*   the green from 0-5, and the blue from 10 to 7, in
*   ten steps.
*   We'd want to count the 10 steps and increase or 
*   decrease color values in evenly stepped increments.
*   Imagine a + indicates raising a value by 1, and a -
*   equals lowering it. Our 10 step fade would look like:
* 
*   1 2 3 4 5 6 7 8 9 10
* R + + + + + + + + + +
* G   +   +   +   +   +
* B     -     -     -
* 
* The red rises from 0 to 10 in ten steps, the green from 
* 0-5 in 5 steps, and the blue falls from 10 to 7 in three steps.
* 
* In the real program, the color percentages are converted to 
* 0-255 values, and there are 1020 steps (255*4).
* 
* To figure out how big a step there should be between one up- or
* down-tick of one of the LED values, we call calculateStep(), 
* which calculates the absolute gap between the start and end values, 
* and then divides that gap by 1020 to determine the size of the step  
* between adjustments in the value.
*/

int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero, 
    step = 1020/step;              //   divide by 1020
  } 
  return step;
}

/* The next function is calculateVal. When the loop value, i,
*  reaches the step size appropriate for one of the
*  colors, it increases or decreases the value of that color by 1. 
*  (R, G, and B are each calculated separately.)
*/

int calculateVal(int step, int val, int i) {

  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;           
    } 
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    } 
  }
  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  } 
  else if (val < 0) {
    val = 0;
  }
  return val;
}

/* crossFade() converts the percentage colors to a 
*  0-255 range, then loops 1020 times, checking to see if  
*  the value needs to be updated each time, then writing
*  the color values to the correct pins.
*/

void color(String color) {

	if (color == "black") {
		ledcWrite (redPWMChannel, 0);
		ledcWrite (greenPWMChannel, 0);
		ledcWrite (bluePWMChannel, 0);
		ledcWrite (whitePWMChannel, 0);
	}

	else if (color == "red") {
		ledcWrite (redPWMChannel, 255);
		ledcWrite (greenPWMChannel, 0);
		ledcWrite (bluePWMChannel, 0);
		ledcWrite (whitePWMChannel, 0);


	}
	else if (color == "green") {
		ledcWrite (redPWMChannel, 0);
		ledcWrite (greenPWMChannel, 255);
		ledcWrite (bluePWMChannel, 0);
		ledcWrite (whitePWMChannel, 0);


	}

	else if (color == "blue") {
		ledcWrite (redPWMChannel, 0);
		ledcWrite (greenPWMChannel, 0);
		ledcWrite (bluePWMChannel, 255);
		ledcWrite (whitePWMChannel, 0);


	}

	else if (color == "white") {
		ledcWrite (redPWMChannel, 0);
		ledcWrite (greenPWMChannel, 0);
		ledcWrite (bluePWMChannel, 0);
		ledcWrite (whitePWMChannel, 255);

	}

	else if (color == "warmwhite") {
		ledcWrite (redPWMChannel, 255);
		ledcWrite (greenPWMChannel, 128);
		ledcWrite (bluePWMChannel, 0);
		ledcWrite (whitePWMChannel, 77);

	}

	else if (color == "cyan") {
		ledcWrite (redPWMChannel, 0);
		ledcWrite (greenPWMChannel, 255);
		ledcWrite (bluePWMChannel, 190);
		ledcWrite (whitePWMChannel, 15);

	}

	else if (color == "purple") {
		ledcWrite (redPWMChannel, 111);
		ledcWrite (greenPWMChannel, 0);
		ledcWrite (bluePWMChannel, 255);
		ledcWrite (whitePWMChannel, 0);

	}

	else if (color == "pink") {
		ledcWrite (redPWMChannel, 255);
		ledcWrite (greenPWMChannel, 0);
		ledcWrite (bluePWMChannel, 128);
		ledcWrite (whitePWMChannel, 75);

	}

  else if (color == "orange") 
  {
    ledcWrite (redPWMChannel, 255);
    ledcWrite (greenPWMChannel, 80);
    ledcWrite (bluePWMChannel, 0);
    ledcWrite (whitePWMChannel, 0);

  }

    else if (color == "yellow") 
  {
    ledcWrite (redPWMChannel, 255);
    ledcWrite (greenPWMChannel, 255);
    ledcWrite (bluePWMChannel, 0);
    ledcWrite (whitePWMChannel, 0);

  }
  
}

void crossFade(int color[3]) {

    
  // Convert to 0-255
  int R = (color[0] * 255) / 100;
  int G = (color[1] * 255) / 100;
  int B = (color[2] * 255) / 100;
 
  int stepR = calculateStep(prevR, R);
  int stepG = calculateStep(prevG, G); 
  int stepB = calculateStep(prevB, B);
  
  for (int i = 0; i <= 1020 && (isRainbow == true || buttonPushCounter == 1); i++) {
    
    buttonLogic(); //button logic has to be in the rainbow thread in order to change in the middle of rainbow pattern
    fauxmo.handle(); //Alexa discovery also has to be in the rainbow thread in order for discovery to work in the middle of rainbow pattern
    
    redVal = calculateVal(stepR, redVal, i);
    grnVal = calculateVal(stepG, grnVal, i);
    bluVal = calculateVal(stepB, bluVal, i);


    // analogWrite(redPin, redVal);   
    // analogWrite(grnPin, grnVal);      
    // analogWrite(bluPin, bluVal); 

    ledcWrite(redPWMChannel, redVal);	// Write current values to LED pins
    ledcWrite(greenPWMChannel, grnVal);
    ledcWrite(bluePWMChannel, bluVal);
    ledcWrite(whitePWMChannel, 0);

    delay(wait); // Pause for 'wait' milliseconds before resuming the loop

    if (DEBUG) { // If we want serial output, print it at the 
      if (i == 0 or i % loopCount == 0) { // beginning, and every loopCount times
        // Serial.print("Loop/RGB: #");
        // Serial.print(i);
        // Serial.print(" | ");
        // Serial.print(redVal);
        // Serial.print(" / ");
        // Serial.print(grnVal);
        // Serial.print(" / ");  
        // Serial.println(bluVal); 
      } 
      DEBUG += 1;
    }
  }
  // Update current values for next loop
  prevR = redVal; 
  prevG = grnVal; 
  prevB = bluVal;
  delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}

void pastelCrossFade(int color[3]) {

    
  // Convert to 0-255
  int R = (color[0] * 255) / 100;
  int G = (color[1] * 255) / 100;
  int B = (color[2] * 255) / 100;
 
  int stepR = calculateStep(prevR, R);
  int stepG = calculateStep(prevG, G); 
  int stepB = calculateStep(prevB, B);
  
  for (int i = 0; i <= 1020 && (isPastelRainbow == true || buttonPushCounter == 1); i++) {
    
    buttonLogic(); //button logic has to be in the rainbow thread in order to change in the middle of rainbow pattern
    fauxmo.handle(); //Alexa discovery also has to be in the rainbow thread in order for discovery to work in the middle of rainbow pattern
    
    redVal = calculateVal(stepR, redVal, i);
    grnVal = calculateVal(stepG, grnVal, i);
    bluVal = calculateVal(stepB, bluVal, i);


    // analogWrite(redPin, redVal);   
    // analogWrite(grnPin, grnVal);      
    // analogWrite(bluPin, bluVal); 

    ledcWrite(redPWMChannel, redVal); // Write current values to LED pins
    ledcWrite(greenPWMChannel, grnVal);
    ledcWrite(bluePWMChannel, bluVal);
    ledcWrite(whitePWMChannel, 35);

    delay(wait); // Pause for 'wait' milliseconds before resuming the loop

    if (DEBUG) { // If we want serial output, print it at the 
      if (i == 0 or i % loopCount == 0) { // beginning, and every loopCount times
        // Serial.print("Loop/RGB: #");
        // Serial.print(i);
        // Serial.print(" | ");
        // Serial.print(redVal);
        // Serial.print(" / ");
        // Serial.print(grnVal);
        // Serial.print(" / ");  
        // Serial.println(bluVal); 
      } 
      DEBUG += 1;
    }
  }
  // Update current values for next loop
  prevR = redVal; 
  prevG = grnVal; 
  prevB = bluVal;
  delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}
