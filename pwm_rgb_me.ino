#include <TinyWireM.h>
#include <Adafruit_PWMServoDriver.h>
#include <TinySoftPwm.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
#define LED_PIN_G  1 // green
#define LED_PIN_B  3 // blue

  static uint32_t StartUs=micros();
  static uint32_t StartMs=millis();
  static uint8_t PwmG=0;
  static uint8_t PwmB=255;
  static int8_t  DirG=+1;
  static int8_t  DirB=-1;

void setup() {
//  Serial.begin(9600);
//  Serial.println("16 channel PWM test!");

  // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
  // some i2c devices dont like this so much so if you're sharing the bus, watch
  // out for this!
  TinySoftPwm_begin(128, 0);

  pwm.begin();
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
    
  // save I2C bitrate
  // uint8_t twbrbackup = TWBR;  // not needed?
  // must be changed after calling Wire.begin() (inside pwm.begin())
  // TWBR = 12; // upgrade to 400KHz!  // not needed?
    
}

void loop() { 
  // Drive each PWM in a 'wave'
  for (uint16_t i=0; i<4096; i += 8) {
    for (uint8_t pwmnum=0; pwmnum < 2; pwmnum++) {
      pwm.setPWM(pwmnum, 0, (i + (4096/16)*pwmnum) % 4096 );


      /***********************************************************/
      /* Call TinySoftPwm_process() with a period of 60 us       */
      /* The PWM frequency = 128 x 60 # 7.7 ms -> F # 130Hz      */
      /* 128 is the first argument passed to TinySoftPwm_begin() */
      /***********************************************************/
      if((micros() - StartUs) >= 60)
      {
        /* We arrived here every 60 microseconds */
        StartUs=micros();
        TinySoftPwm_process(); /* This function shall be called periodically (like here, based on micros(), or in a timer ISR) */
      }
      
      /*************************************************************/
      /* Increment/decrement PWM on LED Pin with a period of 10 ms */
      /*************************************************************/
      if((millis()-StartMs) >= 10)
      {
        /* We arrived here every 10 milliseconds */
        StartMs=millis();
        PwmG+=DirG; /* increment or decrement PWM depending of sign of DirG */
        PwmB+=DirB; /* increment or decrement PWM depending of sign of DirB */
        TinySoftPwm_analogWrite(LED_PIN_G, PwmG); /* Update LED_G for Digispark */
        if(PwmB==255) DirG=+1; /* if PWM reaches the maximum: change direction */
        if(PwmB==0)   DirG=-1; /* if PWM reaches the minimum: change direction */
        TinySoftPwm_analogWrite(LED_PIN_G, PwmB); /* Update LED_B for Digispark */
        if(PwmG==255) DirB=+1; /* if PWM reaches the minimum: change direction */
        if(PwmG==0)   DirB=-1; /* if PWM reaches the maximum: change direction */
      }
      
    }
  }
}
