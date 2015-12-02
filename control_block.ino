#include <SoftwareSerial.h>

/*--define pre-command--*/
//#define precmd 0x81

/*--define shift register port--*/
#define data_U 0
#define load_U 1
#define enable_U 2
#define clock_U 3
#define data_D 4
#define load_D 5
#define enable_D 6
#define clock_D 7
#define cntuart '1'
#define cntuartack '2'

/*--define Uart Port--*/
#define LRX 8
#define LTX 9
#define URX 10
#define UTX 11
#define DRX 12
#define DTX 13

/*--setting SoftwareSerial Port--*/
SoftwareSerial Serial_L(LRX,LTX);
SoftwareSerial Serial_U(URX,UTX);
SoftwareSerial Serial_D(DRX,DTX);

/*--define variables--*/
uint8_t data[100];
int tmp = 0;
int available_U=0;
int available_D=0;
int inbyte;

void setup(){
  /*--Setting shift register pinMode--*/
  pinMode(enable_U,OUTPUT);
  pinMode(load_U,OUTPUT);
  pinMode(clock_U,OUTPUT);
  pinMode(data_U,INPUT);
  pinMode(enable_D,OUTPUT);
  pinMode(load_D,OUTPUT);
  pinMode(clock_D,OUTPUT);
  pinMode(data_D,INPUT);
  /*--Shift register initialize--*/
  digitalWrite(load_D,HIGH);
  digitalWrite(enable_D,HIGH);
  digitalWrite(load_U,HIGH);
  digitalWrite(enable_U,HIGH);
}

/*--read UP shift register data--*/
  byte read_register_U(){
  byte result=0;
  for(int i=7;i>=0;i--){
    result|=(digitalRead(data_U)<<i);   
    digitalWrite(clock_U,LOW);
    delayMicroseconds(5);
    digitalWrite(clock_U,HIGH);
    delayMicroseconds(5);
  }
  return result;
}

void read_registers_U(){
  digitalWrite(load_U,LOW);
  delayMicroseconds(5);
  digitalWrite(load_U,HIGH);
  delayMicroseconds(5);
  digitalWrite(clock_U,HIGH);
  digitalWrite(enable_U,LOW);  
  byte incoming=read_register_U();
  while(incoming!=0&&incoming!=0xff){
   data[tmp]=incoming;
   incoming=read_register_U();
   tmp++;
  }
  digitalWrite(enable_U,HIGH);
}

/*--read Down shift register data--*/
byte read_register_D()
{
  byte result=0;
  for(int i=7;i>=0;i--){
    result|=(digitalRead(data_D)<<i);       
    digitalWrite(clock_D,LOW);
    delayMicroseconds(5);
    digitalWrite(clock_D,HIGH);
    delayMicroseconds(5);
  }
  return result; 
}

void read_registers_D()
{
  digitalWrite(load_D,LOW);
  delayMicroseconds(5);
  digitalWrite(load_D,HIGH);
  delayMicroseconds(5);
  digitalWrite(clock_D,HIGH);
  digitalWrite(enable_D,LOW);
  byte incoming=read_register_D();
 
  while(incoming!=0&&incoming!=0xff){
   data[tmp]=incoming;
   incoming=read_register_D();
   tmp++;
  }
  digitalWrite(enable_D,HIGH);
}

void Uartcheck_U(){
 /*--checking connect condition on UP layer--*/ 
  int i=0;
  while(i<1000){
    if(Serial_U.available()){
      if(Serial_U.read()==cntuartack){
         available_U=1;
         break;
        }
      }
      i++;
    }
  }
  
void readUart_U(){
 /*--read Uart on UP layer--*/   
  if(available_U==1){    
     while(1){
       if(Serial_U.available()){
         inbyte=Serial_U.read();
         if(inbyte!=0&&inbyte!=0xff){
           data[tmp]=inbyte;
           tmp++;
           }
         else{
           data[tmp]=0x00;  
           break;
           }
        }  
      }
    }
  else{
     data[tmp]=0x00;
     }
  }


void Uartcheck_D(){
  /*--checking connect condition on DOWN layer--*/
  int i=0; 
  while(i<1000){
    if(Serial_D.available()){
      if(Serial_D.read()==cntuartack){
         available_D=1;
         break;
        }
      }
      i++;
    }
  }

void readUart_D(){  
  /*--read Uart on DOWN layer--*/
  if(available_D==1){
      while(1){
       if(Serial_D.available()){ 
         inbyte=Serial_D.read();
         if(inbyte!=0&&inbyte!=0xff){
           data[tmp]=inbyte;
           Serial_D.write(data[tmp]);
           tmp++;
         }
         else{
           data[tmp]=0x01;
           tmp++;
           break;
           }
       }
     }
   }
   else{
     data[tmp]=0x01;
     tmp++;
   }
}

void p(byte X) {
   if (X < 16) {
     Serial_L.print("0");
     }
   Serial_L.print(X, HEX);
   }

void loop(){
  Serial_L.begin(9600);
  int i;
  /*--checking connect Serialcondition on Left Side--*/
  while(1){
    if(Serial_L.available()){
       if(Serial_L.read()==cntuart){
         Serial_L.write(cntuartack);  //Ack
         break;
       }
    }    
  }
  //data[tmp]=precmd;
  //tmp=1;
  read_registers_D();
  Serial_D.begin(9600);
  Serial_D.write(cntuart);
  Uartcheck_D();
  readUart_D();
  Serial_U.begin(9600);
  Serial_U.write(cntuart);
  Uartcheck_U();
  read_registers_U();
  readUart_U();
  
  Serial_L.begin(9600);
  for(i=0;i<=tmp;i++){
  //p(data[i]);
  Serial_L.write(data[i]);
  //Serial_L.println(data[i],HEX);
  delay(100);
  }
  
  for(i=0;i<=tmp;i++){
  data[tmp]=0x00;
  } 
  tmp=0;
  available_U=0;
  available_D=0;
  inbyte=0; 
}
