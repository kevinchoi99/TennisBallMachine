

  #include "CytronMotorDriver.h"
  #include "LiquidCrystal.h"

CytronMD motor1(PWM_DIR, 5, 4);  // PWM 1 = Pin 5, DIR 4
CytronMD motor2(PWM_DIR, 6, 7); // PWM 2 = Pin 6, DIR 7
CytronMD motor3(PWM_DIR, 8, 9);
CytronMD motor4(PWM_DIR, 10, 11);

const int buttonPin1 = 22;
const int buttonPin2 = 24;
const int buttonPin3 = 26;
const int buttonPin4 = 28;
const int togglePin1 = 30;
const int CLK = 36;   
const int DT = 34;    // to determine rotation direction
const int SW = 32;   // pushbutton on rotary (not in use)

const int RS = 38;
const int E = 40;
const int D4 = 42;
const int D5 = 44;
const int D6 = 46;
const int D7 = 48;
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);
 
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
int buttonState4 = 0;
int toggleState1 = 0;
int terminalState = 0;

int state1 = LOW;
int state2 = LOW;
int previous1 = LOW;
int previous2 = LOW;

int Rotationposition = 0;
int lastCLKvalue;
int CLKvalue; 
int DTvalue;

long time1 = 0;
long time2 = 0;
long time3 = 0;
long time4 = 0;
long debounce = 200;
long verticaltime = 5000;
long forwardtime = 0;
long backwardtime = 0;
long startpress1 = 0;
long startpress2 = 0;
long startpress3 = 0;
long startpress4 = 0;
long endpress1 = 0;
long endpress2 = 0;
long totaltime = 0;
long a = 0;
long b = 0;
long feedratespeed = 225;
long RPMspeed = 175;

bool button3Released = false;
bool button4Released = false;
boolean CW;

byte button3prevstate = 0;
byte button4prevstate = 0;
byte terminalStateprev1 = 0;
byte terminalStateprev2 = 0;

char terminalRead;

// The setup routine runs once when you press reset.
void setup() {
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);
  pinMode(togglePin1, INPUT_PULLUP);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);

  lcd.begin(16,2);
  lcd.setCursor(0,0); 

  lastCLKvalue = digitalRead(CLK);
  
  motor1.setSpeed(0);
  motor2.setSpeed(0);
  Serial.begin(9600);
}


// The loop routine runs over and over again forever.
void loop() {
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);
  buttonState4 = digitalRead(buttonPin4);
  toggleState1 = digitalRead(togglePin1);
  

  


  //PUSHBUTTON 1 and 2
  //INCREMENT AND DECREMENT SPEEDS
  // When toggle switch is on, motor is on, if toggle switch is off, motor turns off
  // Pressing button 2 increases speed by 25 and button 1 decreases speed by 25
  if (toggleState1 == HIGH) {
    if (feedratespeed > 0) {
      if (buttonState1 == HIGH && buttonState2 == LOW && millis() - time1 > debounce) {
        feedratespeed = feedratespeed - 25;
        time1 = millis();
      }
    }
    if (feedratespeed < 250) {
      if (buttonState2 == HIGH && buttonState1 == LOW && millis() - time2 > debounce) {
        feedratespeed = feedratespeed + 25;
        time2 = millis();
      }
    }
;
    // Setting speed for each button
    motor2.setSpeed(feedratespeed);
    lcd.print(feedratespeed);

  }
  else if (toggleState1 == LOW)
    motor2.setSpeed(0);





  
  //PUSHBUTTON 3 and 4
  // Creating global variable 'totaltime' which determines the 'distance' travelled
  totaltime = forwardtime - backwardtime;

  // Setting forward and backward motion for the motor
  if (buttonState3 == HIGH && buttonState4 == LOW && totaltime < verticaltime)
    motor1.setSpeed(-255);
  else if (buttonState3 == LOW && buttonState4 == HIGH && totaltime >= 0)
    motor1.setSpeed(255);
  else if (buttonState3 == HIGH && buttonState4 == HIGH)
    motor1.setSpeed(0);
  else if (terminalState == '3' && totaltime < verticaltime)
    motor1.setSpeed(-255);
  else if (terminalState == '4' && totaltime >= 0)
    motor1.setSpeed(255);
  else
    motor1.setSpeed(0);

    

  // Creating 'forwardtime' variable that stores how much time has the motor been moving forward
  if (buttonState3 != button3prevstate) {
    if (buttonState3 == HIGH && totaltime < verticaltime) {
      button3Released = false;
      startpress1 = millis();
    }
    else if (buttonState3 == LOW && totaltime < verticaltime) {
      button3Released = true;
      a = millis() - startpress1;
      forwardtime = forwardtime + a;
      startpress1 = 0;

    }
    button3prevstate = buttonState3;

  }

  // Creating 'backwardtime' variable that stores how much time has the motor been moving backward
  if (buttonState4 != button4prevstate) {
    if (buttonState4 == HIGH && totaltime >= 0 ) {
      button4Released = false;
      startpress2 = millis();
    }
    else if (buttonState4 == LOW && totaltime >= 0) {
      button4Released = true;
      b = millis() - startpress2;
      backwardtime = backwardtime + b;
      startpress2 = 0;

    }
    button4prevstate = buttonState4;

  }




  
    
