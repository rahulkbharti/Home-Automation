/*
  AUTOMATIC HOME SYYSTEM [ SMART HOME ]
========================================
*/
#include <SoftwareSerial.h>
SoftwareSerial mySerial(0,1);

bool ATM = false;
bool TMR = false;
int analogPin[] = {A0,A1,A2,A3,A4,A5,A6,A7};
bool Switch[] = {false,false,false,false,false,false,false,false,false,false};
void setup(){
    mySerial.begin(9600);  
    mySerial.println("Start");
    for(int i = 4;i < 14 ;i++){
        pinMode(i,OUTPUT);
    }
}
int wt = 0;
void loop(){
   if(mySerial.available() == 1){
       String data = mySerial.readString();
       //MN#PP#S #PP[04-13]
       //AT#N/F
       //AT#i#t#a#dd#vvv#t a[0-7] dd[04-13] vvv[100*]
       //TM#N/F
       //TM#i#t#00:00#00:00#pp pp[04-13]
       //UP#AT/TM/MN#NN
       //MN#pp#t pp[04-13] t[0-1]
       String cmd = data.substring(0,2);
       if(cmd == "MN"){
         int pin = data.substring(3,5).toInt();
         mySerial.println(pin);
         int mode = data.substring(6,7).toInt();
         if(mode == 1){
            digitalWrite(pin,HIGH);
            Switch[pin-4] = true;
            String h = "#";
            Sync("UP#MN"+h+pin);
            
         }
         else{
           digitalWrite(pin,LOW);
            Switch[pin-4] = false;
            String h = "#";
            Sync("UP#MN"+h+pin);
         }
       }
       else if(cmd == "AT"){
          String v = data.substring(3,4);
           //mySerial.println(v);
          if(v == "N"){
            ATM = true;
            mySerial.println("ATM#ON");
          }
          else if(v == "F"){
            ATM = false;
            mySerial.println("ATM#OFF");
          }
          else{
             ATM_setup(data);
          }
       }
       else if(cmd == "TM"){
           String v = data.substring(3,4);
            //mySerial.println(v);
          if(v == "N"){
             TMR = true;
             mySerial.println("TMR#ON");
          }
          else if(v == "F"){
             TMR = false;
             mySerial.println("TMR#OFF");
          }
          else{
             TMR_setup(data);
          }
       }
       else if(cmd == "UP"){
           Sync(data);
        }
        else if(cmd == "TS"){
            setTime(data);
        }
    }
    else{
        wt++;
        if(wt ==1000){
            wt = 0;
            Time();
        }
    }
    delay(1);
}
// TIME
int h = 0;
int m = 0;
int s = 0; 
int Time(){
    s++;
    if(s > 59){
       s = 0;
       m++;
       if(m > 59){
          m = 0;
          h++;
          if(h > 23){
               h = 0;
            }
        }
    }
    if(TMR == true && s <= 3 ){
        TMR_operation();
     }
     if(ATM == true){
         ATM_operation();
     }
    String c = ":";
    String sp= "#";
    String tm = "TIME"+sp+h+c+m+c+s;
    mySerial.println(tm);
}
//SETUP TIME
void setTime(String data){
    //TS#00:00:00;
    //01234567890
    h = data.substring(3,5).toInt();
    m = data.substring(6,8).toInt();
    s = data.substring(9,11).toInt();
    mySerial.println("TSU");
}

// Extara funtction
bool Bool(String x){
    if(x == "t"){
       return true; 
    }
    else{
       return false;
    }
}

//AUTOMATIC MODE
bool ATM_status[] = {true,false,false,false};
int ATM_iPin[] = {7,6,4,5};
int ATM_oPin[] = {13,4,5,6};
int ATM_oValue[] = {100,000,000,100};
bool ATM_mode[] = {true,true,true,true};

