#include <max6675.h>

MAX6675 sensor(4,5,6);
int fan = 9;
double temp = 0;
int fanButtonPin = 2;
int fanButtonLevel = 0;
double fanPower = 0;
int buttonState;                    // record the current button state
int lastButtonState = LOW;          // record the last button state
long lastDebounceTime = 0;
long debounceDelay = 100;  
int level = 0;


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(fan, OUTPUT);
pinMode(fanButtonPin, INPUT);
pinMode(LED_BUILTIN, OUTPUT);
}

int getButtonLevel(int buttonPin){
   
      int reading = digitalRead(buttonPin);

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

double getFanPower(){
  double power = 0;
  int maxTemp = 120;
  int minTemp = 40;
  delay(1000);
  temp = sensor.readCelsius();

    if (temp > maxTemp){
     temp = minTemp;
    }
    
    if(temp < minTemp){
      power = 0;
    }
    else if (temp > maxTemp){
      power = 255;
    }
    else{
      
      power = 255-((maxTemp - temp)/(maxTemp-minTemp)*255);
      
    }

  return power;
}

void printStatus(){
  if(fanButtonLevel == 4){
       
      Serial.print(" Mode : AUTO ");
      Serial.print("Sensor TEMP = ");
  Serial.print(temp);
  Serial.print(" => ");
  Serial.print("Fan POWER = ");
  Serial.println(fanPower);
  }
  else if(fanButtonLevel == 0){
    Serial.println("Mode : OFF");
    digitalWrite(LED_BUILTIN, LOW);   
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);   
      Serial.print("Mode : MANUAL ");
      Serial.print("Fan Button Level = ");
      Serial.print(fanButtonLevel);
      Serial.print("Fan Power = ");
      Serial.println(fanPower);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

fanButtonLevel = getButtonLevel(fanButtonPin);

  if(fanButtonLevel == 1){
    digitalWrite(LED_BUILTIN, LOW);
  fanPower = 255/3;
  }
  if(fanButtonLevel == 2){
    digitalWrite(LED_BUILTIN, LOW);
  fanPower = 255/3*2;
  }
  if(fanButtonLevel == 3){
    digitalWrite(LED_BUILTIN, LOW);
  fanPower = 255;
  }
  if(fanButtonLevel == 4){
  digitalWrite(LED_BUILTIN, HIGH);
  fanPower = getFanPower();
  }
  if(fanButtonLevel >= 5){
    digitalWrite(LED_BUILTIN, LOW);
    fanPower = 0;
    level = 0;
    fanButtonLevel = 0;
  }

analogWrite(fan,fanPower);
printStatus();
}

