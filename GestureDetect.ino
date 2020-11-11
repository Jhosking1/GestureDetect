/*
 Purpose:

  This programme is written for an ESP32 dev board and a BNo055 orientation sensor. 
  This programme is a stepping stone in a series of investigations in gestural inputs.
  The goal of theis particular investigation was to be able to recognise movements of a controller 
  as 'gestures' which could be used as software or hardware inputs. Being able to create
  gestural inputs has the opportunity to change how a wide veriety of users experience computer 
  interfaces.

 Method:
 
  - After some preliminary experiments it was found that the chaning euler angles of the BNO055
    are a good approximation of the movement of the sensor though space when used in a handheld 
    capacity. This feature of human anatomy means that there is no need to try and use processor 
    intensive filters to try and calculate movement from accelerometer data.

  - Every loop the euler angles of the BNo055 sensor are read.
  - The change of angle between the current and previous reading is calculated and used and 
    a stand in for velocity
  - This change in angle is categorised and a direction of travel.
  - These direction of travel results are built up into shapes.
  
 Joshua Hosking 2020
 Enjoy!
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* X, Y & Z are the current angle. 
   X1, Y1 & Z1 are the previous angle. 
   x, y & z are the change in angle. 
   i, j & k are used to determine if x, y & z have passed a threshold value.
   d is the direction of movement, d1 is the previous direction
   all values of g are a single movement in order to build a shape
*/

int x, X, X0, X1, X2, X3, X4, y, Y, Y0, Y1, Y2, Y3, Y4 , z, Z, Z0, Z1, Z2, Z3, Z4, i, j, k, d, d1, g1, g2, g3, g4, S;

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28);

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Orientation Sensor Raw Data Test"); Serial.println("");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  /* Display the current temperature */
  int8_t temp = bno.getTemp();
  Serial.print("Current Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.println("");

  bno.setExtCrystalUse(true);

  Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");
}

void loop(void)
{
  
// getting raw euler angle data from the sensors

  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  X = euler.x();
  Y = euler.y();
  Z = euler.z();

/*  
 *  Comparing euler angle to the previous reading and producing a change value which is a reasonable 
 *  substitute for velocity.
 *  
 *  If the start angle is 0 then moving right (or up) is an increase the 0 upwards but a 
 *  movement left (or down) is a decrease from 359, crossing this 0 point in a movement creates 
 *  a spike and so is ignored
 */


  if (X != X0)
  { 
    if ( X == 359 && X0 == 1)
    {}
    else if ( X == 1 && X0 == 359)
    {}
    else  
    {
      x = X0 - X;    
      X0 = X;
    }

  }

  if (Y != Y0)
  { 
    if ( Y == 359 && Y0 == 1)
    {}
    else if ( Y == 1 && Y0 == 359)
    {}
    else  
    {
      y = Y0 - Y;    
      Y0 = Y;

    }

  }  

  if (Z != Z0)
  { 
    if ( Z == 359 && Z0 == 1)
    {}
    else if ( Z == 1 && Z0 == 359)
    {}
    else  
    {
      z = Z0 - Z;    
      Z0 = Z;

    }

  }    

  /* 
   *  In normal use x & y do not go above 50 but there are spikes in the data in the hundreds
   *  these again are ignored.
   */

  if (x > 50)
  {
    x = -10;
  }
  if (x < -50)
  {
    x = 10;
  }

/*
 *  x and y values between 8 * -8 are considered not moving. Above 8 is positive
 *  -8 is negative. These are used to set i,j & k which show the direction of travel.
 */

  if (x > -8 && x < 8)
  {
    i = 0;
  }
  if (x >= 8)
  {
    i = -1;
  }
  if (x <= -8)
  {
    i = 1;
  }
  if (y > -8 && y < 8)
  {
    j = 0;
  }

  if (y >= 8)
  {
    j = -1;
  }
  if (y <= -8)
  {
    j = 1;
  }

  /*
   * Combining i,j & k values the d variable is set shwoing the direction of travel.
   * These directions are split into increments 8 increasing clockwise where UP is 1.
   */
   
  if (j==0 && i==0)
  {
    d = 0;
    S = S+1;
  }
  if (j==1 && i==0)
  {
    d = 1;
    S = 0;
  }
  if (j==1 && i==1)
  {
    d = 2;
    S = 0;
  }
  if (j==0 && i==1)
  {
    d = 3;
    S = 0;
  }
  if (j==-1 && i==1)
  {
    d = 4;
    S = 0;
  }
  if (j==-1 && i==0)
  {
    d = 5;
    S = 0;
  }
  if (j==-1 && i==-1)
  {
    d = 6;
    S = 0;
  } 
  if (j==0 && i==-1)
  {
    d = 7;
    S = 0;
  }
  if (j==1 && i==-1)
  {
    d = 8;
    S = 0;
  }   

if (d != d1 && d !=0)  
  {
    Serial.print("d = ");
    Serial.println(d);
    d1 = d;
    
    g4 = g3;
    g3 = g2;
    g2 = g1;
    g1 = d;

    X4 = X3;
    X3 = X2;
    X2 = X1;
    X1 = X0;

    Y4 = Y3;
    Y3 = Y2;
    Y2 = Y1;
    Y1 = Y0;

    Z4 = Z3;
    Z3 = Z2;
    Z2 = Z1;
    Z1 = Z0;
  }

if (S > 5)
{
  Serial.println("STOP");

  if (g4 == 3 && g3 == 5 && g2 == 7 && g1 ==1)
  {
    Serial.println("THATS A SQUARE");
    g1 = 0;
    g2 = 0;
    g3 = 0;
    g4 = 0;
  }
  else if (g3 == 3 && g2 == 8 && g1 == 3)
  {
    Serial.println("THATS A Z");
    g1 = 0;
    g2 = 0;
    g3 = 0;
    g4 = 0;
  }
  else if (g1 == 3 && X >= 40 && X <= 80 && Z < 50 && Z > -50)
  {
    Serial.println("RIGHT");
    g1 = 0;
    g2 = 0;
    g3 = 0;
    g4 = 0;
  }
  
  else if (g1 == 3 && X >= 220 && X <= 260 && Z < 50 && Z > -50) 
  {
    Serial.println("RIGHT");
    g1 = 0;
    g2 = 0;
    g3 = 0;
    g4 = 0;
  } 
  //This is to counter a quirk in the sensor where the X data occasionally flips
   
  else if (g1 == 3 && Z < -50)
  {
    Serial.println("TILT LEFT GO RIGHT");
    g1 = 0;
    g2 = 0;
    g3 = 0;
    g4 = 0;
  }
  else if (g1 == 7 && Z > 50)
  {
    Serial.println("TILT RIGHT GO LEFT");
    g1 = 0;
    g2 = 0;
    g3 = 0;
    g4 = 0;
  }   
  else
  {
  g1 = 0;
  g2 = 0;
  g3 = 0;
  g4 = 0;
  }
}


  /*
   * Changes in direction are detected and the direction of travel recorded. 
   * These directions are built up into shapes which can be recognised.
   */


  
  /* Display calibration status for each sensor. */
  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  delay(BNO055_SAMPLERATE_DELAY_MS);
}
