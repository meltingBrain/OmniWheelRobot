//#include "MotorDrive.KIT_IDEALS"
#include "MotorDrive.hpp"

//isActivate 1:Hardware PWM, 0:Software PWM
MotorDriven::MotorDriven(Motors motors, char* address, char* port) 
{
	printf("Pigpio Version : %d\n", pigpiod_if_version());
	setGPIO(address, port);
	if(this -> pid < 0){
		printf("Error : Failed to Start Pigpio\n");
		printf("Did you input \"sudo pidpiod\" in Terminal ?");
		exit(1);
	}
}

MotorDriven::~MotorDriven()
{
	pigpio_stop(this->pid);
}

void MotorDriven::setGPIO(char* address, char* port)
{
	this->pid = pigpio_start(address, port);
}

void MotorDriven::exitGPIO(Motors motors)
{
	setMotorInMode(motors);
	pigpio_stop(this->pid);
}

/* Setting(GPIO) */
int MotorDriven::setMotorInMode(Motors motors)
{
	for(int i=0; i<2; i++){ //各モータのcw, ccwピンを入力モードへ設定
		if(setGPIOInMode(motors.m1[i]) < 0){
			this->err_message = strerror(errno);
			return -1;
		}
		if(setGPIOInMode(motors.m2[i]) < 0){
			this->err_message = strerror(errno);
			return -1;
		}
		if(setGPIOInMode(motors.m3[i]) < 0){
			this->err_message = strerror(errno);
			return -1;
		}
		if(MOTOR_NUM == 4){
			if(setGPIOInMode(motors.m4[i]) < 0){
				this->err_message = strerror(errno);
				return -1;
			}
		}
		
	}

	return 0;
}

int MotorDriven::setMotorOutMode(Motors motors)
{
	for(int i=0; i<2; i++){ //各モータのcw, ccwピンを出力モードへ設定
		if(setGPIOOutMode(motors.m1[i]) < 0){
			this->err_message = strerror(errno);
			return -1;
		}
		if(setGPIOOutMode(motors.m2[i]) < 0){
			this->err_message = strerror(errno);
			return -1;
		}
		if(setGPIOOutMode(motors.m3[i]) < 0){
			this->err_message = strerror(errno);
			return -1;
		}
		if(MOTOR_NUM == 4){
			if(setGPIOOutMode(motors.m4[i]) < 0){
				this->err_message = strerror(errno);
				return -1;
			}
		}
	}

	return 0;
}

/* Setting(PWM) */
int MotorDriven::initPWM(unsigned int* pin, unsigned int pwm_freq, unsigned int pwm_range)
{
	for(int i = 0; i < 2; i++){ //引数に指定したモータのcw, ccwピンのPWM出力設定
		if(setPWMFrequency(pin[i], pwm_freq) < 0) //PWM周波数設定
			return -1;
		if (setPWMRange(pin[i], pwm_range) < 0) //PWM出力レンジの設定
			return -1;
	}
	return 0;
}


/* Output(PWM) */
void MotorDriven::outputMotor(unsigned int motor_pin[2], int duty_cycle) //添字 0:cw, 1:ccw
{
	if(duty_cycle < 0){
		printf("\n\n motor %d",duty_cycle);
		duty_cycle = -duty_cycle;
		outputPWM(motor_pin[0], 0); //cw = 0
		outputPWM(motor_pin[1], saturatePWM(cutLowerPWM(duty_cycle))); //ccw = duty_cycle
		printf("PWM : %d\n\n", getPWMDutyCycle(motor_pin[1]));
	}
	else {
		printf("\n\n motor %d",duty_cycle);
		outputPWM(motor_pin[1], 0); //ccw = 0
		outputPWM(motor_pin[0], saturatePWM(cutLowerPWM(duty_cycle))); //cw = duty_cycle
		printf("PWM : %d\n\n", getPWMDutyCycle(motor_pin[0]));
	}
}

/* Batch Motors Control */
int MotorDriven::setAllMotorsPWMConfig(Motors motors, unsigned int pwm_freq, unsigned int pwm_range) //すべてのモータのPWM設定
{
	initPWM(motors.m1, pwm_freq, pwm_range);
	initPWM(motors.m2, pwm_freq, pwm_range);
	initPWM(motors.m3, pwm_freq, pwm_range);
	if(MOTOR_NUM == 4) initPWM(motors.m4, pwm_freq, pwm_range);
	
	return 0;
}

void MotorDriven::driveAllMotors(Motors motors, int* duty_cycle) //指定した値ですべてのモータを駆動
{
	outputMotor(motors.m1, duty_cycle[0]);
	outputMotor(motors.m2, duty_cycle[1]);
	outputMotor(motors.m3, duty_cycle[2]);
	if(MOTOR_NUM == 4)outputMotor(motors.m4, duty_cycle[3]);
}

void MotorDriven::stopAllMotors(Motors motors) //すべてのモータの停止
{
	outputPWM(motors.m1[0], 0);
	outputPWM(motors.m1[1], 0);
	outputPWM(motors.m2[0], 0);
	outputPWM(motors.m2[1], 0);
	outputPWM(motors.m3[0], 0);
	outputPWM(motors.m3[1], 0);
	if(MOTOR_NUM == 4){
		outputPWM(motors.m4[0], 0);
		outputPWM(motors.m4[1], 0);
	}
}
