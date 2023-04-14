/* inspired by 32x8 NTP clock with weather station: https://github.com/707pheonix/WS2812-WeatherStation
 * v1 - niq_ro changed for 8x16 matrix: https://github.com/tehniq3 
 * v1a - changed colour for hours and minutes
 * v1b - automatic colour changed for each hour , tens of minute and tens of seconds
 */
 
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#include "FastLED.h"
#include <ESP8266WiFi.h>
#include <time.h>
#define NUM_LEDS 128 // 128+8

// Data pin that led data will be written out over
#define DATA_PIN 5  //5

CRGB leds[NUM_LEDS];

//WIFI Credentials
const char* ssid = "niq_ro";
const char* password = "5coldBeers!";

int timezone = 10800; //19080; //9 * 3600;
int dst = 0; //1800;

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
  {0,1,0},
  {0,1,0},
  {1,1,0},
  {1,1,0},
  {0,1,0},
  {0,1,0},
  {0,1,0},
  {1,1,1}};        
int N2 [8][3]={
  {0,1,0},
  {1,0,1},
  {0,0,1},
  {0,0,1},
  {0,1,0},
  {1,0,0},
  {1,0,0},
  {1,1,1}};        
int N3 [8][3]={
  {0,1,0},
  {1,0,1},
  {0,0,1},
  {0,1,0},
  {0,0,1},
  {0,0,1},
  {1,0,1},
  {0,1,0}};        
int N4 [8][3]={
  {0,1,0},
  {0,1,0},
  {1,0,0},
  {1,0,1},
  {1,1,1},
  {0,0,1},
  {0,0,1},
  {0,0,1}};        
int N5 [8][3]={
  {1,1,1},
  {1,0,0},
  {1,0,0},
  {1,1,0},
  {0,0,1},
  {0,0,1},
  {1,0,1},
  {0,1,0}};        
int N6 [8][3]={
  {0,1,0},
  {1,0,1},
  {1,0,0},
  {1,1,0},
  {1,0,1},
  {1,0,1},
  {1,0,1},
  {0,1,0}};  
int N7 [8][3]={
  {1,1,1},
  {1,0,1},
  {0,0,1},
  {0,0,1},
  {0,1,0},
  {0,1,0},
  {0,1,0},
  {0,1,0}};        
int N8 [8][3]={
  {0,1,0},
  {1,0,1},
  {1,0,1},
  {0,1,0},
  {1,0,1},
  {1,0,1},
  {1,0,1},
  {0,1,0}};        
int N9 [8][3]={
  {0,1,0},
  {1,0,1},
  {1,0,1},
  {0,1,1},
  {0,0,1},
  {0,0,1},
  {1,0,1},
  {0,1,0}};        
int N0 [8][3]={
  {0,1,0},
  {1,0,1},
  {1,0,1},
  {1,0,1},
  {1,0,1},
  {1,0,1},
  {1,0,1},
  {0,1,0}};        

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

byte intensitate = 128;
byte hcol = 0;
byte scol = 0;

byte mcul = 15;
byte hcul = 15;

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(100);
  
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(5);
  FastLED.clear();
  displaytext(offset,N0,Matrix, her, hge, hbe);
  offset = 4;
  displaytext(offset,N1,Matrix, her, hge, hbe);
  offset = 8;
  displaytext(offset,N2,Matrix, her, hge, hbe);
  offset = 12;
  displaytext(offset,N3,Matrix, her, hge, hbe);
  
  delay(1000);
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
  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  Serial.println("\nWaiting for Internet time");

  while(!time(nullptr)){
     Serial.print("*");
     delay(1000);
  }
  Serial.println("\nTime response....OK");   
  FastLED.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);
    //Serial.println(p_tm->tm_hour);
    h1=p_tm->tm_hour/10;  //1st digit of hour
    h2=p_tm->tm_hour%10;  //2st digit of hour
    m1=p_tm->tm_min/10;   //1st digit of minute
    m2=p_tm->tm_min%10;   //2st digit of minute
    s1=p_tm->tm_sec/10;   //1st digit of sec
    s2=p_tm->tm_sec%10;   //2st digit of sec
    dat=p_tm->tm_mday;    //Day of the month
    mon=p_tm->tm_mon + 1; // month of the year
    yr=p_tm->tm_year + 1900; //year
    secondreference= p_tm->tm_sec;                     // to be used as update animation based on second value
    hourreference=p_tm->tm_hour;
    minutereference=p_tm->tm_min;

//hcul = map(0,10*h1+h2, 1,15);
//mcul = map(0,10*m1+m2, 1,15);

hcol = (10*h1+h2)%6;  // 0...5
her = (hcol+1)/4;
hge = ((hcol+1)%4)/2;
hbe = ((hcol+1)%4)%2;

her = her*intensitate;
hge = hge*intensitate;
hbe = hbe*intensitate;

mer = (m1+1)/4;
mge = ((m1+1)%4)/2;
mbe = ((m1+1)%4)%2;

mer = mer*intensitate;
mge = mge*intensitate;
mbe = mbe*intensitate;

scol = (10*s1+s2)%6;  // 0...5
ser = (scol+1)/4;
sge = ((scol+1)%4)/2;
sbe = ((scol+1)%4)%2;

ser = ser*intensitate;
sge = sge*intensitate;
sbe = sbe*intensitate;

if (yr >= 2023)
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
