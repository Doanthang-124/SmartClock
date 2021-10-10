//***** Wifi Smart Clock with blynk by  Crazy Indiana*****//
/*************************************************************
*  CRAZY INDIANA Wifi Smart Clock (NodeMcu) with Blynk        *
*  Website www.crazyindiana.in                                *
*  Youtube www.youtube.com/c/CrazyIndiana                     *
*  Download latest Blynk library here:                        *
*  https://github.com/blynkkk/blynk-library/releases/latest   *
**************************************************************/
// Date and time functions using a DS3231 RTC connected via I2C and Wire lib

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "RTClib.h"
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>0
#include "DHT.h"

#define DHTPIN 0    // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);


RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char spellMonth[12][6] = {"Jan","Feb","March","April","May","June","July","Aug","Sep","Oct","Nov","Dec"};

char strTime[12]={ "" };      //Store time
char strBDate[40]={ "" };     //Store date
char strTemp[10]={ "" };      //Store Temperature
char strHum[10]={ "" };       //Store Humidity
char strBMsg[]={ "" };        //Store Blynk Message

int intZ=1;               // Case Loop Flag
bool CTFlag=false ;       // Call getTime()
int intDTime=0;           // Display Time 
bool BMFlag=false ;       // Check Blynk message flag
int intBtn=0;             // Check Button Press 
const int Btnpin = 2;    // Button Key press Pin

char auth[] = "PYtM-yEMWfHBHSdZc1o2dLseIZIVEfQy";  // Authorization Token No. From Blynk
char ssid[] = "Wifi Chua";          // Your Router/Hotspot ID.
char pass[] = "haimuoitrieu";      // Your Router/Hotspot Password




// Turn on debug statements to the serial output
#define DEBUG 0

#if DEBUG
#define PRINT(s, x) { Serial.println(F(s)); Serial.println(x); }
#define PRINTS(x) Serial.println(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW //PAROLA_HW
#define MAX_DEVICES 4

#define CLK_PIN   14
#define DATA_PIN  13
#define CS_PIN    15

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup() 
{       
        pinMode(Btnpin,INPUT_PULLUP); // Button Key press Pin
        
        
        
        Blynk.begin(auth, ssid, pass); 
        Serial.begin(9600);
        dht.begin();
        
        if (! rtc.begin()) 
        {
          Serial.println("Couldn't find RTC");
          Serial.flush();
          abort();
        }
      
        if (rtc.lostPower())
        {
          Serial.println("RTC lost power, let's set the time!");
          // When time needs to be set on a new device, or after a power loss, the
          // following line sets the RTC to the date & time this sketch was compiled
          //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
          // This line sets the RTC with an explicit date & time, for example to set
          // January 21, 2014 at 3am you would call:
          // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
        }
           
      //-----------------------------------
      P.begin();
      P.setIntensity(2); 
      P.displayText("Smart Clock - Doan Van Thang", PA_LEFT, 40,2000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
     
}
 //-----------------------------
    BLYNK_WRITE(V1)
    { 
      BMFlag=true;
      String strMsg = param.asStr(); 
      int intlen=strMsg.length() +1 ;
      strMsg.toCharArray(strBMsg, intlen);
      
    }

//------------------------

//-------------Function for Reading Clock Time---------------------//
void getTime( bool f = true)

{
  intDTime++;
  DateTime now = rtc.now();
   
    char strhour[3]={ "" };
    char strmin[3]={ "" };
    int inthour = now.hour()%12; ////%12;
    int intmin = now.minute();
    Serial.println(inthour);
   
    if (inthour <10) 
    { sprintf(strhour,"0%d",inthour);}
    else 
    { sprintf(strhour,"%d",inthour); }
    
    if (inthour == 0)
    { inthour =12 ; sprintf(strhour,"%d%",inthour);}

        
    if (intmin <10) 
    { sprintf(strmin,"0%d",intmin); }
    else 
    { sprintf(strmin,"%d",intmin); }
    
    sprintf(strTime,"%s%c%s", strhour,(f ? ':' : ' '), strmin);
   
  
    if (intDTime==15) // How long time to Display in sec.
    { CTFlag=false;
      intDTime=0;
      intZ++;
      //P.displayReset();
    }
  
}
//-------------------------------------------------//

//------Function to get Temp & Hum.-----------------// 
void getTempHum()
  {
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
    Serial.println(h);
    Serial.println(t);
    Serial.println(f);
    

     // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) 
    {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
    }
    sprintf(strTemp,"T: %d\xB0",(int)t);
    sprintf(strTemp,"%sc",strTemp);
    sprintf(strHum,"H: %d",(int)h);
    strcat(strHum,"%");
  }
void loop() {
    Blynk.run();
    static uint32_t lastTime = 0; // millis() memory
    static bool flasher = false;  // seconds passing flasher
    
     if(P.displayAnimate())
     { 
           if (BMFlag == true)
           { 
              P.displayText(strBMsg, PA_LEFT, 35,2000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
              BMFlag = false;
           }
           else
           {
           switch (intZ)
               {
                  case 1:
                  
                  P.displayText(strTime, PA_CENTER, 0,1000, PA_PRINT, PA_NO_EFFECT);
                  //intZ++;
                  CTFlag=true;
                  P.displayReset();
                  break;
                  
                  case 2:
                  getTempHum();
                  P.displayText(strTemp, PA_CENTER,100,10000, PA_SCROLL_UP, PA_SCROLL_UP);
                  intZ++;
                  break;
                  
                  case 3:
                  getTempHum();
                  P.displayText(strHum, PA_CENTER,100,10000, PA_SCROLL_UP, PA_SCROLL_UP);
                  intZ++;
                  P.displayReset();
                  break;
                  
                  case 4:
                  //Display date like --- Monday 5 Jan 2020//
                  DateTime now = rtc.now();
                  sprintf(strBDate,"%s %d %s %d",daysOfTheWeek[now.dayOfTheWeek()],now.day(),spellMonth[now.month()-1],now.year());
                  P.displayText(strBDate, PA_LEFT, 35,2000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                  intZ=1;
                  P.displayReset();
                  break;
    
               }
           }
           
           P.displayClear();
           P.displayReset();
     }
            intBtn=digitalRead(Btnpin);// Send notification to SmartPhone App Blynk 
            if (intBtn==LOW)
            {
              Serial.println("Button pressed");
              Blynk.notify("Warning PANIC Button Pressed");
            }
        // Finally, adjust the time string if we have to
          if (millis() - lastTime >= 1000)
          {  
            lastTime = millis();
            //getsec(szsecond);
      
            if (CTFlag == true)
            {
            getTime(flasher);
            flasher = !flasher;
            P.displayReset();
            }        
          }

}