///BLUETOOTH MODULE
  if (Serial.available() > 0) {
    terminalRead = Serial.read();
    terminalState = terminalRead;
    delay(2);
    //Button 1 and 2
    if (toggleState1 == HIGH) {
      if (feedratespeed > 0) {
        if (terminalState == '1'){
          feedratespeed = feedratespeed - 25;
          }
        if (terminalState == 'a'){  
          feedratespeed = feedratespeed - 25;
        }
      }
      if (feedratespeed < 250) {
        if (terminalState == '2'){
          feedratespeed = feedratespeed + 25;
        }
        if (terminalState == 'b'){
          feedratespeed = feedratespeed + 25;
        }
      }
    }

      // Setting speed for each button
      motor2.setSpeed(feedratespeed);
      lcd.setCursor(0,0);
      lcd.print(feedratespeed);

      

    //Button 3 and 4
    // Creating 'forwardtime' variable that stores how much time has the motor been moving forward
    if (terminalState != terminalStateprev1) {
      if (terminalState == '3' && totaltime < verticaltime) {
        button3Released = false;
        startpress3 = millis();
      }
      else if (terminalState == 'c' && totaltime < verticaltime) {
        button3Released = true;
        a = millis() - startpress3;
        forwardtime = forwardtime + a;
        startpress3 = 0;

      }
      terminalStateprev1 = terminalState;

    }

    // Creating 'backwardtime' variable that stores how much time has the motor been moving backward
    if (terminalState != terminalStateprev2) {
      if (terminalState == '4' && totaltime >= 0 ) {
        button4Released = false;
        startpress4 = millis();
      }
      else if (terminalState == 'd' && totaltime >= 0) {
        button4Released = true;
        b = millis() - startpress4;
        backwardtime = backwardtime + b;
        startpress4 = 0;

      }
      terminalStateprev2 = terminalState;
    }

    //
    if (RPMspeed >= 100) {
        if (terminalState == '5'){
          RPMspeed = RPMspeed - 10;
          }
        if (terminalState == 'e'){  
          RPMspeed = RPMspeed - 10;
        }
      }
      if (RPMspeed <= 250) {
        if (terminalState == '6'){
          RPMspeed = RPMspeed + 10;
        }
        if (terminalState == 'f'){
          RPMspeed = RPMspeed + 10;
        }
      }
  }






  // ROTARY ENCODER
  CLKvalue = digitalRead(CLK);
  DTvalue = digitalRead(DT);
  if (CLKvalue != lastCLKvalue){
    if(DTvalue != CLKvalue){
      Rotationposition ++;
      CW = true;
    }
    else{
      CW = false;
      Rotationposition --;
    }
    if(CW){
      if (RPMspeed <= 250) {
        RPMspeed = RPMspeed + 10;
      }
    }
    else{
      if (RPMspeed >= 100) {
        RPMspeed = RPMspeed - 10;
      }
    }
    lastCLKvalue = CLKvalue;
  }
  //motor3.setSpeed(RPMspeed);
  //motor4.setSpeed(RPMspeed);
  
  lcd.setCursor(0,1);
  lcd.print(RPMspeed);
  






  /*
  //TOGGLE SPEEDS
  
    // Creating toggle for button 1 and creating overwrite if button 2 is high (when pressing button 1 when button 2 is high, button 1 takes over and button 2 becomes low)
    if (buttonState1 == HIGH && previous1 == LOW && millis() - time1 > debounce) {
      if (state1 == HIGH)
        state1 = LOW;
      else
        state1 = HIGH;
      if (state1 == HIGH && state2 == HIGH)
        state2 = LOW;

      time1 = millis();
    }

    // Creating toggle for button 2 and creating overwrite if button 1 is high (when pressing button 2 when button 1 is high, button 2 takes over and button 1 becomes low)
    if (buttonState2 == HIGH && previous2 == LOW && millis() - time2 > debounce) {
      if (state2 == HIGH)
        state2 = LOW;
      else
        state2 = HIGH;
      if (state2 == HIGH && state1 == HIGH)
        state1 = LOW;

      time2 = millis();
    } 

    // Setting speed for each button
    if (state1 == HIGH && state2 == LOW)
      motor2.setSpeed(200);
      else if (state1 == LOW && state2 == HIGH)
      motor2.setSpeed(255);
      else if (state1 == LOW && state2 == LOW)
      motor2.setSpeed(0);

    previous1 = buttonState1;
    previous2 = buttonState2;

  */

//Serial.println(totaltime);
//Serial.println(terminalState);
//Serial.println(feedratespeed);
//Serial.println(RPMspeed);
//Serial.println(CLKvalue);
//Serial.println(rotation);
//Serial.println(DTvalue);
  lcd.clear();
}
