
#include <Arduino.h>

const int FORWARDS = 1;
const int BACKWARDS = -1;

const int RIGHT = 1;
const int LEFT = -1;

const int NONE = 0;


class Motor {
	const int _ctrl_pin_1;
	const int _ctrl_pin_2;
	const int _speed_pin;
	int _direction;
	
public:
	Motor(const int ctrl_pin_1,    // the order of control pins has to
		  const int ctrl_pin_2,    // be tested if it suits directions
		  const int speed_pin_pwm) // pin with pulse width modulation 
		  :
		  _ctrl_pin_1(ctrl_pin_1),
		  _ctrl_pin_2(ctrl_pin_2),
		  _speed_pin(speed_pin_pwm),
		  _direction(NONE)
	{ 
		pinMode(_ctrl_pin_1, OUTPUT);
		pinMode(_ctrl_pin_2, OUTPUT);
		pinMode(_speed_pin, OUTPUT);
		
		digitalWrite(_ctrl_pin_1, LOW);
		digitalWrite(_ctrl_pin_2, LOW);
		analogWrite(_speed_pin, 0);
	}
	~Motor() {}
	
	void forwards(const int speed_from_0_to_255)
	{ 
		digitalWrite(_ctrl_pin_1, HIGH);
		digitalWrite(_ctrl_pin_2, LOW);
		analogWrite(_speed_pin, speed_from_0_to_255);
		_direction = FORWARDS;
	}
	
	void backwards(const int speed_from_0_to_255)
	{ 
		digitalWrite(_ctrl_pin_1, LOW);
		digitalWrite(_ctrl_pin_2, HIGH);
		analogWrite(_speed_pin, speed_from_0_to_255);
		_direction = BACKWARDS;
	}
	
	void stop()
	{
		analogWrite(_speed_pin, 0);
		digitalWrite(_ctrl_pin_1, LOW);
		digitalWrite(_ctrl_pin_2, LOW);
		_direction = NONE;
	}
	
	int direction()
	{
		return _direction;
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
	Motor _motor;
	const int _ctrl_pin_brown;
	const int _ctrl_pin_green;
	int _direction;
		
public:
	Rudder (const int motor_ctrl_pin_1, 
		    const int motor_ctrl_pin_2, 
		    const int motor_speed_pin_pwm,
		    const int steer_ctrl_pin_brown,
		    const int steer_ctrl_pin_green)
		    :
		    _motor(motor_ctrl_pin_1, 
				   motor_ctrl_pin_2, 
				   motor_speed_pin_pwm),
			_ctrl_pin_brown(steer_ctrl_pin_brown),
			_ctrl_pin_green(steer_ctrl_pin_green),
			_direction(NONE)
	{
		pinMode(_ctrl_pin_brown, INPUT);
		pinMode(_ctrl_pin_green, INPUT);
	}
	~Rudder() {}
	
	void turn_right()
	{
		if (!digitalRead(_ctrl_pin_brown)) {
			_motor.forwards(255);
		}
		const int start_t = millis();
		while (!digitalRead(_ctrl_pin_brown) && millis()) {
			_motor.forwards(255);
		}
		
	}
	
	
	
};

//~ Motor motor(5,2,4,3);

void setup()
{	
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	
	for (int pin = 2; pin < 13; ++pin) {
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
	}
	//~ for (int pin = 2; pin < 13; ++pin) {
		//~ pinMode(pin,OUTPUT);
		//~ digitalWrite(pin,LOW);
	//~ }
	Serial.begin(9600);
}




void loop()
{	
	//~ while (millis() < 1000 && !digitalRead(6)) {
		//~ motor.forwards(240);
	//~ }
	//~ while (digitalRead(7) && millis() < 2000) {
		//~ motor.backwards(240);
	//~ }
	//~ motor.stop();
	//~ for (int pin = 2; pin < 13; ++pin) {
		//~ Serial.print("p");
		//~ Serial.print(pin);
		//~ Serial.print(" ");
		//~ Serial.print(digitalRead(pin));
		//~ Serial.print("  ");
	//~ }
	Serial.println(analogRead(A0));
		//~ delay(5);
	//~ for (int ii = 0; ii < 5000; ++ii) {
		//~ a[ii] = 0;
	//~ }
	//~ int piezo = analogRead(A0);
	//~ Serial.println(piezo);
	
	//~ if (millis() < 15000) {
		
		//~ if (piezo > 10) {
			//~ motor.stop();
			//~ if (motor.going_forwards()) {
				//~ motor.backwards(255);
			//~ } else {
				//~ motor.forwards(255);
			//~ }
		//~ }
	//~ } else {
		//~ motor.stop();
	//~ }
		
	//~ delay(5);
}
