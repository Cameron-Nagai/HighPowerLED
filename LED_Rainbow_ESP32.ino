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
#include "fauxmoESP.h"

#define WIFI_SSID "Sue Associates Downstairs"
#define WIFI_PASS "Do you know the way to mayan warrior?"

#define rainbowWord "rainbow"
#define fastrainbowWord "fast rainbow"
#define redWord "red"
#define blueWord "blue"
#define greenWord "green"
#define whiteWord "white"
#define amberWord "amber"
#define cyanWord "cyan"
#define purpleWord "purple"
#define pinkWord "pink"

fauxmoESP fauxmo;

const int buttonPin = 23;

int buttonPushCounter = 1;   // counter for the number of button presses
int buttonPushCounterTemp; //TODO saves the push button state when using alexa 
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

int pastelWhiteValue = 0; //white led color value (used to turn on pastel mode)

bool isRainbow = false;
bool isRed = false;
bool isBlue = false;
bool isGreen = false;
bool isWhite = false;
bool isAmber = false;
bool isCyan = false;
bool isPurple = false;
bool isPink = false;

int redPin = 16;   // Red LED,   connected to digital pin 16
int grnPin = 17;  // Green LED, connected to digital pin 17
int bluPin = 18;  // Blue LED,  connected to digital pin 18
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





