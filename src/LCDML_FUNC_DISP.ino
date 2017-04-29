#include <Arduino.h>
#include <wire.h>
#include <HTInfraredSeeker.h>
#include <SpeedTrig.h>
#include <AFMotor.h>
#include <NewPing.h>
#include <Adafruit_TCS34725.h>
AF_DCMotor motor1(2); // create motor #2, 64KHz pwm
AF_DCMotor motor2(1); // create motor #2, 64KHz pwm
AF_DCMotor motor3(3); // create motor #2, 64KHz pwm

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
//____________________________________________________________________________

switch (ball) {
    case 1110000:
      irang = 60;
      ircon = 2;
      ball = 0;
      break;
    case 1100001:
      irang = 30;
      ircon = 2;
      ball = 0;
      break;
    case 1100000:
      irang = 30;
      ircon = 2;
      ball = 0;
      break;
   case 1000110:
      irang = 300;
      ircon = 1;
      ball = 0;
      break;
    case 1000011:
      irang = 330;
      ircon = 1;
      ball = 0;
      break;
    case 1000010:
      irang = 330;
      ircon = 0;
      ball = 0;
      break;
    case 1000001:
      irang = 0;
      ircon = 3;
      ball = 1;
      break;
    case 1000000:
      irang = 0;
      ircon = 3;
      ball = 0;
      break;
    case 0111000:
      irang = 120;
      ircon = 2;
      ball = 0;
      break;
    case 0110000:
      irang = 90;
      ircon = 2;
      ball = 0;
      break;
    case 0100000:
      irang = 60;
      ircon = 2;
      ball = 0;
      break;
    case 0011100:
      irang = 180;
      ircon = 1;
      ball = 0;
      break;
    case 0011000:
      irang = 150;
      ircon = 2;
      ball = 0;
      break;
    case 0010000:
      irang = 120;
      ircon = 2;
      ball = 0;
      break;
    case 0001110:
      irang = 240;
      ircon = 1;
      ball = 0;
      break;
    case 0001100:
      irang = 210;
      ircon = 1;
      ball = 0;
      break;
    case 0001000:
      irang = 180;
      ircon = 1;
      ball = 0;
      break;
    case 0000110:
      irang = 270;
      ircon = 1;
      ball = 0;
      break;
    case 0000100:
      irang = 240;
      ircon = 1;
      ball = 0;
      break;
    case 0000010:
      irang = 300;
      ircon = 1;
      ball = 0;
      break;
    case 0000001:
      irang = 0;
      ircon = 3;
      ball = 1;
      break;
    default:
    ircon = 4; // ir config error
    irang = 370; // ir angle set error
    ball = 0;
     break;
  }




switch (ircon) {
    case 1:
      irang - 10;
      break;
    case 2:
      irang + 10;
      break;
    case 3:
      irang;
      break;
    case 4:
      lcd.print("error");
      break;
  }

  m1 =  255*cos((30-irang)*0.0174533);
  m2 =  255*cos((270-irang)*0.0174533);
  m3 =  255*cos((150-irang)*0.0174533);


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




}


void LCDML_DISP_loop_end(LCDML_FUNC_Attacker){


}
