/* inspired by 32x8 NTP clock with weather station: https://github.com/707pheonix/WS2812-WeatherStation
 * v1 - niq_ro changed for 8x16 matrix: https://github.com/tehniq3 
 * v1a - changed colour for hours and minutes
 * v1b - automatic colour changed for each hour , tens of minute and tens of seconds
 * v2 - added DST control (A0), updated time calculation, used brigtness control due to sunrise/sunset, as at https://github.com/tehniq3/NTP_clock_ST7789
 * v2a - changed number as 7-segment style
 */
 
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SolarCalculator.h> //  https://github.com/jpb10/SolarCalculator
#include "FastLED.h"

#define NUM_LEDS 128 // 128
#define DSTpin   A0 //  see https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
#define DATA_PIN 14 // D5 = GPIO14 // Data pin that led data will be written out over

CRGB leds[NUM_LEDS];

//WIFI Credentials
const char* ssid =     "niq_ro";
const char* password = "5coldBeers!";

const long timezoneOffset = 2; // GMT + seconds  (Romania)

//#define DSTpin  12 // GPIO12 = D6, see https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
#define DSTpin  A0 //  see https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", timezoneOffset*3600);

////weather variables
WiFiClient client;

int Matrix[8][16]={
                  {0,15,16,31,32,47,48,63,64,79,80,95, 96,111,112,127},
                  {1,14,17,30,33,46,49,62,65,78,81,94, 97,110,113,126},
                  {2,13,18,29,34,45,50,61,66,77,82,93, 98,109,114,125},
                  {3,12,19,28,35,44,51,60,67,76,83,92, 99,108,115,124},
                  {4,11,20,27,36,43,52,59,68,75,84,91,100,107,116,123},
                  {5,10,21,26,37,42,53,58,69,74,85,90,101,106,117,122},
                  {6, 9,22,25,38,41,54,57,70,73,86,89,102,105,118,121},
                  {7, 8,23,24,39,40,55,56,71,72,87,88,103,104,119,120},
                  };
                 
//Number mapping in 8x3 matrix - all recreted by niq_ro
int N1 [8][3]={
  {0,0,1},
  {0,0,1},
  {0,0,1},
  {0,0,1},
  {0,0,1},
  {0,0,1},
  {0,0,1},
  {0,0,0}};        
int N2 [8][3]={
  {1,1,1},
  {0,0,1},
  {0,0,1},
  {1,1,1},
  {1,0,0},
  {1,0,0},
  {1,1,1},
  {0,0,0}};        
int N3 [8][3]={
  {1,1,1},
  {0,0,1},
  {0,0,1},
  {1,1,1},
  {0,0,1},
  {0,0,1},
  {1,1,1},
  {0,0,0}};        
int N4 [8][3]={
  {1,0,1},
  {1,0,1},
  {1,0,1},
  {1,1,1},
  {0,0,1},
  {0,0,1},
  {0,0,1},
  {0,0,0}};       
int N5 [8][3]={
  {1,1,1},
  {1,0,0},
  {1,0,0},
  {1,1,1},
  {0,0,1},
  {0,0,1},
  {1,1,1},
  {0,0,0}};        
int N6 [8][3]={
  {1,1,1},
  {1,0,0},
  {1,0,0},
  {1,1,1},
  {1,0,1},
  {1,0,1},
  {1,1,1},
  {0,0,0}}; 
int N7 [8][3]={
  {1,1,1},
  {0,0,1},
  {0,0,1},
  {0,0,1},
  {0,0,1},
  {0,0,1},
  {0,0,1},
  {0,0,0}};        
int N8 [8][3]={
  {1,1,1},
  {1,0,1},
  {1,0,1},
  {1,1,1},
  {1,0,1},
  {1,0,1},
  {1,1,1},
  {0,0,0}};       
int N9 [8][3]={
  {1,1,1},
  {1,0,1},
  {1,0,1},
  {1,1,1},
  {0,0,1},
  {0,0,1},
  {1,1,1},
  {0,0,0}};        
int N0 [8][3]={
  {1,1,1},
  {1,0,1},
  {1,0,1},
  {1,0,1},
  {1,0,1},
  {1,0,1},
  {1,1,1},
  {0,0,0}};        