void ATM_setup(String data){ 
    // AT#i#t#0#00#000#t
    // 01234567890123456
    int i = data.substring(3,4).toInt();
    
       ATM_status[i] = Bool(data.substring(5,6));
       ATM_iPin[i] = data.substring(7,8).toInt();
       ATM_oPin[i] = data.substring(9,11).toInt();
       ATM_oValue[i] = data.substring(12,15).toInt();
       ATM_mode[i] = Bool(data.substring(16,17));
       
       String sp = "#";
       String s = "AT#Update"+sp+i;
       
       mySerial.println(s);
}
void ATM_operation(){
   for(int i = 0;i < 4 ;i++){
        if(ATM_status[i] == true){
            if(analogRead(analogPin[ATM_iPin[i]]) >= ATM_oValue[i]){
                if(ATM_mode[i] == true){
                    digitalWrite(ATM_oPin[i],HIGH);
                    Switch[ATM_oPin[i]-4] = true;
                    String h = "#";
                    Sync("UP#AT"+h+i);
                }
                else{
                    digitalWrite(ATM_oPin[i],LOW);
                    Switch[ATM_oPin[i]-4] = false;
                    String h = "#";
                    Sync("UP#AT"+h+i);
                }
            }
            else{
               if(ATM_mode[i] == true){
                    digitalWrite(ATM_oPin[i],LOW);
                    Switch[ATM_oPin[i]-4] = false;
                    String h = "#";
                    Sync("UP#AT"+h+i);
                }
                else{
                    digitalWrite(ATM_oPin[i],HIGH);
                    Switch[ATM_oPin[i]-4] = true;
                    String h = "#";
                    Sync("UP#AT"+h+i);
                } 
            }
        }
    }
}
// TIMER MODE
int TMR_status[] = {true,false,false,false};
String TMR_start[] = {"00:01","00:00","00:00","00:00","00:00"};
String TMR_stop[] = {"00:02","00:00","00:00","00:00","00:00"};
int TMR_oPin[] = {4,5,6,13};
    
void TMR_setup(String data){
    //TM#i#t#00:00#00:00#pp
    //012345678901234567890
    int i = data.substring(3,4).toInt();
    TMR_status[i] = Bool(data.substring(5,6));
    TMR_start[i] = data.substring(7,12);
    TMR_stop[i] = data.substring(13,18);
    TMR_oPin[i] = data.substring(19,21).toInt();
    
    String sp = "#";
    String s = "TM#Update"+sp+i;
    mySerial.println(s);
}
void TMR_operation(){
    for(int i = 0;i < 4;i++){
      if(TMR_status[i] == true){
        int sh = TMR_start[i].substring(0,2).toInt();
        int sm = TMR_start[i].substring(3,5).toInt();
        int rh = TMR_stop[i].substring(0,2).toInt();
        int rm = TMR_stop[i].substring(3,5).toInt();
        
        if(sh == h && sm == m){
           digitalWrite(TMR_oPin[i],HIGH);
           Switch[TMR_oPin[i]-4] = true;
           String h = "#";
           Sync("UP#TM"+h+i);
        }
        if(rh == h && rm == m){
           digitalWrite(TMR_oPin[i],LOW);
           Switch[ATM_oPin[i]-4] = false;
           String h = "#";
           Sync("UP#TM"+h+i);
        }
      }
    }
}
// BOOL TO STRING
String Str(bool v){
    if(v == false){
        return "F";
    }
    else{
        return "T";
    }
}
//SYNC
void Sync(String data){
    //UP#MN/AT/TM#NN NN[0-4]
    String h = "#";
    String cmd = data.substring(3,5);
    int n = data.substring(6,8).toInt();
    if(cmd == "MN"){
        String dt = "MNU"+h+n+h+Str(Switch[n-4]);
        mySerial.println(dt);
    }
    else if(cmd == "AT"){
       //String dt = data+h+Str(ATM_status[n]);
       String data = "ATU"+h+n+h+Str(ATM_status[n])+h+ATM_iPin[n]+h+ATM_oPin[n]+h+Str(ATM_mode[n]);
       mySerial.println(data);
    }
    else if(cmd == "TM"){
       //String dt = data+h+Str(TMR_status[n]);
       String data = "TMU"+h+n+h+Str(TMR_status[n])+h+TMR_start[n]+h+TMR_stop[n]+h+TMR_oPin[n];
       mySerial.println(data);
    }
}

