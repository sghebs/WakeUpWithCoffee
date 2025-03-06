#include <Keypad.h> //Libraries needed
#include <Wire.h>
#include <virtuabotixRTC.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

//Dichiarazione variabili
int caffe_corto = 15000;
int caffe_lungo = 19000; //ok
int tazza = 30000;
int refresh = 10000; //refresh schermo
long TempRisc = 40000; //tempo di riscaldamento macchina
long delaysveglia = 80000; //dopo quanto uscito il caffè far partire la sveglia

const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

//Pin
int buzzer = 13;
int pinrele = 14;
int pinservo = 15;
virtuabotixRTC myRTC(2, 3, 4); //Wiring of the RTC (CLK,DAT,RST)
byte rowPins[numRows] = {12,11,10,9}; //Rows 0 to 3 //if you modify your pins you should modify this too
byte colPins[numCols]= {8,7,6,5}; //Columns 0 to 3

char keymap[numRows][numCols]= //keymap defines the key pressed according to the row and columns just as appears on the keypad
{{'1', '2', '3', 'A'}, 
 {'4', '5', '6', 'B'}, 
 {'7', '8', '9', 'C'},
 {'*', '0', '#', 'D'}};

Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

LiquidCrystal_I2C lcd(0x27,16,2);

int i1,i2,i3,i4;
char c1,c2,c3,c4;
char keypressed,keypressedx;

int A_hour=NULL;
int A_minute=NULL;
int AlarmIsActive=NULL;
unsigned long int t = millis();

int schermo = 1;

Servo myservo;  

int delays[] = {150,300,300,100,300,550,575,450,400,500,300,330,150,300,200,200,150,300,150,350,300,150,150,500,450,400,500,300,330,150,300,200,200,150,300,150,350,300,150,150,500,300,100,150,150,300,300,150,150,300,150,100,220,300,100,150,150,300,300,300,150,300,300,300,100,150,150,300,300,150,150,300,150,100,420,450,420,360,300,300,150,300,300,100,150,150,300,300,150,150,300,150,100,220,300,100,150,150,300,300,300,150,300,300,300,100,150,150,300,300,150,150,300,150,100,420,450,420,360,300,300,150,300,150,300,350,150,350,150,300,150,600,150,300,350,150,150,550,325,600,150,300,350,150,350,150,300,150,600,150,300,300,100,300,550,575};
int Tone1[] = {660,660,660,510,660,770,380,510,380,320,440,480,450,430,380,660,760,860,700,760,660,520,580,480,510,380,320,440,480,450,430,380,660,760,860,700,760,660,520,580,480,500,760,720,680,620,650,380,430,500,430,500,570,500,760,720,680,620,650,1020,1020,1020,380,500,760,720,680,620,650,380,430,500,430,500,570,585,550,500,380,500,500,500,500,760,720,680,620,650,380,430,500,430,500,570,500,760,720,680,620,650,1020,1020,1020,380,500,760,720,680,620,650,380,430,500,430,500,570,585,550,500,380,500,500,500,500,500,500,500,580,660,500,430,380,500,500,500,500,580,660,870,760,500,500,500,500,580,660,500,430,380,660,660,660,510,660,770,380};
int Tone2[] = {100,100,100,100,100,100,100,100,100,100,100,80,100,100,100,80,50,100,80,50,80,80,80,80,100,100,100,100,80,100,100,100,80,50,100,80,50,80,80,80,80,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,150,200,80,80,80,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,150,200,80,80,80,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,100,100,100,60,80,60,80,80,80,80,80,80,60,80,60,80,80,80,80,80,60,80,60,80,80,80,80,80,80,100,100,100,100,100,100,100};

void setup() {
  Serial.begin(9600);
  lcd.init(); //Initialize the LCD
  lcd.setBacklight(HIGH);
  lcd.home();    
  pinMode(pinrele, OUTPUT);                               
}

void loop() {
   
  while(keypressed == NO_KEY){ 
    keypressed = myKeypad.getKey();
    myRTC.updateTime();
    if((millis()-t) >= refresh || t < 100){
      lcd.clear(); 
      setlcd();
      t = millis();
    }
    
    if(myRTC.hours == A_hour && myRTC.minutes == A_minute && myRTC.seconds >= 0 && myRTC.seconds <= 1 && AlarmIsActive == 1){ 
      accensione();
      fai_caffe(caffe_lungo);
      pausa(delaysveglia);
      sveglia();
    } //Controllo se devo attivare la sveglia
    
  }
  menu();
}