// variable to store time value
int h1=0;
int h2=0;
int m1=0;
int m2=0;
int s1=0;
int s2=0;
int dat=0;
int mon=0;
int yr=0;
// variable to store last time value 
int temph1=11;
int temph2=11;
int tempm1=11;
int tempm2=11;
int temps1=11;
int temps2=11;
int tempdat=11;
int tempmon=11;
int tempyr=11;
//other control variable
int offset =0;             // to control reset of clock digits
float tempTEMP=0.00;       // last vale of temp to prevent unwanted number refresh
int secondreference=0;     // to use second value to control Animations
int minutereference=0;     // to use minute value to control Animations
int hourreference=0;     // to use hour value to control Animations
int stringoffset=9; // offset for controlling text with animation
int displayflag=0;
int screenid=1;

byte contor = 0;
byte pauza =0;

byte her = 64;
byte hge = 64;
byte hbe = 64;
byte mer = 64;
byte mge = 64;
byte mbe = 64;
byte ser = 64;
byte sge = 64;
byte sbe = 64;

byte fer = 16;
byte fge = 16;
byte fbe = 16;

byte intensitate = 5;
byte intensitate1 = 10;  // day
byte intensitate0 = 3;  // night
byte intensitate3 = 8;
byte hcol = 0;
byte scol = 0;

byte mcul = 15;
byte hcul = 15;

int hh, mm, ss;
byte DST,DST0;

int an, zi, luna;
int ora1, ora2, oraactuala;
int r1, hr1, mn1, r2, hr2, mn2; 
//int ora, minut, secunda, rest;
int ora0, minut0;
//byte ziua, luna, an;

// Location - Craiova: 44.317452,23.811336
double latitude = 44.31;
double longitude = 23.81;
double transit, sunrise, sunset;
byte noapte = 0;

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(100);
  
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(intensitate1);
  FastLED.clear();
  displaytext(offset,N0,Matrix, her, hge, hbe);
  offset = 4;
  displaytext(offset,N1,Matrix, her, hge, hbe);
  offset = 8;
  displaytext(offset,N2,Matrix, her, hge, hbe);
  offset = 12;
  displaytext(offset,N3,Matrix, her, hge, hbe);
  
  delay(1000);
  FastLED.setBrightness(intensitate0);
  FastLED.show();

      // TIME setup
  Serial.begin(115200);
  Serial.println();
  Serial.print("Wifi connecting to ");
  Serial.println( ssid );
  WiFi.begin(ssid,password);
  Serial.println();
  Serial.print("Connecting");
  
  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      Serial.print(".");        
  }

  Serial.println();
  Serial.println("Wifi Connected Success!");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP() );

  if (analogRead(DSTpin) < 500)
   DST = 0;
  else
   DST = 1;
  DST0 = DST;
  timeClient.setTimeOffset((timezoneOffset + DST)*3600);
  timeClient.begin();  
  
  FastLED.clear();
}

