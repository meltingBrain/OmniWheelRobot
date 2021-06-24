#ifndef IS_ACTIVATE_MOTOR
	#define IS_ACTIVATE_MOTOR 1
	#define ACTIVATE_MOTOR_MAIN 1
	#define CHECK 0
	
	#define prtErr(err)  (printf("Error : %s \n", err.c_str()))
	#define prt(str) (printf("\n %s \n", str ))
#endif

#ifndef MOTOR_NUM
	#define MOTOR_NUM 4
	#define OUT_PIN_NUM (MOTOR_NUM*2)
#endif

#include <unistd.h>
#include <string>
#include <cstring>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>     //exit()
#include <stdint.h>
#include <signal.h>     //signal()


#include "pigpiod_if2.h"

#define PWM_FREQUENCY 1000
#define PWM_RANGE_MAX 60
#define PWM_RANGE_MIN 0
 
using namespace std;

struct Motors{
	unsigned int m1[2];//{cw, ccw}
	unsigned int m2[2]; 
	unsigned int m3[2];
	unsigned int m4[2];
}; //モータピン格納用構造体.

class  MotorDriven{
	private :
		int pid;
		
	protected:
		string err_message;

	public :
		MotorDriven(Motors motors, char* address = NULL, char* port = NULL);//NULL = address:local host , port:8888
		~MotorDriven();

		void setGPIO(char* address = NULL, char* port = NULL);
		void exitGPIO(Motors motors);

		//GPIO setting
		int setMotorInMode(Motors motors);
		int setMotorOutMode(Motors motors);
		inline int setGPIOInMode(unsigned int pin){return set_mode(this -> pid , pin, PI_INPUT);}
		inline int setGPIOOutMode(unsigned int pin){return set_mode(this -> pid , pin, PI_OUTPUT);}
		inline int getGPIOMode(unsigned int pin){return get_mode(this->pid, pin);}
		
		//PWM setting (frequency etc...)
		int initPWM(unsigned int* pin, unsigned int pwm_freq, unsigned int pwm_range);
		inline int setPWMRange(unsigned int pin, unsigned int pwm_range){return set_PWM_range(this->pid, pin, pwm_range);}
		inline int setPWMFrequency(unsigned int pin, unsigned int pwm_freq){return set_PWM_frequency(this->pid, pin, pwm_freq);}
		
		//output
		void outputMotor(unsigned int motor_pin[2], int duty_cycle);
		inline int outputPWM(unsigned int pin, unsigned int duty_cycle){return set_PWM_dutycycle(this->pid, pin, duty_cycle);}//ソフトウェアPWM
		
		inline int saturatePWM(unsigned int pwm){if(pwm > PWM_RANGE_MAX) return PWM_RANGE_MAX;}
		inline int cutLowerPWM(unsigned int pwm){if(pwm < PWM_RANGE_MIN) return 0;}
		
		//一括司令
		int setAllMotorsPWMConfig(Motors motors, unsigned int pwm_freq, unsigned int pwm_range);
		void driveAllMotors(Motors motors, int* duty_cycle);
		void stopAllMotors(Motors motors);
		int getAllMotorsPWM(Motors motors);
		
		//getter
		int getPiID(){return pid;}
		int getPWMDutyCycle(unsigned int pin){return get_PWM_dutycycle(this->pid, pin);}
		int getPWMRealRange(unsigned int pin){return get_PWM_real_range(this->pid, pin);}
		string getErrorMessage(){return err_message;}
};
