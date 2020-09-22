#include <max6675.h>
#include <Led7Segment.h>


MAX6675 sensor(6,5,4);
int fanLevel[] = { 0,3,9,10,11,2 };
double temp = 0;
int fanButtonPin = 12;
int fanButtonLevel = 0;
int fanPower = 0;
int buttonState;                    // record the current button state
int lastButtonState = LOW;          // record the last button state
long lastDebounceTime = 0;
long debounceDelay = 100;  
int level = 0;
Led7Segment display(7,8);


uint8_t letterA[] = { 
	SEG_TR | SEG_TL | SEG_T | SEG_BL | SEG_M | SEG_BR
};
uint8_t letterL[] = {
	SEG_TL | SEG_BL | SEG_B,  // L
};
uint8_t off[] = {
	SEG_BR | SEG_B | SEG_BL | SEG_M,   // o
	SEG_TL | SEG_BL | SEG_T | SEG_M,   // F
	SEG_TL | SEG_BL | SEG_T | SEG_M,   // F

};


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(fanButtonPin, INPUT);
pinMode(LED_BUILTIN, OUTPUT);
pinMode(fanLevel[1], OUTPUT);
pinMode(fanLevel[2], OUTPUT);
pinMode(fanLevel[3], OUTPUT);
pinMode(fanLevel[4], OUTPUT);
pinMode(fanLevel[5], OUTPUT);
//pinMode(fanLevel[0], OUTPUT);
display.turnOn();
display.setBrightness(1);
display.clearDisplay(0, 4, false);
}

int getButtonLevel(int buttonPin){
	//int level = 0;
      int reading = digitalRead(buttonPin);
	  Serial.println(reading);
   // check to see if you just pressed the button
   // (i.e. the input went from LOW to HIGH), and you've waited long enough
   // since the last press to ignore any noise:
   // If the switch changed, due to noise or pressing:
   
   if (reading != lastButtonState) {
     lastDebounceTime = millis(); // reset the debouncing timer
     //Serial.print("lastDebounceTime = ");
     //Serial.println(lastDebounceTime);
   }

   if ((millis() - lastDebounceTime) > debounceDelay) {
    //Serial.println("IN");
     // whatever the reading is at, it's been there for longer than the debounce
     // delay, so take it as the actual current state:

     // if the button state has changed:
     if (reading != buttonState) {
       buttonState = reading;

       // only toggle the LED if the new button state is HIGH
       if (buttonState == HIGH) {
         level = level + 1;
       }
     }
   }

   // save the reading. Next time through the loop, it'll be the lastButtonState:
   lastButtonState = reading;
   
  return level;
}

int getFanPower(){
  int power = 0;
  int maxTemp = 90;
  int minTemp = 46;
  delay(500);
  temp = sensor.readCelsius();
    
    if(temp < minTemp){
      return 0;
    }
    else if (temp > maxTemp){
      return 5;
    }
    else{
      
      power = 5 -((maxTemp - temp)/(maxTemp-minTemp)*5);
	 
    }
  return power;
}

void printStatus(){
	 if(fanButtonLevel == 4){
	 Serial.print(" Mode : AUTO ");
	 Serial.print("Sensor TEMP = ");
	 Serial.print(sensor.readCelsius());
	 Serial.print(" => ");
	 Serial.print("Fan POWER = ");
	 Serial.println(fanPower);
	 display.setSegments(letterA,0,1);
	 display.showNumber(temp,1,3, true);

  }
  else if(fanButtonLevel == 0){
	Serial.println("Mode : OFF");
	display.setSegments(off,1,3);
    digitalWrite(LED_BUILTIN, LOW);   
  }
  else {
	  digitalWrite(LED_BUILTIN, LOW);   
      Serial.print("Mode : MANUAL ");
      Serial.print("Fan Button Level = ");
	  display.setSegments(letterL, 0, 1);
	  display.showNumber(fanButtonLevel, 1, 1, false);
      Serial.print(fanButtonLevel);
      Serial.print("Fan Power = ");
      Serial.println(fanPower);

  }
}

void loop() {
  // put your main code here, to run repeatedly:
int oldLevel = fanButtonLevel;
fanButtonLevel = getButtonLevel(fanButtonPin);
Serial.println(fanButtonLevel);
if (oldLevel != fanButtonLevel)
{
	display.clearDisplay();
	for (int i = 1; i < 6; i++)
	{
		digitalWrite(fanLevel[i], LOW);
	}
}
  if(fanButtonLevel == 1){
    digitalWrite(LED_BUILTIN, LOW);
	digitalWrite(fanLevel[1], HIGH);
  }
  if(fanButtonLevel == 2){
    digitalWrite(LED_BUILTIN, LOW);
	digitalWrite(fanLevel[3], HIGH);
  }
  if(fanButtonLevel == 3){
    digitalWrite(LED_BUILTIN, LOW);
	digitalWrite(fanLevel[5], HIGH);
  }
  if(fanButtonLevel == 4){
  digitalWrite(LED_BUILTIN, HIGH);
  int currPower = fanPower;
  fanPower = getFanPower();
  if (currPower != fanPower)
  {
	  digitalWrite(fanLevel[currPower], LOW);
	  digitalWrite(fanLevel[fanPower], HIGH);
  }
  
  }
  if(fanButtonLevel >= 5){
    digitalWrite(LED_BUILTIN, LOW);
    fanPower = 0;
    level = 0;
    fanButtonLevel = 0;
  }
printStatus();
}

