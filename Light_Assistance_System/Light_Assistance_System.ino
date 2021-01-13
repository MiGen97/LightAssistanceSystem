#include <stdio.h>

//define used pins
#define LEFT_HEADLIGHT "pwm1"
#define RIGHT_HEADLIGHT "pwm3"
#define LEFT_DISTANCE_SENSOR "gpio6"
#define RIGHT_DISTANCE_SENSOR "gpio1"
#define PHOTORESISTOR 0

//define the constant values used
#define PWM_PERIOD_NS 10000000
#define MAX_ANALOG_IN ((float)4095.00))
#define reverseDutyCycle(x) (1-(x/MAX_ANALOG_IN)
#define DIMMING_STEP (dutyCycle/10)

uint32_t dutyCycleDecrementValueLeft=0;
uint32_t dutyCycleDecrementValueRight=0;
uint32_t dutyCycleValueLeft=0;
uint32_t dutyCycleValueRight=0;

uint32_t minBrightness=1024;
uint32_t maxBrightness=3072;

bool IRSensorLeft=false;
bool IRSensorRight=false;

uint32_t dutyCycle=0;
uint32_t obstacleDutyCycle=0;

// the setup function runs once when you press reset or power the board
void setup() {

 //pinMode(LEFT_HEADLIGHT, OUTPUT); 
    //set pin mux2 GPIO on HIGH
    configureGPIO(64,"out","hiz",1);
    //set pin mux1 GPIO on LOW
    configureGPIO(76,"out","hiz",0);
    //set level shifter GPIO to out
    configureGPIO(16,"out","hiz");
    //set 22k pull-up GPIO to in
    configureGPIO(17,"in","pullup");
    //configure PIN3 as pwm
    configurePWM(1,PWM_PERIOD_NS);

 //pinMode(RIGHT_HEADLIGHT, OUTPUT);
    //set pin mux1 GPIO on HIGH
    configureGPIO(66,"out","hiz",1);
    //set level shifter GPIO to out
    configureGPIO(18,"out","hiz");
    //set 22k pull-up GPIO to in
    configureGPIO(19,"in","pullup");
    //configure PIN5 as pwm
    configurePWM(3,PWM_PERIOD_NS);
  
 //pinMode(LEFT_DISTANCE_SENSOR, INPUT);
    //configure pin 4 as INPUT
    configureGPIO(6,"in","hiz");
  
 //pinMode(RIGHT_DISTANCE_SENSOR, INPUT);
    //set pin mux1 GPIO on LOW
    configureGPIO(68,"out","pulldown",0);
    //configure pin 6 as INPUT
    configureGPIO(1,"in","hiz");
  
 //pinMode(PHOTORESISTOR, INPUT);
    configureGPIO(49,"in","pullup");
}

// the loop function runs over and over again forever
void loop() {
  dutyCycle=0;
  obstacleDutyCycle=0;
  IRSensorLeft=readIRSensorValue(LEFT_DISTANCE_SENSOR);
  IRSensorRight=readIRSensorValue(RIGHT_DISTANCE_SENSOR);
  
  //dutyCycle=analogRead(PHOTORESISTOR);
  dutyCycle= calibratePhotoResistor(readAnalogPin(PHOTORESISTOR));

  //calculate the duty cycle as inverse of the value read from the photoresistor
  dutyCycle=(uint32_t)(reverseDutyCycle(dutyCycle)*PWM_PERIOD_NS); // percentage * pwm_period
  obstacleDutyCycle=dutyCycle - DIMMING_STEP;

  //run the Dimming mechanism
  checkAndRunDimming();
  
  //analogWrite(LEFT_HEADLIGHT,dutyCycleValueLeft);
  writeDutyCycle(LEFT_HEADLIGHT,dutyCycleValueLeft);
  
  //analogWrite(RIGHT_HEADLIGHT,dutyCycleValueRight);
  writeDutyCycle(RIGHT_HEADLIGHT,dutyCycleValueRight);

}

void checkAndRunDimming(){
    if (true == IRSensorLeft || true == IRSensorRight ){
      delay(1);
    }
    if (true == IRSensorLeft ){
      dutyCycleDecrementValueLeft+=DIMMING_STEP;
      if( dutyCycleDecrementValueLeft >= obstacleDutyCycle )  dutyCycleDecrementValueLeft = obstacleDutyCycle;
    }else{
      dutyCycleDecrementValueLeft=0;
    }
    if (true == IRSensorRight ){
      dutyCycleDecrementValueRight+=DIMMING_STEP;
      if( dutyCycleDecrementValueRight >= obstacleDutyCycle )  dutyCycleDecrementValueRight = obstacleDutyCycle;
    }else{
      dutyCycleDecrementValueRight=0;
    }
  dutyCycleValueLeft=dutyCycle-dutyCycleDecrementValueLeft;
  dutyCycleValueRight=dutyCycle-dutyCycleDecrementValueRight;
}

void configureGPIO(int gpio,String gpioDirection,String drive){
  configureGPIO(gpio,gpioDirection,drive,0);
  return;
}