void loop() {
   if (analogRead(DSTpin) < 500)
   DST = 0;
  else
   DST = 1;

if (DST != DST0)
{
  timeClient.setTimeOffset((timezoneOffset + DST)*3600);
  timeClient.begin();
  timeClient.update();
  DST0 = DST;
  Serial.println("DST changed !");   
}

 timeClient.update();
 hh = timeClient.getHours();
 mm = timeClient.getMinutes();
 ss = timeClient.getSeconds();
    h1=hh/10;  //1st digit of hour
    h2=hh%10;  //2st digit of hour
    m1=mm/10;   //1st digit of minute
    m2=mm%10;   //2st digit of minute
    s1=ss/10;   //1st digit of sec
    s2=ss%10;   //2st digit of sec

an = getYear();
luna = getMonth();
zi = getDate();

 if (h1 == 0) Serial.print("0");
 Serial.print(hh);
 Serial.print(":");
 if (m1 == 0) Serial.print("0");
 Serial.print(mm);
 Serial.print(":");
 if (s1 == 0) Serial.print("0");
 Serial.print(ss);
 Serial.print(" -> ");
 if (zi < 10) Serial.print("0");
 Serial.print(zi);
 Serial.print("/");
 if (luna < 10) Serial.print("0");
 Serial.print(luna);
 Serial.print("/");
 if (an < 10) Serial.print("0");
 Serial.println(an);

//hcul = map(0,10*h1+h2, 1,15);
//mcul = map(0,10*m1+m2, 1,15);

hcol = (10*h1+h2)%6;  // 0...5
her = (hcol+1)/4;
hge = ((hcol+1)%4)/2;
hbe = ((hcol+1)%4)%2;

her = her*255;
hge = hge*255;
hbe = hbe*255;

mer = (m1+1)/4;
mge = ((m1+1)%4)/2;
mbe = ((m1+1)%4)%2;

mer = mer*255;
mge = mge*255;
mbe = mbe*255;

scol = (10*s1+s2)%6;  // 0...5
ser = (scol+1)/4;
sge = ((scol+1)%4)/2;
sbe = ((scol+1)%4)%2;

ser = ser*255;
sge = sge*255;
sbe = sbe*255;

if (an >= 2023)
{
   if (tempm1 != m1 ) 
      { 
      offset=9;
      tempm1=m1;
      tempm2=11;
      updatedigit(offset,m1, mer,mge,mbe);      
      }
   if (tempm2 != m2 )
      { 
      offset=13;
      tempm2=m2;
      updatedigit(offset,m2,mer,mge,mbe);
      temph1 = 11;
      } 
   if (temph2 != h2)
      { 
      offset=4;
      temph2=h2;
      updatedigit(offset,h2, her,hge,hbe);
      temph1=11;
      }
   if (temph1 != h1)
      { 
      offset=0;
      temph1=h1;
      updatedigit(offset,h1, her,hge,hbe);
      }

 if (temps2 != s2)
      {
      leds[58] = CRGB(ser,sge,sbe);
      leds[61] = CRGB(ser,sge,sbe);
      leds[66] = CRGB(ser,sge,sbe);
      leds[69] = CRGB(ser,sge,sbe);      
      temps2 = s2;
      }
      else
      {
      leds[58] = CRGB(0,0,0);
      leds[61] = CRGB(0,0,0);
      leds[66] = CRGB(0,0,0);
      leds[69] = CRGB(0,0,0); 
      }
      
FastLED.show();
}
Soare();
night();
Serial.print("brightness = ");
Serial.println(intensitate);

delay(500);
} // end main loop

void digit(int offset, int digpoint [8][3], int matrix [8][16], byte er, byte ge, byte be )
{ cleardigit(offset,matrix);
  for (int i=0 ; i<8; i++)
  { for (int k=0 ; k<=i; k++)
    { for(int j=0;j<3; j++)
      { if (digpoint[k][j]==0)
        leds[matrix [k][j+offset]] = CRGB(0,0,0);
        else
        leds[matrix [k][j+offset]] = CRGB(er,ge,be);
        }
        //delayMicroseconds(100);
        FastLED.show(); 
    }
    //delayMicroseconds(100);
   }
 }

  void displaytext(int offset, int digpoint [8][3], int matrix [8][16], byte er, byte ge, byte be )
{ cleardigit(offset,matrix);
  for (int i=0 ; i<8; i++)
  { for (int k=0 ; k<=i; k++)
    { for(int j=0;j<3; j++)
      { if (digpoint[k][j]==0)
        leds[matrix [k][j+offset]] = CRGB(0,0,0);
        else
        leds[matrix [k][j+offset]] = CRGB(er,ge,be);
        }
        //delay(10);
        }
    //delay(20);
   }
   FastLED.show(); 
  }

void cleardigit(int offset,int matrix [8][16])
{ for (int k=0 ; k<8; k++)
    { for(int j=0;j<4; j++)
      { leds[matrix [k][j+offset]] = CRGB(0,0,0);
} }}


