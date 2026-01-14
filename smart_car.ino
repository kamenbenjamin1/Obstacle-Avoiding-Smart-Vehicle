#include <Servo.h>

Servo myservo;  // create Servo object to control a servo
// twelve Servo objects can be created on most boards

int pos = 90;    // variable to store the servo position

int s_delay = 150; //  100-millisecond for the servo to settle 

const int servo_pin = 11;

const int motor1_pin1 = 8;
const int motor1_pin2 = 9;

const int motor2_pin1 = 10;
const int motor2_pin2 = 12;

const int motor1_2_PWM = 6;

const int motor3_pin1 = 3;
const int motor3_pin2 = 2;

const int motor4_pin1 = 4;
const int motor4_pin2 = 5;

const int motor3_4_PWM = 6;

const int trigPin = 13;
const int echoPin = 7;


const float STOP_DIST  = 20.0;  // cm
const float CLEAR_DIST = 40.0;  // cm

enum State { GO, BACKUP, TURN };
State state = GO;

int lastTurnDir = 1; // 1 = right, -1 = left (memory to avoid oscillation)



void setup() 
{ 
  pinMode(motor1_pin1, OUTPUT);
  pinMode(motor1_pin2, OUTPUT);
  pinMode(motor2_pin1, OUTPUT);
  pinMode(motor2_pin2, OUTPUT);
  pinMode(motor1_2_PWM, OUTPUT);

  pinMode(motor3_pin1, OUTPUT);
  pinMode(motor3_pin2, OUTPUT);
  pinMode(motor4_pin1, OUTPUT);
  pinMode(motor4_pin2, OUTPUT);
  pinMode(motor3_4_PWM, OUTPUT);

  myservo.attach(servo_pin);  // attaches the servo on pin 9 to the Servo object
  myservo.write(pos);
  delay(500);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
} 

void stop()
 {
  digitalWrite(motor1_pin1, LOW);
  digitalWrite(motor1_pin2, LOW);

  digitalWrite(motor2_pin1, LOW);
  digitalWrite(motor2_pin2, LOW);

  digitalWrite(motor3_pin1, LOW);
  digitalWrite(motor3_pin2, LOW);

  digitalWrite(motor4_pin1, LOW);
  digitalWrite(motor4_pin2, LOW);
 }

void forward(int speed)
 {
  analogWrite(motor1_2_PWM, speed);
  analogWrite(motor3_4_PWM, speed);

  digitalWrite(motor1_pin1, HIGH);
  digitalWrite(motor1_pin2, LOW);

  digitalWrite(motor2_pin1, HIGH);
  digitalWrite(motor2_pin2, LOW);

  digitalWrite(motor3_pin1, HIGH);
  digitalWrite(motor3_pin2, LOW);

  digitalWrite(motor4_pin1, HIGH);
  digitalWrite(motor4_pin2, LOW);
 }

 void sidewayright(int speed)
 {
  analogWrite(motor1_2_PWM, speed);
  analogWrite(motor3_4_PWM, speed);

  digitalWrite(motor1_pin1, LOW);
  digitalWrite(motor1_pin2, HIGH);

  digitalWrite(motor2_pin1, HIGH);
  digitalWrite(motor2_pin2, LOW);

  digitalWrite(motor3_pin1, LOW);
  digitalWrite(motor3_pin2, HIGH);

  digitalWrite(motor4_pin1, HIGH);
  digitalWrite(motor4_pin2, LOW);
 }

