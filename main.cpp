
#include <Arduino.h>

#define NO_ERROR 0
#define TURN_ERROR 1;

const int DEBUG = 1;

void debug_print(const char* var_name, int var)
{
	if (DEBUG) {
		Serial.print(var_name);
		Serial.println(var);
	}
}

class Motor {
	const int controlPin1;
	const int controlPin2;
	const int speedPin;
	const int ZERO_SPEED;
public:
	/* the order of control pins has to be tested if it suits 
	 * direction
	 */
	Motor(const int controlPin1,
		  const int controlPin2,
		  const int speedPinWithPWM) // PWM = Pulse Width Modulation
		  :
		  controlPin1(controlPin1),
		  controlPin2(controlPin2),
		  speedPin(speedPinWithPWM),
		  ZERO_SPEED(0)
	{ 
		pinMode(controlPin1, OUTPUT);
		pinMode(controlPin2, OUTPUT);
		pinMode(speedPin, OUTPUT);
		
		digitalWrite(controlPin1, LOW);
		digitalWrite(controlPin2, LOW);
		analogWrite(speedPin, ZERO_SPEED);
	}
	~Motor() {}
	
	void turnClockwise(const int speedFrom0To255)
	{ 
		digitalWrite(controlPin1, HIGH);
		digitalWrite(controlPin2, LOW);
		analogWrite(speedPin, speedFrom0To255);
	}
	
	void turnCounterClockwise(const int speedFrom0To255)
	{ 
		digitalWrite(controlPin1, LOW);
		digitalWrite(controlPin2, HIGH);
		analogWrite(speedPin, speedFrom0To255);
	}
	
	void stop()
	{
		analogWrite(speedPin, ZERO_SPEED);
		digitalWrite(controlPin1, LOW);
		digitalWrite(controlPin2, LOW);

	}
};


// if packa turning to the right <- motor has to turn to the left

// then wait until Brown (now pin 2) is HIGH
// Green (now pin 2) will be also HIGH

// if packa going back to straight from the right

// then wait (run the motor) until Green  will get LOW

// if packa going left 

// then wait until Green is HIGH

// if packa going back to straight from left 

// then wait until Brown gets LOW




class Rudder { // Rudder = Kormidlo

	const int RIGHT;
	const int HALF_RIGHT;
	const int STRAIGHT;
	const int HALF_LEFT;
	const int LEFT;
	const int LEFT_OR_RIGHT;
	const int SLOW_SPEED;
	const int FAST_SPEED;
	const unsigned long MAX_TURN_TIME;
	Motor motor;
	const int controlPinBrownWire;
	const int controlPinGreenWire;
	int position;
		
public:
	Rudder (const int motorControlPin1, 
		    const int motorControlPin2, 
		    const int motorSpeedPinWithPWM,
		    const int steerControlPinBrownWire,
		    const int steerControlPinGreenWire)
		    :
		    RIGHT(2),
		    HALF_RIGHT(1),
		    STRAIGHT(0),
		    HALF_LEFT(-1),
		    LEFT(-2),
		    LEFT_OR_RIGHT(3),
		    SLOW_SPEED(100),
			FAST_SPEED(255),
			MAX_TURN_TIME(2000),
		    motor(motorControlPin1, 
				   motorControlPin2, 
				   motorSpeedPinWithPWM),
			controlPinBrownWire(steerControlPinBrownWire),
			controlPinGreenWire(steerControlPinGreenWire),
			position(STRAIGHT)
	{
		pinMode(controlPinBrownWire, INPUT);
		pinMode(controlPinGreenWire, INPUT);
	}
	~Rudder() {}
	
	int turnToLeft()
	{
		if (position == LEFT) return NO_ERROR;
		int errorCode = NO_ERROR;
		if (position == RIGHT) errorCode = this->turnToStraight();
		debug_print("turn to straight error code ", errorCode);
		if (errorCode) return errorCode;
		errorCode = this->turnLeftFromStraight();
		debug_print("turn to left error code ", errorCode);
		if (errorCode) return errorCode;
		position = LEFT;
		return NO_ERROR;
	}
	
	int turnToRight()
	{
		if (position == RIGHT) return NO_ERROR;
		int errorCode = NO_ERROR;
		if (position == LEFT) errorCode = this->turnToStraight();
		if (errorCode) return errorCode;
		errorCode = this->turnRightFromStraight();
		if (errorCode) return errorCode;
		position = RIGHT;
		return NO_ERROR;
	}
	
