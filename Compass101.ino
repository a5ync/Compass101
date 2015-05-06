/* */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <U8glib.h>

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

// U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);	// Fast I2C / TWI 
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST); // Fast I2C / TWI 
sensors_event_t event;

float getAngle()
{
  float headingDegrees=0;
  //
  /* Get a new sensor event 
  Event is global but most likely we won't need event x,y,z details*/
  mag.getEvent(&event);
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  
  // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
  // Find yours here: http://www.magnetic-declination.com/
  // Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
  // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
  float declinationAngle = 0.22;
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
  heading += 2*PI;

  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
  heading -= 2*PI;

  // Convert radians to degrees for readability.
  return heading * 180/M_PI;
}

char* getDirection(float angle)
{
static char dir[3];
//45 degree/2
if((angle < 22.5) || (angle > 337.5 ))
  strcpy(dir,"S");
  if((angle > 22.5) && (angle < 67.5 ))
  strcpy(dir,"SW");
  if((angle > 67.5) && (angle < 112.5 ))
  strcpy(dir,"W");
  if((angle > 112.5) && (angle < 157.5 ))
  strcpy(dir,"NW");
  if((angle > 157.5) && (angle < 202.5 ))
  strcpy(dir,"N");
  if((angle > 202.5) && (angle < 247.5 ))
  strcpy(dir,"NE");
  if((angle > 247.5) && (angle < 292.5 ))
  strcpy(dir,"E");
  if((angle > 292.5) && (angle < 337.5 ))
  strcpy(dir,"SE");
  return dir;
}

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  
  char x[9],y[9],z[9],heading[12];//,dir[5];
  float angle = getAngle();
  //format&copy data to string buffers
  dtostrf(angle, 6, 2 , heading);//convert angle to string
  dtostrf(event.magnetic.x,6,2,x);
  dtostrf(event.magnetic.y,6,2,y);
  dtostrf(event.magnetic.z,6,2,z);

  u8g.drawStr( 0, 15, heading);
  //convert angle to direction
//  u8g.setFont(u8g_font_10x20);
 // u8g.drawStr( 60, 16, getDirection(angle));
  //u8g.drawRFrame(58,0,25,20,5);  //Serial.println(dir);

// draw gear
  u8g.setFontPosTop();
  u8g.setFont(u8g_font_10x20);
  u8g.drawStr( 80, 40, getDirection(angle));
  //u8g.drawRFrame(58,0,25,20,5);  //Serial.println(dir);

  u8g.setFont(u8g_font_5x7);
  u8g.drawStr( 10, 32, "x:");
  u8g.drawStr( 10, 41, "y:");
  u8g.drawStr( 10, 50, "z:");

  u8g.drawStr( 26, 32, x);
  u8g.drawStr( 26, 41, y);
  u8g.drawStr( 26, 50, z);

  //u8g.drawTriangle(14,9, 45,32, 9,42);
}


void displaySensorDetails(void)
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void) {
  Serial.begin(9600);
   // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
   }
   else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
     u8g.setColorIndex(3);         // max intensity
   }
   else if ( u8g.getMode() == U8G_MODE_BW ) {
     u8g.setColorIndex(1);         // pixel on
   }
   else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
     u8g.setHiColorByRGB(255,255,255);
   }
  
  /* Initialise the sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }
  else Serial.println("starting...");
  /* Display some basic information on this sensor */
  displaySensorDetails();
}


void loop(void) {
  // picture loop
  u8g.firstPage();  
  do {
    draw();
    } while( u8g.nextPage() );

  // rebuild the picture after some delay
  delay(300);
}