void updatedigit (int offset, int num, byte er,byte ge,byte be)
{
  switch (num) 
  {
  case 1: 
    digit(offset,N1,Matrix,er,ge,be);
    break;
  case 2:
    digit(offset,N2,Matrix,er,ge,be);
    break;
  case 3:
    digit(offset,N3,Matrix,er,ge,be);
    break;
  case 4:
   digit(offset,N4,Matrix,er,ge,be);
    break;
  case 5:
    digit(offset,N5,Matrix,er,ge,be);
    break;
  case 6:
    digit(offset,N6,Matrix,er,ge,be);
    break;
  case 7:
    digit(offset,N7,Matrix,er,ge,be);
    break;
  case 8:
    digit(offset,N8,Matrix,er,ge,be);
    break;
   case 9:
    digit(offset,N9,Matrix,er,ge,be);
    break;
  case 0:
    digit(offset,N0,Matrix,er,ge,be);
    break;
  default:
    // if nothing else matches, do the default
    // default is optional
    break;
  }
}

void updatedigit1 (int offset, int num, byte er,byte ge,byte be)
{
  switch (num) 
  {
  case 1: 
    digit1(offset,N1,Matrix,er,ge,be);
    break;
  case 2:
    digit1(offset,N2,Matrix,er,ge,be);
    break;
  default:
    // if nothing else matches, do the default
    // default is optional
    break;
  }
}

void digit1(int offset, int digpoint [8][3], int matrix [8][16], byte er, byte ge, byte be )
{ //cleardigit(offset,matrix);
  for (int i=0 ; i<8; i++)
  { for (int k=0 ; k<=i; k++)
    { for(int j=0;j<3; j++)
      { if (digpoint[k][j]==0)
        leds[matrix [k][j+offset]] = CRGB(0,0,0);
        else
        leds[matrix [k][j+offset]] = CRGB(er,ge,be);
        }
       // delayMicroseconds(100);
        FastLED.show(); 
    }
 //   delayMicroseconds(100);
   }
 }

 unsigned int getYear() {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  unsigned int an = ti->tm_year + 1900;
  return an;
}

unsigned int getMonth() {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  unsigned int luna = ti->tm_mon + 1;
  return luna;
}

unsigned int getDate() {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  unsigned int zi = ti->tm_mday;
  return zi;
}

void Soare()
{
   // Calculate the times of sunrise, transit, and sunset, in hours (UTC)
  calcSunriseSunset(an, luna, zi, latitude, longitude, transit, sunrise, sunset);

r1 = int(round((sunrise + timezoneOffset + DST) * 60));
hr1 = (r1 / 60) % 24;
mn1 = r1 % 60;

r2 = int(round((sunset + timezoneOffset + DST) * 60));
hr2 = (r2 / 60) % 24;
mn2 = r2 % 60;

  Serial.print("Sunrise = ");
  Serial.print(sunrise+timezoneOffset+DST);
  Serial.print(" = ");
  Serial.print(hr1);
  Serial.print(":");
  Serial.print(mn1);
  Serial.print(" !  / ");
  Serial.print("Sunset = ");
  Serial.print(sunset+timezoneOffset+DST);
  Serial.print(" = ");
  Serial.print(hr2);
  Serial.print(":");
  Serial.print(mn2);
  Serial.print(" ! ");
}

void night() { 
  ora1 = 100*hr1 + mn1;  // rasarit 
  ora2 = 100*hr2 + mn2;  // apus
  oraactuala = 100*hh + mm;  // ora actuala

  Serial.print(ora1);
  Serial.print(" ? ");
  Serial.print(oraactuala);
  Serial.print(" ? ");
  Serial.print(ora2);  

  if ((oraactuala > ora2) or (oraactuala < ora1))  // night 
  {
    noapte = 1;
    intensitate = intensitate0;
  }
  else
  {
    noapte = 0;
    intensitate = intensitate1;
  }
  if (intensitate3 != intensitate)
  {
    temph1=11;
    temph2=11;
    tempm1=11;
    tempm2=11;
    temps1=11;
    temps2=11;
    intensitate3 = intensitate;;  
  }
    FastLED.setBrightness(intensitate);
    FastLED.show();
  if (noapte == 0) 
  Serial.println(" -> daytime ! ");
  else
  Serial.println(" -> nighttime ! ");
}