void menu(){
  switch(keypressed){
    case '0':
      svegliarapida();
      break;
    case '1':
      fai_caffe(caffe_corto);
      break;
    case '2':
      fai_caffe(caffe_lungo);
      break;
    case '3':
      fai_caffe(tazza);
      break;
    case '4':
      digitalWrite(14, HIGH);
      delay(150);
      digitalWrite(14, LOW);
      keypressed = NO_KEY;
      break;
    /*case '5':  // Per far tornare alla posizione 0 il servo
      myservo.attach(pinservo);
      for (int pos = 37; pos >= 0; pos--) { 
        myservo.write(pos);              
        delay(25);     
      }
      myservo.detach();
      keypressed = NO_KEY;
      break;*/
    /*case '9': // Simulazione sveglia
      accensione();
      fai_caffe(caffe_lungo);
      pausa(delaysveglia);
      sveglia();
      keypressed = NO_KEY;
      break;*/
    case '*':
      setup_data();
      break;
    case '#':
      setup_orario();
      break;
    case 'A':
      allarme_setup();
      break;
    case 'B':
      vedi_allarme();
      break;
    case 'C':
      disattiva_allarme();  
      break;
    case 'D':
      if(schermo){
        lcd.setBacklight(LOW);
        schermo = 0;
      }else{
        lcd.setBacklight(HIGH);
        schermo = 1;
      }
      keypressed = NO_KEY;
      break;
    default:
      myRTC.updateTime();
      keypressed = NO_KEY;
      break;
  }  
}

void svegliarapida(){
  keypressed = NO_KEY;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sveglia rapida:");

  while(keypressed == NO_KEY){
    keypressed = myKeypad.waitForKey();
  }
  switch(keypressed){
    case '5':
      A_hour = 5;
      break;
    case '6':
      A_hour = 6;
      break;
    case '7':
      A_hour = 7;
      break;
    case '8':
      A_hour = 8;
      break;
    case '9':
      A_hour = 9;
      break;
    case '0':
      A_hour = 10;
      break;
    case '1':
      A_hour = 11;
      break;
    case '2':
      A_hour = 12;
      break;
  }
  keypressed = NO_KEY;
  lcd.setCursor(1,1);
  lcd.print("--> ");
  if(A_hour < 10){
    lcd.print("0");
  }
  lcd.print(A_hour);
  lcd.print(":");

  while(keypressed == NO_KEY){
    keypressed = myKeypad.waitForKey();
  }

  switch(keypressed){
    case '1':
      A_minute = 15;
      break;
    case '2':
      A_minute = 30;
      break;
    case '3':
      A_minute = 45;
      break;
    default:
      A_minute = 00;
      break;
  }
  if(A_minute < 10){
    lcd.print("0");
  }
  lcd.print(A_minute);
  pausa(1500);
  lcd.clear(); 
  myRTC.updateTime();
  AlarmIsActive = 1;
  setlcd();
  keypressed = NO_KEY;
}

void pausa(long tempo){
  keypressed = NO_KEY;
  t = millis();
  while((millis()-t) <= tempo && keypressed == NO_KEY){
    keypressed = myKeypad.getKey();
  }
  keypressed = NO_KEY;
}

void accensione(){
  digitalWrite(14, HIGH);
  delay(150);
  digitalWrite(14, LOW);
  lcd.clear();
  lcd.print("|  Heating...  |");
  pausa(TempRisc);
  lcd.clear();
}

void fai_caffe(int tempo){
  keypressed = NO_KEY;
  if(keypressed == NO_KEY){
    lcd.clear();
    lcd.print("|  Erogazione  |");

    myservo.attach(pinservo);
    for (int pos = 0; pos <= 37; pos++) { 
      myservo.write(pos);              
      delay(25);                       
    }

    pausa(tempo);

    for (int pos = 37; pos >= 0; pos--) { 
      myservo.write(pos);              
      delay(25);     
    }
    myservo.detach();

    lcd.clear();
    myRTC.updateTime();
    setlcd();
    
  }

}

