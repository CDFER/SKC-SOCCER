#include <Arduino.h>
#include <wire.h>
#include <HTInfraredSeeker.h>
#include <SpeedTrig.h>
#include <AFMotor.h>
#include <NewPing.h>
#include <Adafruit_TCS34725.h>
AF_DCMotor motor1(1); // create motor #2, 64KHz pwm
AF_DCMotor motor2(2); // create motor #2, 64KHz pwm
AF_DCMotor motor3(3); // create motor #2, 64KHz pwm
//------------------------------------------------------------------------------
#define IR_INT_PIN 2
#define SOCCER_BALL_TIMEOUT 500 //0.5 second
#define IR_read
volatile char IR_port;    //Create a variable to store the data read from PORT
unsigned long IRTime = -SOCCER_BALL_TIMEOUT;
char prevIR_port = 0b11111111;//start with all IR off
//------------------------------------------------------------------------------
/* ===================================================================== *
 *                                                                       *
 * DISPLAY SYSTEM                                                        *
 *                                                                       *
 * ===================================================================== *
 * every "disp menu function" needs three functions
 * - void LCDML_DISP_setup(func_name)
 * - void LCDML_DISP_loop(func_name)
 * - void LCDML_DISP_loop_end(func_name)
 *
 * EXAMPLE CODE:
    void LCDML_DISP_setup(..menu_func_name..)
    {
      // setup
      // is called only if it i
      s started

      // starts a trigger event for the loop function every 100 millisecounds
      LCDML_DISP_triggerMenu(100);
    }

    void LCDML_DISP_loop(..menu_func_name..)
    {
      // loop
      // is called when it is triggert
      // - with LCDML_DISP_triggerMenu( millisecounds )
      // - with every button status change

      // check if any button is presed (enter, up, down, left, right)
      if(LCDML_BUTTON_checkAny()) {
        LCDML_DISP_funcend();
      }
    }

    void LCDML_DISP_loop_end(..menu_func_name..)
    {
      // loop end
      // this functions is ever called when a DISP function is quit
      // you can here reset some global vars or do nothing
    }
 * ===================================================================== *
 */


// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_information)
// *********************************************************************
{
  // setup function
  lcd.setCursor(0, 0);
  lcd.print(F(" WALL-E  Soccer "));
  lcd.setCursor(0, 1);
  lcd.print(F("    SKC 2016    "));

}

