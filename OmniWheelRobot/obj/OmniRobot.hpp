#ifndef _OMNI_ROBOT
#define _OMNI_ROBOT

#ifndef IS_ACTIVATE_OMNI
	#define IS_ACTIVATE_OMNI 1
	#define IS_ACTIVATE_MOTOR 1
	#define CHECK 1
	
	#define prtErr(err)  (printf("Error : %s \n", err.c_str()))
	#define prt(str) (printf("\n %s \n", str ))
#endif

#define OMNI_NUM 3
#define MOTOR_NUM 4
#define OUT_PIN_NUM (MOTOR_NUM*2)
#define RECV_ARRAY_NUM (MOTOR_NUM) + 1

/* GPIO 番号 2 - 26 ☓pin番号 */
#define MOTOR1_FORWARD 16
#define MOTOR1_REVERSE 20
#define MOTOR2_FORWARD 12
#define MOTOR2_REVERSE 7
#define MOTOR3_FORWARD 24
#define MOTOR3_REVERSE 25
#define MOTOR4_FORWARD 15
#define MOTOR4_REVERSE 14

/* Moving Average  1/Sample */
#define SAMPLE_NUM 5
#define _SAMPLE_NUM 0.2 // サンプル個数 = 10（高速化のため少数表記( = 1/SAMPLE_NUM)）

/*Current Senser*/
#define SENSITIVITY 0.718804
#define VOLT_REF 2.5

/*Torque Feedback*/
#define TORQUE_CONST 0.002
#define GEAR_RATIO   64.8

/*ADC*/
#define IS_ACTIVE_ADC 0
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#pragma once
#include <stdio.h>
#include <stdlib.h>     //exit()
#include <stdint.h>
#include <signal.h>     //signal()
#include <unistd.h>
#include <string>
#include <cstring>
#include <errno.h>
#include <cmath>

//スレッド処理周り
#include <pthread.h>

//poll
#include <poll.h>
//signal(for Crtl+c)
#include <signal.h>

#include "TextReader.hpp"
#include "MotorDrive.hpp"
#include "AkashicRecords_Genesis.hpp"

/* /archive/libads1256 より*/
uint8_t ADS1256_init();
void ADS1256_GetAll(UDOUBLE *ADC_Value);
UDOUBLE ADS1256_GetChannalValue(UBYTE Channel);
void DEV_ModuleInit();
void DEV_ModuleExit();
void exitHandler(int id, siginfo_t* info, void *ctx);

class OmniRobotDriven : public MotorDriven
{
	private:
		int omni_no = OMNI_NUM;
		Motors motor ={	{MOTOR1_FORWARD, MOTOR1_REVERSE}, //m1
						{MOTOR2_FORWARD, MOTOR2_REVERSE}, //m2
						{MOTOR3_FORWARD, MOTOR3_REVERSE}, //m3
						{MOTOR4_FORWARD, MOTOR4_REVERSE} }; //Motors -> MotorDrive.hpp

		unsigned int mode = 0;
		float val[RECV_ARRAY_NUM-1] = {.0}; //val : received data. 
		float cycle_start;

		/* Robot Paramaeter*/
		float r = 0.024; //wheel radius[m]
		
		/* PID Parameter (using Senser)*/
		float Kp_pos;
		float Ki_pos; 
		float Kd_pos; 
		
		float Kp_vel;
		float Ki_vel;
		float Kd_vel;
		
		float pre_pid[MOTOR_NUM]        = {.0f};
		float error_integral[MOTOR_NUM] = {.0f};
		float thr_error = 0.015;
		
		/* Senser/ADC */
		pthread_t sens_thread;
		float adc_arr[MOTOR_NUM][SAMPLE_NUM] = {{.0}}; //adc data (*5)
		float senser_offset[MOTOR_NUM]       = {.0};   //
		float voltage_ref = 2.5; //Reference Votage[V]
		float i_limit = 1.8;     //Current Limit[A]
		
		/* Other */
		float pre_torque_d[MOTOR_NUM] = {.0f}; //using Chenged Desired Torque
		
		int adc_num = 2;
		
		struct sigaction sig;
		
		WriteCSV wcsv;
		
		
	public:
		OmniRobotDriven();
		~OmniRobotDriven();
	
		int driveOmniRobot(float* rdata); //omni
	
		/* exit */
		void exitRobot();
		
		/*mode*/
		inline unsigned int selectMode(unsigned int omni_mode){return this->mode = omni_mode;}
		int TorqueFeedbackMode();
		int PWMThroughMode();
		
		/*Senser, A/D*/
		void measADCOffset();//
		float calcMovingAverage(float* sample, float new_sample, int n);
		inline float ADC2Current(float val){return SENSITIVITY*(val - VOLT_REF);}
		
		/* receive data */
		void divideRecvData(float *rdata);
		
		/*PID Controller*/
		/*setting kp, kd, ki*/
		void PIDControllerPosition(float* duty_ratio, float* val_deisired, float* val_sens);
		void PIDControllerVelocity(float* duty_ratio, float* val_deisired, float* val_sens);
		inline float calcError(float val_desired, float val_sens){return val_desired-val_sens;}
		inline float calcIntegral(float val_integral, float val_latest){return val_latest+val_integral;} //単純な足し合わせ
		float calcIntegrationTrapezoidal(float val_integral, float val, float val_pre, float cycle_time);//台形則
		float calcDifferential(float val, float val_pre){return val - val_pre;}
		
		/*Torque*/
		inline float Adc2Current(float adc){return SENSITIVITY * (adc - this->voltage_ref);}//分解能から計算
		inline float Current2Torque(float current){return GEAR_RATIO * TORQUE_CONST * current;}//トルク定数の算出
		
		/*Motor*/
		inline unsigned int* getMotor1Pin(int motor_number){return motor.m1;}
		inline unsigned int* getMotor2Pin(int motor_number){return motor.m2;}
		inline unsigned int* getMotor3Pin(int motor_number){return motor.m3;}
		inline unsigned int* getMotor4Pin(int motor_number){return motor.m4;}
		
		/*getter*/
		float* getADCArray(int motor_i){return adc_arr[motor_i];}
		
		/*setter*/
		
		/**/
		void run();
};

#endif
//テキスト読み込み部も