void sidewayleft(int speed)
 {
  analogWrite(motor1_2_PWM, speed);
  analogWrite(motor3_4_PWM, speed);

  digitalWrite(motor1_pin1, HIGH);
  digitalWrite(motor1_pin2, LOW);

  digitalWrite(motor2_pin1, LOW);
  digitalWrite(motor2_pin2, HIGH);

  digitalWrite(motor3_pin1, HIGH);
  digitalWrite(motor3_pin2, LOW);

  digitalWrite(motor4_pin1, LOW);
  digitalWrite(motor4_pin2, HIGH);
 }

 void rotateclockwise(int speed)
 {
  analogWrite(motor1_2_PWM, speed);
  analogWrite(motor3_4_PWM, speed);

  digitalWrite(motor1_pin1, LOW);
  digitalWrite(motor1_pin2, HIGH);

  digitalWrite(motor2_pin1, HIGH);
  digitalWrite(motor2_pin2, LOW);

  digitalWrite(motor3_pin1, HIGH);
  digitalWrite(motor3_pin2, LOW);

  digitalWrite(motor4_pin1, LOW);
  digitalWrite(motor4_pin2, HIGH);
 }

void rotatecounterclockwise(int speed)
 {
  analogWrite(motor1_2_PWM, speed);
  analogWrite(motor3_4_PWM, speed);

  digitalWrite(motor1_pin1, HIGH);
  digitalWrite(motor1_pin2, LOW);

  digitalWrite(motor2_pin1, LOW);
  digitalWrite(motor2_pin2, HIGH);

  digitalWrite(motor3_pin1, LOW);
  digitalWrite(motor3_pin2, HIGH);

  digitalWrite(motor4_pin1, HIGH);
  digitalWrite(motor4_pin2, LOW);
 }

void backward(int speed)
 {
  analogWrite(motor1_2_PWM, speed);
  analogWrite(motor3_4_PWM, speed);

  digitalWrite(motor1_pin1, LOW);
  digitalWrite(motor1_pin2, HIGH);

  digitalWrite(motor2_pin1, LOW);
  digitalWrite(motor2_pin2, HIGH);

  digitalWrite(motor3_pin1, LOW);
  digitalWrite(motor3_pin2, HIGH);

  digitalWrite(motor4_pin1, LOW);
  digitalWrite(motor4_pin2, HIGH);
 }

 void linearmotion(int speed, int x, int y)
 {

  analogWrite(motor1_2_PWM, speed);
  analogWrite(motor3_4_PWM, speed);

  if (x == 0 && y == 1)
  {   
       forward(speed);
       return;
  }
  else if ( x== 0 && y==-1)
  {
      backward(speed);
      return;
  }
  else if ( x==1 && y ==0)
  {
      sidewayright(speed);
      return;
  }
  else if ( x==-1 && y ==0)
  {
      sidewayleft(speed);
      return;
  }
 }
 void rotate(int speed, int clock)
 {

    analogWrite(motor1_2_PWM, speed);
    analogWrite(motor3_4_PWM, speed);

    if (clock == 1)
    {
      rotateclockwise(speed);
      return;
    }
    else
    {
      rotatecounterclockwise(speed);
      return;
    }
 }

void nudgeTurn(int dir)
{
  int turnSpeed = 100;
  int turnMs = 400;     // tune 100-1000
  if (dir > 0) rotateclockwise(turnSpeed);
  else rotatecounterclockwise(turnSpeed);
  delay(turnMs);
  stop();
}

void stepBackup()
{
  backward(100);
  delay(500);
  stop();
}

float look(int angle)
{
  myservo.write(angle);
  delay(s_delay);
  return senseDistanceRobust();
}

 float senseDistance()
 {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // timeout prevents long blocking when no echo is received
    unsigned long duration = pulseIn(echoPin, HIGH, 30000UL); // 30ms timeout

    // If no echo, duration == 0
    if (duration == 0) {
      // Treat as "far away" (no obstacle detected) OR choose a sentinel
      return 999.0;
    }

    float distance = (duration * 0.0343f) / 2.0f;
    return distance;
 }

  float senseDistanceRobust()
 {
    float a = senseDistance(); delay(25);
    float b = senseDistance(); delay(25);
    float c = senseDistance(); delay(25);

    if ((a <= b && b <= c) || (c <= b && b <= a)) return b;
    if ((b <= a && a <= c) || (c <= a && a <= b)) return a;
    return c;
 }

