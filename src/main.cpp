#include <Arduino.h>

//definitons (push buttons, indicator lights, solenoids and delay in ms)
#define DPS_Plus PD2 //uno 2
#define DPS_Minus PD3 //uno 3
#define DPS_1 PD4 //uno 4
#define DPS_2 PD5 //uno 5
#define DPS_3 PD6 //uno 6
#define Solenoid_A  PB2 //uno 10
#define Solenoid_B  PB3 //uno 11
#define Start_Delay 500
#define Change_Delay 100 

//definitions for setting the values for turning on or off the solenoids
#define ONSolenoid_A (1 << Solenoid_A)
#define OFFSolenoid_A (~(1 << Solenoid_A))
#define ONSolenoid_B (1 << Solenoid_B)
#define OFFSolenoid_B (~(1 << Solenoid_B))


//variables 
int gear_state; //current gear state/postion
uint64_t press_time, current_time, start_time, count; //variables used in Start_State function, returns number of milliseconds from millis() and counting.
//set each gear state to an enum type of the integer gear_state
enum 
{
  gear_state1 = 1,
  gear_state2 = 2,
  gear_state3 = 3,
};

//function (500ms delay after power on reset)
void Start_State()
{
 while (count <= Start_Delay)
 {
    current_time = millis();

    if (current_time > start_time)
    {
      count += current_time - start_time;
      start_time = current_time;
    }
  }
}

//function for turning indicator lights on
void DPS_LightON(uint16_t value)
{
  PORTD |= value;
}

//function for turning indicator lights off
void DPS_LightOFF(uint16_t value)
{
  PORTD &= ~value;
}

//function to select gear state 1
void Select_Gear1()
{
  PORTB |= ONSolenoid_A; //solenoid A is switched on separately (only one solenoid is driven at a point), see requirement two of the final assignment document
  delay(Change_Delay); //delay of 100ms after ONLY one solenoid is driven, see requirement one of the final assignment document
  PORTB |= ONSolenoid_B; //solenoid B is switched on separately (only one solenoid is driven at a point), see requirement two of the final assignment document
  delay(Change_Delay); //delay of 100ms after ONLY one solenoid is driven, see requirement one of the final assignment document
  DPS_LightON(_BV(DPS_1)); //DPS1 indicator light is turned on
  DPS_LightOFF(((_BV(DPS_2)) | (_BV(DPS_3)))); //DPS2 and DPS3 indicator lights are turned off
}

//function to select gear state 2
void Select_Gear2()
{
  PORTB &= OFFSolenoid_B; //solenoid B is switched off separately (only one solenoid is driven at a point), see requirement two of the final assignment document
  delay(Change_Delay); //delay of 100ms after ONLY one solenoid is driven, see requirement one of the final assignment document
  PORTB |= ONSolenoid_A; //solenoid A is switched on separately (only one solenoid is driven at a point), see requirement two of the final assignment document
  delay(Change_Delay); //delay of 100ms after ONLY one solenoid is driven, see requirement one of the final assignment document
  DPS_LightOFF(((_BV(DPS_1)) | (_BV(DPS_3)))); //DPS1 and DPS3 indicator lights are turned off
  DPS_LightON(_BV(DPS_2)); //DPS2 indicator light is turned on
}

//function to select gear state 3
void Select_Gear3()
{
  PORTB &= OFFSolenoid_A; //solenoid A is switched off separately (only one solenoid is driven at a point), see requirement two of the final assignment document
  delay(Change_Delay); //delay of 100ms after ONLY one solenoid is driven, see requirement one of the final assignment document
  PORTB |= ONSolenoid_B; //solenoid B is switched on separately (only one solenoid is driven at a point), see requirement two of the final assignment document
  delay(Change_Delay); //delay of 100ms after ONLY one solenoid is driven, see requirement one of the final assignment document
  DPS_LightOFF(((_BV(DPS_1)) | (_BV(DPS_2)))); //DPS1 and DPS2 indicator lights are turned off
  DPS_LightON(_BV(DPS_3)); //DPS3 indicator light is turned on
}

void setup() 
{
  //return number of milliseconds since the program started to run and set count to 0
  start_time = millis();
  count = 0;

  //call function for 500ms delay after power on reset (POR)
  Start_State();

  //pins set to output (indicator lights and solenoids)
  DDRD |= ((_BV(DPS_1)) | (_BV(DPS_2)) | (_BV(DPS_3)));
  DDRB |= ((_BV(Solenoid_A)) | (_BV(Solenoid_B)));

  //turn on pull-up resistors for the DPS+ and DPS- push buttons
  PORTD |= ((_BV(DPS_Plus)) | (_BV(DPS_Minus)));

  //select default gear state (gear 1) after power on reset (POR)
  Select_Gear1();
  gear_state = gear_state1;
  current_time = millis();
}
//end setup

void loop()
{
  //switch to and from different gear states
  switch (gear_state)
  {
    //gear 1
    case gear_state1:
      Select_Gear1(); //drive solenoids and indicator lights (on/off) for gear 1 according to the document
      if ((PIND & (1 << DPS_Plus))) //check if DPS+ is pressed
      {
        press_time = millis();
        if (press_time - current_time >= Change_Delay) //check that 100ms delay is given between last state and push button press
        {
          gear_state = gear_state2; //move to next state, gear 2
          current_time = millis();
        }
      }
      break;
  
    //gear 2
    case gear_state2:
      Select_Gear2(); //drive solenoids and indicator lights (on/off) for gear 2 according to the document
      if ((PIND & (1 << DPS_Plus))) //check if DPS+ is pressed
      {
        press_time = millis();
        if (press_time - current_time >= Change_Delay) //check that 100ms delay is given between last state and push button press
        {
          gear_state = gear_state3; //move to next state, gear 3
          current_time = millis();
        }
      }
      else if ((PIND & (1 << DPS_Minus))) //check if DPS- is pressed
      {
        press_time = millis();
        if (press_time - current_time >= Change_Delay) //check that 100ms delay is given between last state and push button press
        {
          gear_state = gear_state1; //move to previous state, gear 1
          current_time = millis();
        }
      }
      break;

    //gear 3
    case gear_state3:
      Select_Gear3(); //drive solenoids and indicator lights (on/off) for gear 3 according to the document
      if ((PIND & (1 << DPS_Minus))) //check if DPS- is pressed
      {
        press_time = millis();
        if (press_time - current_time >= Change_Delay) //check that 100ms delay is given between last state and push button press
        {
          gear_state = gear_state2; //move to previous state, gear 2
          current_time = millis();
        }
      }
      break;
  }
  //end switch
}
//end loop, end of program