//Prateek
//www.prateeks.in

#include <SPI.h>
#include <MFRC522.h>

#define RFID_SDA 10
#define RFID_SCK 13
#define RFID_MOSI 11
#define RFID_MISO 12
#define RFID_RST 9

#define IR_SENSOR_RIGHT 14
#define IR_SENSOR_LEFT 15
#define MOTOR_SPEED 10

//Right motor
int enableRightMotor = 3;
int rightMotorPin1 = 2;
int rightMotorPin2 = 7;

//Left motor
int enableLeftMotor = 6;
int leftMotorPin1 = 4;
int leftMotorPin2 = 5;

#define STOP_TIME 3000
#define BEEP_LENGTH 200
#define MOTOR_SPEED 110
#define MINIMUM_TIME_BETWEEN_CARDS 2000

MFRC522 mfrc522(10, 9);
long int last_card_read;
#define COMMANDS_LENGTH 4
char* Type[4] = {"Stop", "Beep&stop", "Beep&ignore", "Stop"};
int Value[4] = {3000, 1000, 0, 5000};
char* CardID[4] = {"B2 3B E2 E2", "20 9B EF 2F", "ED B9 E0 2B", "83 87 3B 2E"};


void setup()
{

  SPI.begin();
  mfrc522.PCD_Init();

  TCCR0B = TCCR0B & B11111000 | B00000010 ;

  // put your setup code here, to run once:
  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  pinMode(IR_SENSOR_RIGHT, INPUT);
  pinMode(IR_SENSOR_LEFT, INPUT);
  rotateMotor(0, 0);
}


void loop()
{

  int rightIRSensorValue = digitalRead(IR_SENSOR_RIGHT);
  int leftIRSensorValue = digitalRead(IR_SENSOR_LEFT);

  //If none of the sensors detects black line, then go straight
  if (rightIRSensorValue == LOW && leftIRSensorValue == LOW)
  {
    rotateMotor(MOTOR_SPEED, MOTOR_SPEED);
  }
  //If right sensor detects black line, then turn right
  else if (rightIRSensorValue == HIGH && leftIRSensorValue == LOW )
  {
    rotateMotor(-MOTOR_SPEED, MOTOR_SPEED);
  }
  //If left sensor detects black line, then turn left
  else if (rightIRSensorValue == LOW && leftIRSensorValue == HIGH )
  {
    rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);
  }
  //If both the sensors detect black line, then stop
  else
  {
    rotateMotor(0, 0);
  }


  if (millis() - last_card_read >= MINIMUM_TIME_BETWEEN_CARDS) {
    //here we have to wait for card, when it is near the sensor
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    //we can read it's value
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    content = content.substring(1);
    for (int a = 0; a < COMMANDS_LENGTH; a++) {
      last_card_read = millis();
      if (content == CardID[a]) {
        if (Type[a] == "Stop") {
          Stop();
          delay(Value[a]);
        } else if (Type[a] == "Ignore") {
        }
      }
    }
  }
}


void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{

  if (rightMotorSpeed < 0)
  {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
  }
  else if (rightMotorSpeed > 0)
  {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  }
  else
  {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
  }

  if (leftMotorSpeed < 0)
  {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
  }
  else if (leftMotorSpeed > 0)
  {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
  }
  else
  {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
  }
  analogWrite(enableRightMotor, abs(rightMotorSpeed));
  analogWrite(enableLeftMotor, abs(leftMotorSpeed));
}
void Stop() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  delay(30000);
}