// Wi-Fi Connection
void wifiSetup() {
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", 
  	WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void buttonLogic() {
  //Button Logic
  buttonState = digitalRead(buttonPin);

  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {

      isRainbow = false;
  	  isRed = false;
  	  isGreen = false;
 	  isBlue = false;
  	  isWhite = false;
  	  isAmber = false;
	  isCyan = false;
	  isPurple = false;
	  isPink = false;


      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      if (buttonPushCounter > 10) {
      	buttonPushCounter = 1;
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


void setup()
{

  pinMode(buttonPin, INPUT);

  Serial.begin(9600);  // ...set up the serial ouput
  
  // Wi-Fi connection
  wifiSetup();	

  fauxmo.createServer(true); // not needed, this is the default value
  fauxmo.setPort(80); // This is required for gen3 devices

  // You have to call enable(true) once you have a WiFi connection
  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);



  Serial.println("fauxmo enabled");
 

  // You can use different ways to invoke alexa to modify the devices state:
  // "Alexa, turn lamp two on"

  // Add virtual devices
  fauxmo.addDevice(rainbowWord);
  fauxmo.addDevice(redWord);
  fauxmo.addDevice(greenWord);
  fauxmo.addDevice(blueWord);
  fauxmo.addDevice(whiteWord);
  fauxmo.addDevice(amberWord);
  fauxmo.addDevice(cyanWord);
  fauxmo.addDevice(purpleWord);
  fauxmo.addDevice(pinkWord);



  Serial.println("devices enabled");

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
  	buttonPushCounter = 0;
    // Callback when a command from Alexa is received. 
    // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
    // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
    // Just remember not to delay too much here, this is a callback, exit as soon as possible.
    // If you have to do something more involved here set a flag and process it in your main loop.
        
    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
      if ( (strcmp(device_name, rainbowWord) == 0) ) {
      Serial.println("Rainbow switched on by Alexa");
        if (state) {
        isRainbow = true;
        Serial.print("rainbow is true");
      } 
    }
	    else {
	        isRainbow = false;
	      }

      if ( (strcmp(device_name, redWord) == 0) ) {
      Serial.println("Red switched on by Alexa");
        if (state) {
        isRed = true;
        Serial.println("red is true");
      } 
    }
		else {
	        isRed = false;
	      }

      if ( (strcmp(device_name, greenWord) == 0) ) {
      Serial.println("Green switched on by Alexa");
        if (state) {
        isGreen = true;
        Serial.println("green is true");
      } 
    }
	    else {
	        isGreen = false;
	      }

      if ( (strcmp(device_name, blueWord) == 0) ) {
      Serial.println("Blue switched on by Alexa");
        if (state) {
        isBlue = true;
        Serial.println("blue is true");
      } 
    }
	    else {
	        isBlue = false;
	      }

	  if ( (strcmp(device_name, whiteWord) == 0) ) {
      Serial.println("White switched on by Alexa");
        if (state) {
        isWhite = true;
        Serial.println("white is true");
      } 
    }
	    else {
	        isWhite = false;
	      }

    if ( (strcmp(device_name, amberWord) == 0) ) {
      // this just sets a variable that the main loop() does something about
      Serial.println("Amber switched on by Alexa");
        if (state) {
        isAmber = true;
        Serial.println("amber is true");
      } 
    }
      else {
          isAmber = false;
        }

    if ( (strcmp(device_name, cyanWord) == 0) ) {
      // this just sets a variable that the main loop() does something about
      Serial.println("Cyan switched on by Alexa");
        if (state) {
        isCyan = true;
        Serial.println("cyan is true");
      } 
    }
      else {
          isCyan = false;
        }

    if ( (strcmp(device_name, purpleWord) == 0) ) {
      // this just sets a variable that the main loop() does something about
      Serial.println("Purple switched on by Alexa");
        if (state) {
        isPurple = true;
        Serial.println("purple is true");
      } 
    }
      else {
          isPurple = false;
        }

    if ( (strcmp(device_name, pinkWord) == 0) ) {
      // this just sets a variable that the main loop() does something about
      Serial.println("pink switched on by Alexa");
        if (state) {
        isPink = true;
        Serial.println("pink is true");
      } 
    }
      else {
          isPink = false;
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


  //Booleans for checking if alexa/button has activated a certain color/pattern

 //  if (isRainbow == true) {
	// Serial.println("Rainbow is true");
 //  }
 
 //  if (isRed == true) {
 //  	Serial.println("red is true");
 //  }


 //  else if (isGreen == true) {
 //  	Serial.println("green is true");
 //  }

 //   else if (isBlue == true) {
 //  	Serial.println("blue is true");
 //  }

 //  else {
 //  	Serial.println("black is black");

 //  }


 //runs color/pattern if booleans are true or button is pressed a certain amount of times

  

  if (isRainbow == true || buttonPushCounter == 1) {
	  isRed = false;
	  isGreen = false;
	  isBlue = false;
	  isWhite = false;
	  isAmber = false;
	  isCyan = false;
	  isPurple = false;
	  isPink = false;


	  crossFade(red);
	  crossFade(green);
	  crossFade(blue);
	  crossFade(yellow);

  Serial.println("color set to rainbow");
  }
 
  else if (isRed == true || buttonPushCounter == 2) {
  	
	  isRainbow = false;
	  isGreen = false;
	  isBlue = false;
	  isWhite = false;
	  isAmber = false;
  	  isCyan = false;
      isPurple = false;
      isPink = false;
      color("red");
  // Serial.println("color set to red");
  }

  else if (isGreen == true || buttonPushCounter == 3) {
  	
	  isRed = false;
	  isRainbow = false;
	  isBlue = false;
	  isWhite = false;
	  isAmber = false;
	  isCyan = false;
	  isPurple = false;
	  isPink = false;
	  color("green");
  // Serial.println("color set to green");

  }

  else if (isBlue == true || buttonPushCounter == 4) {
  	
	  isRed = false;
	  isGreen = false;
	  isRainbow = false;
	  isWhite = false;
	  isAmber = false;
	  isCyan = false;
	  isPurple = false;
	  isPink = false;
	  color ("blue");
  // Serial.println("color set to blue");

  }

  else if (isWhite == true || buttonPushCounter == 5) {
  	
	  isRed = false;
	  isGreen = false;
	  isRainbow = false;
	  isBlue = false;
	  isAmber = false;
	  isCyan = false;
	  isPurple = false;
	  isPink = false;
	  color ("white");

  }

  else if (isAmber == true || buttonPushCounter == 6) {
  	
	  isRed = false;
	  isGreen = false;
	  isBlue = false;
	  isRainbow = false;
	  isWhite = false;
	  isCyan = false;
	  isPurple = false;
	  isPink = false;
	  color ("amber");

  }

  else if (isCyan == true || buttonPushCounter == 7) {
  	
	  isRed = false;
	  isGreen = false;
	  isRainbow = false;
	  isBlue = false;
	  isWhite = false;
	  isAmber = false;
	  isPurple = false;
	  isPink = false;
	  color ("cyan");

  }

    else if (isPurple == true || buttonPushCounter == 8) {
  	
	  isRed = false;
	  isGreen = false;
	  isRainbow = false;
	  isBlue = false;
	  isWhite = false;
	  isAmber = false;
	  isCyan = false;
	  isPink = false;
	  color ("purple");
  }

  else if (isPink == true || buttonPushCounter == 9) {
  	

	  isRed = false;
	  isGreen = false;
	  isRainbow = false;
	  isBlue = false;
	  isWhite = false;
	  isAmber = false;
	  isCyan = false;
	  isPurple = false;
	  color ("pink");

  }


//display black if none of the booleans are true

  else {
  	color("black");
  	Serial.println("color is set to black");
  	if (isRainbow){
  		Serial.println("rainbow is true");
  	}

  	else {
  		Serial.println("rainbow is false");
  	}
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

	else if (color == "amber") {
		ledcWrite (redPWMChannel, 255);
		ledcWrite (greenPWMChannel, 128);
		ledcWrite (bluePWMChannel, 0);
		ledcWrite (whitePWMChannel, 70);

	}

	else if (color == "cyan") {
		ledcWrite (redPWMChannel, 0);
		ledcWrite (greenPWMChannel, 255);
		ledcWrite (bluePWMChannel, 255);
		ledcWrite (whitePWMChannel, 25);

	}

	else if (color == "purple") {
		ledcWrite (redPWMChannel, 128);
		ledcWrite (greenPWMChannel, 8);
		ledcWrite (bluePWMChannel, 255);
		ledcWrite (whitePWMChannel, 25);

	}

	else if (color == "pink") {
		ledcWrite (redPWMChannel, 255);
		ledcWrite (greenPWMChannel, 0);
		ledcWrite (bluePWMChannel, 128);
		ledcWrite (whitePWMChannel, 75);

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