void setlcd(){
  lcd.setCursor(0,0);
  lcd.print("  |");
  lcd.setCursor(3,0);
  if(myRTC.dayofmonth < 10){
    lcd.print("0");
  }
  lcd.print(myRTC.dayofmonth);
  lcd.setCursor(5,0);
  lcd.print("/");
  if(myRTC.month < 10){
    lcd.print("0");
  }
  lcd.print(myRTC.month);
  lcd.setCursor(8,0);
  lcd.print("/");
  lcd.print(myRTC.year);
  lcd.print("|  ");
  
  lcd.setCursor(0,1);
  if(AlarmIsActive){
    lcd.print("*   |");
    lcd.setCursor(15,1);
    lcd.print("*");
  } else{
    lcd.print("    |");
  }
  lcd.setCursor(5,1);
  if(myRTC.hours < 10){
    lcd.print("0");
  }
  lcd.print(myRTC.hours);
  lcd.print("//");
  if(myRTC.minutes < 10){
    lcd.print("0");
  }
  lcd.print(myRTC.minutes);
  lcd.print("|");

  if(myRTC.hours == 0 && myRTC.minutes == 0 && myRTC.seconds <= 2 && schermo == 1){
    lcd.setBacklight(LOW);
    schermo = 0;
  }

  if(myRTC.hours == 5 && myRTC.minutes == 55 && myRTC.seconds <= 2 && schermo == 0){
    lcd.setBacklight(HIGH);
    schermo = 1;
  }
}

void vedi_allarme(){
  if(AlarmIsActive == 1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("|Alarm set at: |");
    lcd.setCursor(0,1);
    lcd.print("|-");
    lcd.setCursor(14,1);
    lcd.print("-|");
    lcd.setCursor(5,1);
    if(A_hour < 10){
      lcd.print("0");
    }
    lcd.print(A_hour);
    lcd.print(":");
    if(A_minute < 10){
      lcd.print("0");
    }
    lcd.print(A_minute);
    pausa(20000);
    lcd.clear(); 
    myRTC.updateTime();
    setlcd();
  }
  keypressed = NO_KEY;
}

void disattiva_allarme(){
  if(AlarmIsActive){
    lcd.clear();
    lcd.print("|    Alarm     |");
    lcd.setCursor(0,1);
    lcd.print("| deactivated  |");
    AlarmIsActive = 0;
    pausa(2000);
    lcd.clear(); 
    myRTC.updateTime();
    setlcd();
  }
  keypressed = NO_KEY;
}

