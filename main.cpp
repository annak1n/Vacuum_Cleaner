
#include <Arduino.h>

const int DEBUG = 1;

int digital_read(uint8_t pin)
{
	int value = digitalRead(pin);
	delayMicroseconds(50);
	return value;
}

void debug_print(const char* var_name, int var)
{
	if (DEBUG) {
		Serial.print(var_name);
		Serial.println(var);
	}
}

class Motor {
	const int _FORWARDS;
	const int _BACKWARDS;
	const int _NONE;
	const int _ctrl_pin_1;
	const int _ctrl_pin_2;
	const int _speed_pin;
	int _direction;
	
public:
	Motor(const int ctrl_pin_1,    // the order of control pins has to
		  const int ctrl_pin_2,    // be tested if it suits directions
		  const int speed_pin_pwm) // pin with pulse width modulation 
		  :
		  _FORWARDS(1),
		  _BACKWARDS(-1),
		  _NONE(0),
		  _ctrl_pin_1(ctrl_pin_1),
		  _ctrl_pin_2(ctrl_pin_2),
		  _speed_pin(speed_pin_pwm),
		  _direction(_NONE)
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
		_direction = _FORWARDS;
	}
	
	void backwards(const int speed_from_0_to_255)
	{ 
		digitalWrite(_ctrl_pin_1, LOW);
		digitalWrite(_ctrl_pin_2, HIGH);
		analogWrite(_speed_pin, speed_from_0_to_255);
		_direction = _BACKWARDS;
	}
	
	void stop()
	{
		analogWrite(_speed_pin, 0);
		digitalWrite(_ctrl_pin_1, LOW);
		digitalWrite(_ctrl_pin_2, LOW);
		_direction = _NONE;
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

	const int _RIGHT;
	const int _HALF_RIGHT;
	const int _CENTER;
	const int _HALF_LEFT;
	const int _LEFT;
	const int _SLOW_SPEED;
	const int _FAST_SPEED;
	Motor _motor;
	const int _ctrl_pin_brown;
	const int _ctrl_pin_green;
	int _last_position;
		
public:
	Rudder (const int motor_ctrl_pin_1, 
		    const int motor_ctrl_pin_2, 
		    const int motor_speed_pin_pwm,
		    const int steer_ctrl_pin_brown,
		    const int steer_ctrl_pin_green)
		    :
		    _RIGHT(2),
		    _HALF_RIGHT(1),
		    _CENTER(0),
		    _HALF_LEFT(-1),
		    _LEFT(-2),
		    _SLOW_SPEED(100),
			_FAST_SPEED(255),
		    _motor(motor_ctrl_pin_1, 
				   motor_ctrl_pin_2, 
				   motor_speed_pin_pwm),
			_ctrl_pin_brown(steer_ctrl_pin_brown),
			_ctrl_pin_green(steer_ctrl_pin_green),
			_last_position(_CENTER)
	{
		pinMode(_ctrl_pin_brown, INPUT);
		pinMode(_ctrl_pin_green, INPUT);
	}
	~Rudder() {}
	
	int go_left()
	{
		
	}
	
	int go_right()
	{
		
	}
	
	int go_straight()
	{
		int position = this->get_position();
		while (position != _CENTER) {
			switch (position) {
				case 2: // _RIGHT
				case 1: // _HALF_RIGHT
					this->turn_packa_left(_SLOW_SPEED);
					break;
				case -1: // _HALF_LEFT
				case -2: // _LEFT
					this->turn_packa_right(_SLOW_SPEED);
					break;
			}
			position = this->get_position();
		}
		_motor.stop();
	}

private:

	void turn_packa_left(const int speed) // speed in [0..255]
	{
		_motor.backwards(speed);
	}	
	
	void turn_packa_right(const int speed) // speed in [0..255]
	{
		_motor.forwards(speed);
	}	
	
//~ private:
	int turn_right_from_center()
	{
		debug_print("Position at the start: ",_position);
		debug_print("Brown pin at start: ", digital_read(_ctrl_pin_brown));
		debug_print("Green pin at start: ", digital_read(_ctrl_pin_green));
		if (!this->is_in_error_state() && _position == _CENTER) {
			_motor.forwards(255);
			const int start_t = millis();
			while (millis() - start_t < 5000) {
				debug_print("Brown pin ",digital_read(_ctrl_pin_brown));
				if (digital_read(_ctrl_pin_brown)) {
					_motor.stop();
					break;
				}
			}
			_motor.stop();
			debug_print("Position in the middle: ", _position);
			debug_print("Brown pin at the end: ", digital_read(_ctrl_pin_brown));
			debug_print("Green pin at the end: ", digital_read(_ctrl_pin_green));
			if (!digital_read(_ctrl_pin_brown) || 
				!digital_read(_ctrl_pin_green)) {
				return 1; // error - rudder didn't get to the position
			} else {
				_position = _RIGHT;
				debug_print("Position at the end: ", _position);
				return 0;
			}			
		} else {
			return 1;
		}
	}
	
	int turn_left_from_center()
	{
		debug_print("Position at the start: ",_position);
		debug_print("Brown pin at start: ", digital_read(_ctrl_pin_brown));
		debug_print("Green pin at start: ", digital_read(_ctrl_pin_green));
		if (!this->is_in_error_state() && _position == _CENTER) {
			_motor.backwards(255);
			const int start_t = millis();
			while (millis() - start_t < 5000) {
				debug_print("Greem pin ",digital_read(_ctrl_pin_green));
				if (digital_read(_ctrl_pin_green)) {
					_motor.stop();
					break;
				}
			}
			_motor.stop();
			debug_print("Position in the middle: ", _position);
			debug_print("Brown pin at the end: ", digital_read(_ctrl_pin_brown));
			debug_print("Green pin at the end: ", digital_read(_ctrl_pin_green));
			if (!digital_read(_ctrl_pin_brown) || 
				!digital_read(_ctrl_pin_green)) {
				return 1; // error - rudder didn't get to the position
			} else {
				_position = _LEFT;
				debug_print("Position at the end: ", _position);
				return 0;
			}			
		} else {
			return 1;
		}
	}
	
	int return_to_center_from_left()
	{
		debug_print("Position at the start: ",_position);
		debug_print("Brown pin at start: ", digital_read(_ctrl_pin_brown));
		debug_print("Green pin at start: ", digital_read(_ctrl_pin_green));
		if (!this->is_in_error_state() && _position == _LEFT) {
			_motor.forwards(100);
			const int start_t = millis();
			while (millis() - start_t < 1000) {
				debug_print("Brown pin ",digital_read(_ctrl_pin_brown));
				if (!digital_read(_ctrl_pin_brown)) {
					_motor.stop();
					break;
				}
			}
			_motor.stop();
			debug_print("Position in the middle: ", _position);
			debug_print("Brown pin at the end: ", digital_read(_ctrl_pin_brown));
			debug_print("Green pin at the end: ", digital_read(_ctrl_pin_green));
			if (digital_read(_ctrl_pin_brown) && 
				digital_read(_ctrl_pin_green)) {
				return 1; // error - rudder didn't get to the position
			} else if (!digital_read(_ctrl_pin_brown) && 
					   digital_read(_ctrl_pin_green)) {
				debug_print("Went too far: ", 1);
				_motor.backwards(100);
				const int start_t = millis();
				while (millis() - start_t < 1000) {
					debug_print("Green pin correction ",digital_read(_ctrl_pin_green));
					if (!digital_read(_ctrl_pin_green)) {
						_motor.stop();
						break;
					}
				}
				_motor.stop();	
				if (digital_read(_ctrl_pin_brown) || 
					digital_read(_ctrl_pin_green)) {
					debug_print("Even after correction not successful ", 1);
					return 1; // error - rudder didn't get to the position	
				} else {
					_position = _CENTER;
					debug_print("Position at the end: ", _position);
					return 0;		
				}					
			} else {
				_position = _CENTER;
				debug_print("Position at the end: ", _position);
				return 0;
			}			
		} else {
			return 1;
		}
	}
	

	
	int return_to_center_from_right()
	{
		if (!this->is_in_error_state() && _position == _RIGHT) {
			_motor.backwards(100);
			const int start_t = millis();
			while (millis() - start_t < 1000) {
				if (!digital_read(_ctrl_pin_green)) {
					_motor.stop();
					break;
				}
			}
			_motor.stop();
			if (digital_read(_ctrl_pin_brown) || 
				digital_read(_ctrl_pin_green)) {
				return 1; // error - rudder didn't get to the position
			} else {
				_position = _CENTER;
				return 0;
			}			
		} else {
			return 1;
		}
	}
	
	
	
	int is_in_error_state()
	{
		if (_position == _CENTER) {
			if (!digital_read(_ctrl_pin_brown) && 
				!digital_read(_ctrl_pin_green)) return 0;
			else return 1;
		} else if (_position == _RIGHT || _position == _LEFT) {		
			if (digital_read(_ctrl_pin_brown) && 
				digital_read(_ctrl_pin_green)) return 0;
			else return 1;
		} else {
				return 1;
		}
	}
	
		
	
	int get_position() {
		if (!digital_read(_ctrl_pin_brown) && 
			!digital_read(_ctrl_pin_green)) {
			_last_position = _CENTER;
			return _CENTER;
		} else if (!digital_read(_ctrl_pin_brown) && 
				 digital_read(_ctrl_pin_green)) { 
			_last_position = _HALF_RIGHT;
			return _HALF_RIGHT;
		} else if (digital_read(_ctrl_pin_brown) && 
				 !digital_read(_ctrl_pin_green)) {	 
			_last_position = _HALF_LEFT;
			return _HALF_LEFT;
		}
		if (!digital_read(_ctrl_pin_brown) && 
			!digital_read(_ctrl_pin_green)) return _CENTER;
		else if 
		return _position;
	}
		
};

int do_x_times = 1;

Rudder rudder(2,4,3,5,6);

void setup()
{	
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);

	Serial.begin(9600);
}




void loop()
{	
	if (do_x_times) {
		Serial.println(rudder.turn_right_from_center());
		//~ delay(500);
		Serial.println(rudder.return_to_center_from_right());
		//~ delay(500);
		Serial.println(rudder.turn_left_from_center());
		//~ delay(500);
		Serial.println(rudder.return_to_center_from_left());
		//~ delay(500);
		--do_x_times;
	} else {
		
	}
}
