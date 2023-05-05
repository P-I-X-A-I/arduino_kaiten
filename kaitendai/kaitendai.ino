#include <FlexiTimer2.h>

//****************************
double unit_degree = 60.0;
//****************************


double rotation_speed = 0.2;

// button
bool isRotating = false;
bool isPushedIn = false;
int pushCounter = 0;

bool pwmLED = false;
double pwmInc = 0.01;
double LED_COUNTER;


// stepping motor
bool flipPulse = false;
double pulseCounter = 0.0;
double stepCounter = 0.0;


// motor driver
int DIR = 2;
int STEP = 3;
int MS3 = 4;
int MS2 = 5;
int MS1 = 6;
int ENABLE = 7;


// 360deg = 200step = 3200pulse
// 60deg = 33.33333step = 533.3333pulse *2
// gear ratio = 120:15 = 8:1
// final 60deg = 266.6666666step = 4266.666666pulse *2



double target_pulse = (3200.0 * 8.0) * (unit_degree / 360.0) *2.0; 
double tPulse_to_radian = PI / target_pulse;
double increment = 0.0;

int debugCounter = 0;


void stepper()
{
    // PWM for RED LED
    LED_COUNTER += pwmInc;
    if( LED_COUNTER > 1.0 )
    {
        pwmLED = !pwmLED;
        LED_COUNTER = 0.0;

        if( pwmLED )
        {
          pwmInc = 0.2;
        }
        else
        {
          pwmInc = 0.005; 
        }
    }
    digitalWrite(13, isRotating*pwmLED);


  if( isRotating )
  {
      pulseCounter += increment;
      stepCounter += increment;

      if( stepCounter > 1.0 )
      {
        stepCounter -= 1.0;
        flipPulse = !flipPulse;

        digitalWrite(STEP, flipPulse);
      }

      if( pulseCounter > target_pulse )
      {
        isRotating = false;
        pulseCounter -= target_pulse;
      }
  }



}



void setup() {

  // for debug
  Serial.begin(9600);

// set driver pins
  pinMode( DIR, OUTPUT );
  pinMode( STEP, OUTPUT );
  pinMode( MS1, OUTPUT );
  pinMode( MS2, OUTPUT );
  pinMode( MS3, OUTPUT );
  pinMode( ENABLE, OUTPUT );

  digitalWrite( DIR, LOW );
  digitalWrite( STEP, LOW );
  digitalWrite( MS1, HIGH ); // 1/16 step mode
  digitalWrite( MS2, HIGH );
  digitalWrite( MS3, HIGH );
  digitalWrite( ENABLE, LOW ); // active at low


  FlexiTimer2::set(1, 1.0/10000, stepper);
  FlexiTimer2::start();
}





void loop() {

// detect button hit
  int a_0 = analogRead( 0 );

  if( a_0 > 500 )
  {
    
    if( isRotating == false )
    {
      isPushedIn = true;
      pushCounter++;
    }


    // blink LED when long push
    if( pushCounter > 100 )
    {
      int tempVal = pushCounter % 30;

      if( tempVal < 15 )
      {
        digitalWrite( 13, HIGH );  
      }
      else
      {
        digitalWrite( 13, LOW );
      }
    }

    
  }
  else
  {
    if( isPushedIn && pushCounter < 100 )// normal push
    {
      Serial.print(pushCounter);
      Serial.println("normal push");

      // change motor direction
      digitalWrite( DIR, LOW );

      // start rotation
      isRotating = true;
    }
    else if( isPushedIn && pushCounter >= 100 ) // long push
    {
      Serial.print(pushCounter);
      Serial.println("long push");

      // change motor direction
      digitalWrite( DIR, HIGH );


      // start rotation
      isRotating = true;
    }
    
    isPushedIn = false;
    pushCounter = 0;
    
  }


  if( isRotating )
  {
    double radian = pulseCounter * tPulse_to_radian;
    double coef = sin( radian);
    coef = sqrt( coef );

    increment = rotation_speed * coef + 0.03;

  }
  else
  {
    increment = 0.0;  
  }

  delay(10);
}