void setup_data(){
  lcd.clear();
  lcd.print("[]-  Setup:  -[]");
  pausa(1000);

  lcd.clear();
  lcd.print("| Setup year:  |");
  int ctr = 1;

  while(ctr){
    char keypressed2 = myKeypad.waitForKey();  
    if (keypressed2 != NO_KEY && keypressed2 !='*' && keypressed2 !='#' && keypressed2 !='A' && keypressed2 !='B' && keypressed2 !='C' && keypressed2 !='D' ){
      c1 = keypressed2;
      lcd.setCursor(6, 1);
      lcd.print(c1);
      ctr = 0;
    }
  }

  while(!ctr){
    char keypressed3 = myKeypad.waitForKey();
    if (keypressed3 != NO_KEY && keypressed3 !='*' && keypressed3 !='#' && keypressed3 !='A' && keypressed3 !='B' && keypressed3 !='C' && keypressed3 !='D' ){
      c2 = keypressed3;
      lcd.setCursor(7, 1);
      lcd.print(c2);
      ctr = 1;
    }
  }

  while(ctr){
    char keypressed4 = myKeypad.waitForKey();
    if (keypressed4 != NO_KEY && keypressed4 !='*' && keypressed4 !='#' && keypressed4 !='A' && keypressed4 !='B' && keypressed4 !='C' && keypressed4 !='D' ){
      c3 = keypressed4;
      lcd.setCursor(8, 1);
      lcd.print(c3);
      ctr = 0;
    }
  }

  while(!ctr){
    char keypressed5 = myKeypad.waitForKey();
    if (keypressed5 != NO_KEY && keypressed5 !='*' && keypressed5 !='#' && keypressed5 !='A' && keypressed5 !='B' && keypressed5 !='C' && keypressed5 !='D' ){
      c4 = keypressed5;
      lcd.setCursor(9, 1);
      lcd.print(c4);
      ctr = 1;
    }
  }

  i1=(c1-48)*1000;        //the keys pressed are stored into chars I convert them to int then i did some multiplication to get the code as an int of xxxx
  i2=(c2-48)*100;
  i3=(c3-48)*10;
  i4=c4-48;
  int N_year=i1+i2+i3+i4;
  pausa(500);

  lcd.clear();
  lcd.print("| Setup month: |");  // here all programs are stopped until you enter the four digits then it gets compared to the code above

  while(ctr){
    char keypressed6 = myKeypad.waitForKey();  
    if (keypressed6 != NO_KEY && keypressed6 !='*' && keypressed6 !='#' && keypressed6 !='A' && keypressed6 !='B' && keypressed6 !='C' && keypressed6 !='D' && (keypressed6-48) < 2 ){
      c1 = keypressed6;
      lcd.setCursor(7, 1);
      lcd.print(c1);
      ctr = 0;
    }
  }

  while(!ctr){
    char keypressed7 = myKeypad.waitForKey();
    if (keypressed7 != NO_KEY && keypressed7 !='*' && keypressed7 !='#' && keypressed7 !='A' && keypressed7 !='B' && keypressed7 !='C' && keypressed7 !='D' && ((keypressed7-48) + (c1-48)*10) < 13 ){
      c2 = keypressed7;
      lcd.setCursor(8, 1);
      lcd.print(c2);
      ctr = 1;
    }
  }

  i1=(c1-48)*10;
  i2=c2-48;
  int N_month=i1+i2;
  pausa(500);

  lcd.clear();
  lcd.print("|  Setup Day:  |");  // here all programs are stopped until you enter the four digits then it gets compared to the code above

  while(ctr){
    char keypressed8 = myKeypad.waitForKey();  
    if (keypressed8 != NO_KEY && keypressed8 !='*' && keypressed8 !='#' && keypressed8 !='A' && keypressed8 !='B' && keypressed8 !='C' && keypressed8 !='D' && (keypressed8-48) < 4){
      c1 = keypressed8;
      lcd.setCursor(7, 1);
      lcd.print(c1);
      ctr = 0;
    }
  }

  while(!ctr){
    char keypressed9 = myKeypad.waitForKey();
    if (keypressed9 != NO_KEY && keypressed9 !='*' && keypressed9 !='#' && keypressed9 !='A' && keypressed9 !='B' && keypressed9 !='C' && keypressed9 !='D' && ((keypressed9-48) + (c1-48)*10) < 32){
      c2 = keypressed9;
      lcd.setCursor(8, 1);
      lcd.print(c2);
      ctr = 1;
    }
  }


  i1=(c1-48)*10;
  i2=c2-48;
  int N_day=i1+i2;
  pausa(500);
  lcd.clear();
  
  myRTC.setDS1302Time(22, myRTC.minutes, myRTC.hours, 1, N_day, N_month, N_year);
  lcd.clear(); 
  myRTC.updateTime();
  setlcd();
  keypressed = NO_KEY; //the "*" key is stored in "keypressed" so I remove that value from it otherwise it will get me in the setup again
}

void setup_orario(){
  lcd.clear();
  lcd.print("| Setup hour:  |");  // here all programs are stopped until you enter the four digits then it gets compared to the code above

  int ctr = 1;
  while(ctr){
    char keypressed10 = myKeypad.waitForKey();  
    if (keypressed10 != NO_KEY && keypressed10 !='*' && keypressed10 !='#' && keypressed10 !='A' && keypressed10 !='B' && keypressed10 !='C' && keypressed10 !='D' && (keypressed10-48) < 3){
      c1 = keypressed10;
      lcd.setCursor(7, 1);
      lcd.print(c1);
      ctr = 0;
    }
  }

  while(!ctr){
    char keypressed11 = myKeypad.waitForKey();
    if (keypressed11 != NO_KEY && keypressed11 !='*' && keypressed11 !='#' && keypressed11 !='A' && keypressed11 !='B' && keypressed11 !='C' && keypressed11 !='D' && ((keypressed11-48)+(c1-48)*10) < 24){
      c2 = keypressed11;
      lcd.setCursor(8, 1);
      lcd.print(c2);
      ctr = 1;
    }
  }

  i1=(c1-48)*10;
  i2=c2-48;
  int N_hour=i1+i2;
  delay(500);

  lcd.clear();
  lcd.print("|Setup minutes:|");  // here all programs are stopped until you enter the four digits then it gets compared to the code above

  while(ctr){
    char keypressed12 = myKeypad.waitForKey();  
    if (keypressed12 != NO_KEY && keypressed12 !='*' && keypressed12 !='#' && keypressed12 !='A' && keypressed12 !='B' && keypressed12 !='C' && keypressed12 !='D' && (keypressed12-48) < 6){
      c1 = keypressed12;
      lcd.setCursor(7, 1);
      lcd.print(c1);
      ctr = 0;
    }
  }

  while(!ctr){
    char keypressed13 = myKeypad.waitForKey();
    if (keypressed13 != NO_KEY && keypressed13 !='*' && keypressed13 !='#' && keypressed13 !='A' && keypressed13 !='B' && keypressed13 !='C' && keypressed13 !='D' && ((keypressed13-48)+(c1-48)*10) < 60){
      c2 = keypressed13;
      lcd.setCursor(8, 1);
      lcd.print(c2);
      ctr = 1;
    }
  }

  i1=(c1-48)*10;
  i2=c2-48;
  int N_minutes=i1+i2;
  delay(500);
  lcd.clear();
  myRTC.updateTime();
  setlcd();
  myRTC.setDS1302Time(22, N_minutes, N_hour, 1, myRTC.dayofmonth, myRTC.month, myRTC.year);
  keypressed=NO_KEY;
}