float senseSurface()
{

    myservo.write(110);     // right         
    delay(s_delay);  
    delay(s_delay);          
    Serial.print("Right ");            
    float dist_right = senseDistance();



    myservo.write(85);        //straight      
    delay(s_delay);     
    Serial.print("Center ");                 
    float dist_straight = senseDistance();

    myservo.write(50);       // left       
    delay(s_delay);      
    Serial.print("Left ");                
    float dist_left = senseDistance();

}
void clearforward(int speed)
 {
    myservo.write(110);     // right         
    delay(s_delay);  
    delay(s_delay);          
    Serial.print("Right ");            
    float dist_right = senseDistance();

    myservo.write(85);        //straight      
    delay(s_delay);     
    Serial.print("Center ");                 
    float dist_straight = senseDistance();

    myservo.write(50);       // left       
    delay(s_delay);      
    Serial.print("Left ");                
    float dist_left = senseDistance();

    if ( dist_straight >=998  || dist_right >=998 || dist_left >=998 )
    {
      stop();
      return;
    }


    if ( (dist_straight > (dist_left))  && ( dist_straight > (dist_right)) && (dist_straight >= CLEAR_DIST))
    {
      Serial.println("No Turn");
      forward(speed);
      return;
    }
    else if (dist_straight < CLEAR_DIST  &&   dist_left >= CLEAR_DIST)
    {
      Serial.println("Turn left");
      rotatecounterclockwise(100);
      delay(1000);
      stop();
      return;
    }
    else if (dist_straight < CLEAR_DIST  &&  dist_right >= CLEAR_DIST)
    {
      Serial.println("Turn right");
      rotateclockwise(100);
      delay(1000);
      stop();   
      return;
    }
    else if ( dist_right <= CLEAR_DIST && dist_left <= CLEAR_DIST && dist_straight <= CLEAR_DIST )
    {
      Serial.println("backward");
      backward(100);
      delay(1000);
      stop();
      return;
    }
    else 
    {
        Serial.println("Random");

        int randNum = random(0,100);

        if (randNum < 90)
        {
            forward(100);
        }
        else if ( randNum < 95)
        {
            rotateclockwise(100);
            delay(100);
            stop();   
        }
        else if (randNum < 100)
        {
          rotatecounterclockwise(100);
          delay(100);
          stop();   
        }
        return;
    }
 }

float min3(float a, float b, float c)
{
  float m = a;
  if (b < m) m = b;
  if (c < m) m = c;
  return m;
}

void loop()
{
  
  float front, front_L, front_R;

  front_R = look(40);
 // Serial.println(front_R);

 
   float front_1 = look(90);
  //Serial.println(front);

  front_L = look(140);
  //Serial.println(front_L);

   
   float front_2 = look(90);

   front = (front_1 + front_2)/2.0;



 
  front  = min3(front, front_L, front_R);
  Serial.println(front);

  // Emergency stop zone
  if (front <= STOP_DIST)
  {
    stop();
    state = BACKUP;
  }

  if (state == GO)
  {
    if (front >= CLEAR_DIST)
    {
      if (front > 100){
        forward(80);
        Serial.println("foward full speed");
        }
      else if ( front > 65){
        forward(70);
        Serial.println("forward half speed");
      }
      else
      {
        forward(65);
        Serial.println("forward low speed");
      }
      return;
    }

    // front is "not clear enough" -> decide a turn direction using left/right scan
    float leftD  = front_R;//look(40);

    float rightD = front_L;//look(125);


    int dir;
    if (leftD > rightD) dir = -1;
    else if (rightD > leftD) dir = 1;
    else dir = lastTurnDir;   // tie -> use memory

    // 1 = right, -1 = left (memory to avoid oscillation)

    lastTurnDir = dir;
    stop();
    state = TURN;
  }
  else if (state == BACKUP)
  {
    stepBackup();
    state = TURN;  // after backing up, turn
    Serial.println("reverse");
  }
  else if (state == TURN)
  {
    // turn a little, then go back to GO and re-check
    nudgeTurn(lastTurnDir);
    Serial.println("Turn");
    state = GO;
  }
}