void configureGPIO(int gpio,String gpioDirection,String drive,int value){
  String sgpio=String(gpio);
  String stringCommand;
  char charArrayCommand[300];

  //reserve the wished gpio
  memset((void *)charArrayCommand, sizeof(charArrayCommand), 0);   
  stringCommand="echo "+sgpio+" > /sys/class/gpio/export";
  stringCommand.toCharArray(charArrayCommand, sizeof(charArrayCommand), 0);   
  system(charArrayCommand);

  //set the direction of the gpio
  memset((void *)charArrayCommand, sizeof(charArrayCommand), 0);   
  stringCommand="echo "+gpioDirection+" > /sys/class/gpio/gpio"+sgpio+"/direction";
  stringCommand.toCharArray(charArrayCommand, sizeof(charArrayCommand), 0);   
  system(charArrayCommand);

  //set the drive of the gpio
  memset((void *)charArrayCommand, sizeof(charArrayCommand), 0);   
  stringCommand="echo "+drive+" > /sys/class/gpio/gpio"+sgpio+"/drive";
  stringCommand.toCharArray(charArrayCommand, sizeof(charArrayCommand), 0);   
  system(charArrayCommand);

  //set the value of the gpio (usefull only when set to direction output
  memset((void *)charArrayCommand, sizeof(charArrayCommand), 0);   
  stringCommand="echo "+String(value)+" > /sys/class/gpio/gpio"+sgpio+"/value";
  stringCommand.toCharArray(charArrayCommand, sizeof(charArrayCommand), 0);   
  system(charArrayCommand);
  return;
}

void configurePWM(int pwm_pin,int pwm_period){
  String sgpio=String(pwm_pin);
  String stringCommand;
  char charArrayCommand[300];

  //reserve the wished pwm pin
  memset((void *)charArrayCommand, sizeof(charArrayCommand), 0);   
  stringCommand="echo "+sgpio+" > /sys/class/pwm/pwmchip0/export";
  stringCommand.toCharArray(charArrayCommand, sizeof(charArrayCommand), 0);   
  system(charArrayCommand);

  //set the pwm pin to enable
  memset((void *)charArrayCommand, sizeof(charArrayCommand), 0);   
  stringCommand="echo 1 > /sys/class/pwm/pwmchip0/pwm"+sgpio+"/enable";
  stringCommand.toCharArray(charArrayCommand, sizeof(charArrayCommand), 0);   
  system(charArrayCommand);

  //set the pwm period ( this is set for all the pwm outputs )
  memset((void *)charArrayCommand, sizeof(charArrayCommand), 0);   
  stringCommand="echo "+String(pwm_period)+" > /sys/class/pwm/pwmchip0/device/pwm_period";
  stringCommand.toCharArray(charArrayCommand, sizeof(charArrayCommand), 0);   
  system(charArrayCommand);

  return;
}

bool readIRSensorValue(String gpio){
  if(readDigitalPin(gpio)==0){
    delayMicroseconds(395);               // wait for 15 pulses at 38kHz.
    if(readDigitalPin(gpio)==0)           // check if it was a false read
      return true;
  }
  return false;
}

bool readDigitalPin(String gpio){
   FILE *fpipe;
   String command = "cat /sys/class/gpio/"+gpio+"/value";
   
   // This buffer will containg the script response
   char cmd_rsp[4];  
   // buffer to be used with popen   
   char cmd_char[300];   
   // clear message buffer   
   memset((void *)cmd_char, sizeof(cmd_char), 0);   
   // convert the message to char array   
   command.toCharArray(cmd_char, sizeof(cmd_char), 0);   
   if ( (fpipe = (FILE*)popen((char *)cmd_char,"r")) ){  
      while ( fgets( cmd_rsp, sizeof(cmd_rsp), fpipe)) {}     
      pclose(fpipe);    
      return atoi(cmd_rsp);   
   }   
   return -1;
}

int readAnalogPin(int pin){
   FILE *fpipe;
   String command = "cat /sys/bus/iio/devices/iio:device0/in_voltage"+String(pin)+"_raw";
   // This buffer will containg the script response
   char cmd_rsp[12];  
   // buffer to be used with popen   
   char cmd_char[300];   
   // clear message buffer   
   memset((void *)cmd_char, sizeof(cmd_char), 0);   
   // convert the message to char array   
   command.toCharArray(cmd_char, sizeof(cmd_char), 0);   
   if ( (fpipe = (FILE*)popen((char *)cmd_char,"r")) ){  
      while ( fgets( cmd_rsp, sizeof(cmd_rsp), fpipe)) {}     
      pclose(fpipe);        
      return atoi(cmd_rsp);   
   }   
   return -1;
}

void writeDutyCycle(String pin, uint32_t value){
   String command = "echo "+String(value)+" > /sys/class/pwm/pwmchip0/"+pin+"/duty_cycle"; 
   // buffer to be used to send the command  
   char cmd_char[300];   
   // clear message buffer   
   memset((void *)cmd_char, sizeof(cmd_char), 0);   
   // convert the message to char array   
   command.toCharArray(cmd_char, sizeof(cmd_char), 0);   
   system(cmd_char);
   return;
}

uint32_t calibratePhotoResistor(uint32_t analogValue){
   uint32_t brightness=0;

   //find if a new min or max value have been found
   if(minBrightness > analogValue)  minBrightness= analogValue;
   if(maxBrightness < analogValue)  maxBrightness= analogValue;
   //Adjust the brightness level to produce a result between 0 and 4096 (2^12).
   brightness=map(analogValue, minBrightness, maxBrightness, 0, 4095); 
    // in case the sensor value is outside the range seen during calibration
   brightness = constrain(brightness, 0, 4095);
   
   if(brightness < 200)  brightness = 0;
   if(brightness > 3900) brightness = 4095;
   
   return brightness;
}