void allarme_setup(){
  lcd.clear();
  lcd.print("|    Alarm     |");
  lcd.setCursor(0,1);
  lcd.print("|    setup     |");
  delay(1000);
  lcd.clear();

  lcd.print("|Set alarm hour|"); // here all programs are stopped until you enter the four digits then it gets compared to the code above
  int ctr = 1;
  while(ctr){
    char keypressed14 = myKeypad.waitForKey();
    if (keypressed14 != NO_KEY && keypressed14 !='*' && keypressed14 !='#' && keypressed14 !='A' && keypressed14 !='B' && keypressed14 !='C' && keypressed14 !='D' && (keypressed14-48) < 3){
      c1 = keypressed14;
      lcd.setCursor(7, 1);
      lcd.print(c1);
      ctr = 0;
    }
  }

  while(!ctr){
    char keypressed15 = myKeypad.waitForKey();
    if (keypressed15 != NO_KEY && keypressed15 !='*' && keypressed15 !='#' && keypressed15 !='A' && keypressed15 !='B' && keypressed15 !='C' && keypressed15 !='D' && ((keypressed15-48)+(c1-48)*10) < 25 ){
      c2 = keypressed15;
      lcd.setCursor(8, 1);
      lcd.print(c2);
      ctr = 1;
    }
  }

  i1=(c1-48)*10;
  i2=c2-48;
  A_hour=i1+i2;

  delay(300);
  lcd.clear();
  lcd.print("Set alarm minutes");  // here all programs are stopped until you enter the four digits then it gets compared to the code above
  
  while(ctr){
    char keypressed16 = myKeypad.waitForKey();  
    if (keypressed16 != NO_KEY && keypressed16 !='*' && keypressed16 !='#' && keypressed16 !='A' && keypressed16 !='B' && keypressed16 !='C' && keypressed16 !='D' && (keypressed16-48) < 6){
      c1 = keypressed16;
      lcd.setCursor(7, 1);
      lcd.print(c1);
      ctr = 0;
    }
  }

  while(!ctr){
    char keypressed17 = myKeypad.waitForKey();
    if (keypressed17 != NO_KEY && keypressed17 !='*' && keypressed17 !='#' && keypressed17 !='A' && keypressed17 !='B' && keypressed17 !='C' && keypressed17 !='D' && ((keypressed17-48)+(c1-48)*10) < 60 ){
      c2 = keypressed17;
      lcd.setCursor(8, 1);
      lcd.print(c2);
      ctr = 1;
    }
  }

  i1=(c1-48)*10;
  i2=c2-48;
  A_minute=i1+i2;
  delay(500);

  AlarmIsActive = 1;
  lcd.clear(); 
  myRTC.updateTime();
  setlcd();
  keypressed = NO_KEY;
}

void sveglia(){
  lcd.clear();
  unsigned long t0 = millis();
  int i = 0;
  lcd.print("   AO svejate!!!");
  while(keypressedx == NO_KEY){ 
    if((millis()-t0) >= delays[i]){
      t0 = millis();
      tone(buzzer, Tone1[i], Tone2[i]);
      (i >= sizeof(delays)/sizeof(delays[0]))?i= 0: i++;
    }
    keypressedx = myKeypad.getKey();
  }
  keypressedx = NO_KEY;
  lcd.clear(); 
  myRTC.updateTime();
  setlcd();
  noTone(buzzer);
}
