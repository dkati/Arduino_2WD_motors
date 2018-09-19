//PIN layout
#define PWM1_FWD 9
#define PWM2_BWD 8
//#define DEBUG

enum Direction {
  FWD = 1,
  BWD = 2
};
typedef enum Direction Direction_t ;
Direction_t dir;

struct KeyboardKeys {
  enum Keys {
    S     = 115,  //Backward
    W     = 119,  //Forward
    Z     = 122,  //Exit
    PLUS  = 43,   //Increase speed
    MINUS = 45    //Decrease speed
  };
};
KeyboardKeys Keys;

int CUR_SPEED = 100;
char s[50];

const int SPEED_STEP = 50;
const int MAX_SPEED = 250; // just a little bit below 255.
const int MIN_SPEED = CUR_SPEED;
const int STOP = 0 ;

void setup()
{
  pinMode(PWM1_FWD , OUTPUT); //front
  pinMode(PWM2_BWD , OUTPUT); //back

  Serial.begin(9600);
  Serial.println("Initial speed is 100");
  Serial.println("Navigate using W-S-D-A format.");
  Serial.println("Press 'z' to stop.");
  Serial.println(" ");
}

void loop()
{
  while (Serial.available()) {
    int inByte = Serial.read();

#ifdef DEBUG
    Serial.println("______");
    Serial.println("Bytes:");
    Serial.println(inByte);
    Serial.println("______");
#endif

    //Handle the speed
    if ( (inByte == KeyboardKeys::MINUS) || (inByte == KeyboardKeys::PLUS)) {

      if (inByte == KeyboardKeys::PLUS) {
        CUR_SPEED += SPEED_STEP;
        if (CUR_SPEED > MAX_SPEED) {
          sprintf(s, "SAFETY TRIGGER ACTIVATED!Setting speed to %d", MAX_SPEED);
          Serial.println(s);
          CUR_SPEED = MAX_SPEED;
        }
        else {
          sprintf(s, "Velocity increased.Speed is %d/255", CUR_SPEED);
          Serial.println(s);
        }
      } else { // velocity reduced. Treat the situation as if minus is pressed
        CUR_SPEED -= SPEED_STEP;
        if (CUR_SPEED < MIN_SPEED) {
          sprintf(s, "SAFETY TRIGGER ACTIVATED!Setting speed to %d", MIN_SPEED);
          Serial.println(s);
          CUR_SPEED = MIN_SPEED;
        } else {
          sprintf(s, "Velocity reduced.Speed is %d/255", CUR_SPEED);
          Serial.println(s);
        }
      }

      if (dir == FWD) {
        analogWrite(PWM2_BWD, STOP); //remove all the current from the opposite direction,so the pwm1 will be able to work
        analogWrite(PWM1_FWD, CUR_SPEED);
      } else if ( dir == BWD ) {
        analogWrite(PWM1_FWD, STOP); //remove all the current from the opposite direction,so the pwm2 will be able to work
        analogWrite(PWM2_BWD, CUR_SPEED);
      }

    }
    //Handle the forward/backward action
    if ( inByte == KeyboardKeys::W ) {
      Serial.println("-->Forward key detected<--");
      analogWrite(PWM2_BWD, STOP); //remove all the current from the opposite direction,so the pwm1 will be able to work
      analogWrite(PWM1_FWD, CUR_SPEED);
      dir = FWD;
    }

    else if (inByte == KeyboardKeys::S ) {
      Serial.println("-->Backward key detected<--");
      analogWrite(PWM1_FWD, STOP); //remove all the current from the opposite direction,so the pwm2 will be able to work
      analogWrite(PWM2_BWD, CUR_SPEED);
      dir = BWD;
    }
    else if (inByte == KeyboardKeys::Z) {
      Serial.println("Goodbye");
      analogWrite(PWM1_FWD, STOP);
      analogWrite(PWM2_BWD, STOP);
      return; //exit of 'z' is pressed
    }
    else {}
  } //end of while

}