void LCDML_DISP_loop(LCDML_FUNC_information)
{
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function

  if(LCDML_BUTTON_checkAny()) { // check if any button is presed (enter, up, down, left, right)
    // LCDML_DISP_funcend calls the loop_end function
    LCDML_DISP_funcend();
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_information)
{
    lcd.setCursor(0, 0);
  lcd.print(F("Wall-E  Soccer "));
  lcd.setCursor(0, 1);
  lcd.print(F("      V1.0      "));
}


// *********************************************************************
uint8_t g_func_timer_info = 0;  // time counter (global variable)
unsigned long g_timer_1 = 0;    // timer variable (globale variable)
void LCDML_DISP_setup(LCDML_FUNC_timer_info)
// *********************************************************************
{
  // setup function
  lcd.print(F("x sec waiting")); // print some content on first row
  g_func_timer_info = 10;       // reset and set timer
  LCDML_DISP_triggerMenu(100);  // starts a trigger event for the loop function every 100 millisecounds
}

void LCDML_DISP_loop(LCDML_FUNC_timer_info)
{
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function

  // this function is called every 100 millisecounds

  // this timer checks every 1000 millisecounds if it is called
  if((millis() - g_timer_1) >= 1000) {
    g_timer_1 = millis();
    g_func_timer_info--;                // increment the value every secound
    lcd.setCursor(0, 0);                // set cursor pos
    lcd.print(g_func_timer_info);       // print the time counter value
  }

  // reset the initscreen timer
  LCDML_DISP_resetIsTimer();

  // this function can only be ended when quit button is pressed or the time is over
  // check if the function ends normaly
  if (g_func_timer_info <= 0)
  {
    // end function for callback
    LCDML_DISP_funcend();
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_timer_info)
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
}

// *********************************************************************
uint8_t g_button_value = 0; // button value counter (global variable)
void LCDML_DISP_setup(LCDML_FUNC_p2)
// *********************************************************************
{
  // setup function
  // print lcd content
  lcd.setCursor(0, 0);
  lcd.print(F("press left or up"));
  lcd.setCursor(0, 1);
  lcd.print(F("count: 0 of 3"));
  // Reset Button Value
  g_button_value = 0;
}

void LCDML_DISP_loop(LCDML_FUNC_p2)
{
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function

  if (LCDML_BUTTON_checkAny()) // check if any button is pressed (enter, up, down, left, right)
  {
    if (LCDML_BUTTON_checkLeft() || LCDML_BUTTON_checkUp()) // check if button left is pressed
    {
      LCDML_BUTTON_resetLeft(); // reset the left button
      LCDML_BUTTON_resetUp(); // reset the left button
      g_button_value++;

      // update lcd content
      lcd.setCursor(7, 1); // set cursor
      lcd.print(g_button_value); // print change content
    }
  }

  // check if button count is three
  if (g_button_value >= 3) {
    // end function for callback
    LCDML_DISP_funcend();
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_p2)
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
}
//________________________________________________________________________________
void LCDML_DISP_setup(LCDML_FUNC_Attacker){
  pinMode(IR_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IR_INT_PIN),IR_read,FALLING);
    DDRA = 0b00000000;    //All pins in PORTA are inputs  http://archive.monome.org/community/uploads/2012/02/Mega%20Port%20Diagram.png (154kB)
    Serial.begin(9600);

   //set port A to pull up all pins
    pinMode(22, INPUT_PULLUP);
    pinMode(23, INPUT_PULLUP);
    pinMode(24, INPUT_PULLUP);
    pinMode(25, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);
    pinMode(27, INPUT_PULLUP);
    pinMode(28, INPUT_PULLUP);
    pinMode(29, INPUT_PULLUP);




}
void LCDML_DISP_loop(LCDML_FUNC_Attacker){
  int ball; // if the ball is in capture zone 1 = possitive 2 = negitive
  int irang; // the angle the ball is at
  int ircon; // what side the ball is on or if there is a problime
  float m1; // motor 1 speed
  float m2; // motor 2 Speed
  float m3; // motor 3 Speed
  int lgred; // light green side on mat red from rgb
  int lggreen; // light green side on mat green from rgb
  int lgblue; // light green side on mat blue from rgb
  int mgred;  // mid green side on mat red from rgb
  int mggreen; // mid green side on mat green from rgb
  int mgblue;  // mid green side on mat blue from rgb
  int dgred;  // dark green side on mat red from rgb
  int dggreen; // dark green side on mat green from rgb
  int dgblue;  // dark green side on mat blue from rgb
  int bred;  // black side on mat red from rgb
  int bgreen; // black side on mat green from rgb
  int bblue;  // black side on mat red from rgb


//setup values for on the day setup__________________________________________________________
lgred = 1;
lggreen = 1;
lgblue = 1;
mgred = 1;
mggreen = 1;
mgblue = 1;
dgred = 1;
dggreen = 1;
dgblue = 1;
bred = 1;
bgreen = 1;
bblue = 1;
//ir_________________________________________________________________________
//delay(1000);
//Serial.println(IR_port,BIN);
//Serial.println(IRTime);

//checking if we have seen the soccer ball in the last second
if(millis() < (IRTime+SOCCER_BALL_TIMEOUT)){//can see ball now

 digitalWrite(13, HIGH);//LED on

 if(IR_port != prevIR_port){//only print when value has changed
    Serial.println(~IR_port,BIN);
    prevIR_port = IR_port;
    Serial.println(IRTime);
  }

}else{//Hvaen't seen ball for SOCCER_BALL_TIMEOUT
   digitalWrite(13, LOW);//LED off
   prevIR_port = 0b11111111;//all IR sensors are off
}//END if()

  //________________________________________________________________________________
  switch (ircon) {
      case 1:
        irang - 10;
        break;
      case 2:
        irang + 10;
        break;
      case 3:
        irang
        ;
        break;
      case 4:
        lcd.print("error");
        break;
    }

//---------------------------------------------------------------------------
switch (ball) {
    case 0b0001111:
      irang = 60;
      ircon = 2;
      ball = 0;
      break;
    case 0b0011110:
      irang = 30;
      ircon = 2;
      ball = 0;
      break;
    case 0b0011111:
      irang = 30;
      ircon = 2;
      ball = 0;
      break;
   case 0b0111001:
      irang = 300;
      ircon = 1;
      ball = 0;
      break;
    case 0b0111100:
      irang = 330;
      ircon = 1;
      ball = 0;
      break;
    case 0b0111101:
      irang = 330;
      ircon = 0;
      ball = 0;
      break;
    case 0b0111110:
      irang = 0;
      ircon = 3;
      ball = 1;
      break;
    case 0b0111111:
      irang = 0;
      ircon = 3;
      ball = 0;
      break;
    case 0b1000111:
      irang = 120;
      ircon = 2;
      ball = 0;
      break;
    case 0b1001111:
      irang = 90;
      ircon = 2;
      ball = 0;
      break;
    case 0b1011111:
      irang = 60;
      ircon = 2;
      ball = 0;
      break;
    case 0b1100011:
      irang = 180;
      ircon = 1;
      ball = 0;
      break;
    case 0b1100111:
      irang = 150;
      ircon = 2;
      ball = 0;
      break;
    case 0b1101111:
      irang = 120;
      ircon = 2;
      ball = 0;
      break;
    case 0b1110001:
      irang = 240;
      ircon = 1;
      ball = 0;
      break;
    case 0b1110011:
      irang = 210;
      ircon = 1;
      ball = 0;
      break;
    case 0b1110111:
      irang = 180;
      ircon = 1;
      ball = 0;
      break;
    case 0b1111001:
      irang = 270;
      ircon = 1;
      ball = 0;
      break;
    case 0b1111011:
      irang = 240;
      ircon = 1;
      ball = 0;
      break;
    case 0b1111101:

      irang = 300;
      ircon = 1;
      ball = 0;;
      break;
    case 0b1111110:
      irang = 0;
      ircon = 3;
      ball = 1;
      break;
    default:
     ircon = 4; // ir config error
      break;


  }






irang = 0;


        //MOTOR 1 *************************************************************
        if (0 == (149*cos((300-irang)*0.0174533))){//is the motor equal to zero
          m1;//set it to zero
        }
        else
        {
          m1 = 149*cos((300-irang)*0.0174533)+106;//set it to 5v to 12v
        }//end if else

        //MOTOR 2 *************************************************************
        if (0 == (149*cos((180-irang)*0.0174533))){//is the motor equal to zero
          m2;//set it to zero
        }
        else
        {
          m2 = 149*cos((180-irang)*0.0174533)+106;//set it to 5v to 12v
        }//end if else

        //MOTOR 3 *************************************************************
        if (0 == (149*cos((60-irang)*0.0174533))){//is the motor equal to zero
          m3;//set it to zero
        }
        else
        {
          m3 = 149*cos((60-irang)*0.0174533)+106;//set it to 5v to 12v
        }//end if else



  motor1.setSpeed(m1);
  motor2.setSpeed(m2);
  motor3.setSpeed(m3);

  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);

  if(m1 <0){
  motor1.run(BACKWARD);
  m1 = m1 * -1;
  }

  if(m2 <0){
  motor2.run(BACKWARD);
  m2 = m2 * -1;
  }

  if(m3 <0){
  motor3.run(BACKWARD);
  m3 = m3 * -1;
  }



  //IR_port = 0;
   IR_port = PINA;      //Read the PORTA and put the values in the variable
   IRTime = millis();




}


void LCDML_DISP_loop_end(LCDML_FUNC_Attacker){


}