	int turnToStraight()
	{
		if (position == STRAIGHT) return NO_ERROR;
		int errorCode = NO_ERROR;
		if (position == LEFT) errorCode = this->turnToStraightFromLeft();
		if (position == RIGHT) errorCode = this->turnToStraightFromRight();
		if (errorCode) return errorCode;
		position = STRAIGHT;
		return NO_ERROR;		
	}
	
	int getPosition() 
	{
		int valueOnBrownWire = digitalRead(controlPinBrownWire);
		delay(1);
		int valueOnGreenWire = digitalRead(controlPinGreenWire);
		delay(1);
		if (!valueOnBrownWire) {
			if (!valueOnGreenWire) {
				return STRAIGHT;
			} else {
				return HALF_RIGHT;
			}
		} else {
			if (!valueOnGreenWire) {
				return HALF_LEFT;
			} else {
				return LEFT_OR_RIGHT;
			}
		}
	}

private:

	int turnLeftFromStraight()
	{
		const unsigned long motorStartTime = millis();
		motor.turnClockwise(FAST_SPEED);
		while (this->getPosition() != LEFT_OR_RIGHT) {
			if (millis() - motorStartTime > MAX_TURN_TIME) {
				motor.stop();
				return TURN_ERROR;
			}
		}
		delay(20);
		motor.stop();
		position = LEFT;
		return NO_ERROR;			
	}

	int turnRightFromStraight() 
	{
		const unsigned long motorStartTime = millis();
		motor.turnCounterClockwise(FAST_SPEED);
		while (this->getPosition() != LEFT_OR_RIGHT) {
			if (millis() - motorStartTime > MAX_TURN_TIME) {
				motor.stop();
				return TURN_ERROR;
			}
		}
		delay(20);
		motor.stop();
		position = RIGHT;
		return NO_ERROR;			
	}

	int turnToStraightFromLeft() 
	{
		const unsigned long motorStartTime = millis();
		motor.turnCounterClockwise(SLOW_SPEED);
		while (this->getPosition() != STRAIGHT) {
			if (millis() - motorStartTime > MAX_TURN_TIME) {
				motor.stop();
				return TURN_ERROR;
			}
		}
		motor.stop();
		position = STRAIGHT;
		return NO_ERROR;			
	}
	
	int turnToStraightFromRight()
	{
		const unsigned long motorStartTime = millis();
		motor.turnClockwise(SLOW_SPEED);
		while (this->getPosition() != STRAIGHT) {
			if (millis() - motorStartTime > MAX_TURN_TIME) {
				motor.stop();
				return TURN_ERROR;
			}
		}
		motor.stop();
		position = STRAIGHT;
		return NO_ERROR;		
	}
	

};

class Drive {
	Motor motor;
public:
	/* the order of control pins has to be tested if it suits 
	 * direction
	 */
	Drive(const int controlPin1,
		  const int controlPin2,
		  const int speedPinWithPWM) // PWM = Pulse Width Modulation
		  :
		  motor(controlPin1, controlPin2, speedPinWithPWM) {}
	~Drive() {}	
	
	void goForwards(const int speedFrom0To255)
	{
		motor.turnClockwise(speedFrom0To255);
	}
	
	void goBackwards(const int speedFrom0To255)
	{
		motor.turnCounterClockwise(speedFrom0To255);
	}
	
	void stop()
	{
		motor.stop();
	}
	
};

int doXtimes = 1;

Rudder rudder(2,4,3,5,6);
Drive drive(7,8,9);

void setup()
{	
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);

	Serial.begin(9600);
}




void loop()
{	
	if (doXtimes) {
		rudder.turnToLeft();
		drive.goForwards(255);
		delay(1000);
		rudder.turnToStraight();
		drive.stop();
		delay(1000);
		rudder.turnToRight();
		drive.goBackwards(255);
		delay(1000);
		drive.stop();
		rudder.turnToStraight();
		//~ Serial.println(rudder.turnToLeft());
		//~ Serial.println(rudder.getPosition());
		//~ delay(500);
		//~ Serial.println(rudder.turnToStraight());
		//~ Serial.println(rudder.getPosition());
		//~ delay(500);
		//~ Serial.println(rudder.turnToRight());
		//~ Serial.println(rudder.getPosition());
		//~ delay(500);
		//~ Serial.println(rudder.turnToStraight());
		//~ Serial.println(rudder.getPosition());
		//~ delay(500);
		--doXtimes;
	} else {
		//~ Serial.println(rudder.getPosition());
		//~ delay(100);
	}
}
