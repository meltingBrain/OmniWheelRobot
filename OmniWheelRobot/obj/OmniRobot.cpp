#include "OmniRobot.hpp"
OmniRobotDriven* omni_p; //sigaction での Ctrl+c 終了用

OmniRobotDriven::OmniRobotDriven() : MotorDriven(motor)
{
	printf("%d /n",CHECK);
	omni_p = this;
	
	/*initalize Motor GPIO Pin*/
	if(setMotorOutMode(motor) < 0){   //モータのGPIOピンの出力設定
		printf("GPIO Set Error\n");
		printf("err : %s", this->err_message.c_str());
		exit(1);
	}
	if(setAllMotorsPWMConfig(motor, 2000, 100)){ //1st:pin, 2nd:freq, 3rd:range
		prtErr(getErrorMessage());
		exit(1);
	}
	stopAllMotors(this->motor);
	usleep(100);
	
	/*initialize PID Gain*/
	//位置型PIDのゲイン設定
	string strp[3];
	bool statusp = txtRead::readTextLine(strp, "./config/PID_GAIN_POSITION.txt");
	if(!statusp)
		exit(1);
	this->Kp_pos = stof(strp[0]);
	this->Ki_pos = stof(strp[1]);
	this->Kd_pos = stof(strp[2]);
	string strv[3];
	//速度型PIDのゲイン設定
	bool statusv = txtRead::readTextLine(strv, "./config/PID_GAIN_VELOCITY.txt");
	if(!statusv)
		exit(1);
	this->Kp_vel = stof(strv[0]);
	this->Ki_vel = stof(strv[1]);
	this->Kd_vel = stof(strv[2]);
	printf("\rPOS : Kp:%f, Kd:%f, Ki:%f\n", this->Kp_pos, this->Kd_pos, this->Ki_pos);	
	printf("\rVEL : Kp:%f, Kd:%f, Ki:%f\n", this->Kp_vel, this->Kd_vel, this->Ki_vel);	
	usleep(100);
	
	/* initialize members*/ 
    for(int i = 0; i < MOTOR_NUM; i++)
    {
		this->pre_torque_d[i] = .0;
		this->error_integral[i] = .0;
		this->pre_pid[i] = .0;
		this->senser_offset[i]  = .0;
		for(int j = 0; j<SAMPLE_NUM; j++) adc_arr[i][j] = .0;
	}
	this->mode = 0;
    this->cycle_start = .0;
	
#if IS_ACTIVE_ADC > 0
	/*init A/D Converter*/
	DEV_ModuleInit();
    if(ADS1256_init() == 1){
	printf("\n ");
		exitRobot();
        exit(1);
    }
    usleep(100);
    measADCOffset(); //set senser offset
 
    /*csvファイルのオープン*/
    wcsv.setFilePath("./plotter/Forbidden/akashi.csv");
	wcsv.openCSV();
#endif	
	/*Ctrl+C設定*/
	memset(&sig, 0, sizeof(this->sig));
	sig.sa_sigaction = exitHandler; //Ctrl+Cでの停止時処理
	sig.sa_flags = SA_SIGINFO;
    
    sleep(3);
}

void OmniRobotDriven::measADCOffset()//センサ値のオフセット値設定(平均)
{
	float cycle_num = 500;
	
	float sum[MOTOR_NUM] = {.0};
	for(int cycle = 0; cycle < cycle_num; cycle++){
		float adc[MOTOR_NUM];
		for(int i=adc_num; i<MOTOR_NUM+adc_num; i++){
			sum[i-adc_num] += ((float)ADS1256_GetChannalValue(i)*5.0/0x7fffff - this->voltage_ref);
		}
		//usleep(5*1000);
	}
	
	for(int j=0; j<MOTOR_NUM; j++){
		this->senser_offset[j] = (float)(sum[j]/cycle_num);
		printf("ADS values [%d] : %f\n", j, (float)(ADS1256_GetChannalValue(j+adc_num)*5.0/0x7fffff) - this->voltage_ref);
		printf(" senser offset[%d] : %f\n", j, this->senser_offset[j]);
		printf(" ADS values [%d] : %f\n", j, (float)(ADS1256_GetChannalValue(j+adc_num)*5.0/0x7fffff) - this->voltage_ref - this->senser_offset[j]);
	}
}

OmniRobotDriven::~OmniRobotDriven()
{
	exitRobot();
}

void OmniRobotDriven::exitRobot()
{
	stopAllMotors(this->motor);
	//exitGPIO(this->motor);//by class MotorDriven
	wcsv.closeCSV();
    printf("\n Do you know KIT-IDEALS ?");
    printf("\n read me : https://www.kanazawa-it.ac.jp/about_kit/kit_ideals.html\n\n");
#if IS_ACTIVE_ADC > 0
    DEV_ModuleExit();
#endif
    exit(0);
}

/*Main*/
int OmniRobotDriven::driveOmniRobot(float *rdata) //オムニホイールロボットの駆動制御メイン文
{
	//printf("\n\n rdata [0] %f\n\n", 0, rdata[1]);
	divideRecvData(rdata); //パイプからの受信データを分割, mode, valへの分配
	switch(this->mode){
		case 0 : 
			printf("MODE : STOP\n");
			for(int i=0; i<MOTOR_NUM;i++){
				this->error_integral[i] = .0f;
#if IS_ACTIVE_ADC > 0
				printf("  ADC[%d] : %f\n", i,(float)(ADS1256_GetChannalValue(i+2)*5.0/0x7fffff) - this->voltage_ref-senser_offset[i]);
#endif
			}

			stopAllMotors(this->motor);
			break;
			
		case 1 :
#if IS_ACTIVE_ADC > 0
			printf("MODE : TORQUE FEEDBACK\n");
			TorqueFeedbackMode();
#else 
			printf("Error : Not Mounted ADC");
#endif			
			break;
			
		case 2 :
			printf("MODE : PWM\n");
			PWMThroughMode();
			break;
			
		case 3 :
			printf("MODE : Empty\n");
			printf("  This mode is unused\n");
			stopAllMotors(this->motor);
		    break;
		    
		default:
			for(int i=0; i<MOTOR_NUM;i++)
				this->error_integral[i] = .0f;
			printf("MODE :Illegal Command\n");
			stopAllMotors(this->motor);
	}
	if(sigaction(SIGINT, &sig, NULL)<0){ //Ctrl+cでの非常停止
		printf("exit called by main stream\n");
		exitRobot();
	}
	return 0;
}

/* divide data */
void OmniRobotDriven::divideRecvData(float *rdata) //受信データの分割関数
{
	selectMode((unsigned int)rdata[0]); //第一要素を mode に指定
	for(int i = 1; i < RECV_ARRAY_NUM; i++){ //第二要素以下のデータをvalに指定
		if(rdata[i] == NULL) val[i-1] = 0.0;
		else val[i-1] = rdata[i];
	}
}

/*######## Torque Feedback Mode #######*/
int OmniRobotDriven::TorqueFeedbackMode() //val : Desired Torque
{
	float adc[MOTOR_NUM];
	static int insert_no = 0; //insert number
	static float mes_stimer = 0;

    for(int i = 0; i < MOTOR_NUM; i++){
		adc[i] = calcMovingAverage(adc_arr[i], (float)(ADS1256_GetChannalValue(i+adc_num)*5.0/0x7fffff), insert_no);
		adc[i] -= senser_offset[i];
		//printf("ADC[%d] : %f\r\n", i, adc[i]);
    }
    if(insert_no < SAMPLE_NUM-1) insert_no++;
	else insert_no = 0;

	float torque[MOTOR_NUM];
	for(int j=0; j<MOTOR_NUM; j++)
	{
		float current = Adc2Current(adc[j]);
		torque[j] = Current2Torque(current);
	}

	float pid[MOTOR_NUM];
	for(int l=0; l<MOTOR_NUM; l++)
	{
		if(pre_torque_d[l] != val[l]){
			error_integral[l] = .0f;
		}
	}
	PIDControllerPosition(pid, val, torque);
	//PIDControllerVelocity(pid, val, torque);
	float time_st = cycle_start;
	int pid_i[4] = {0};
	for(int k=0; k<MOTOR_NUM; k++)
	{
		pid_i[k] = (int)round(pid[k]);
		pre_torque_d[k] = val[k];
		//pid_i[k] = (int)pid[k];
		//printf("pid[%d] : %f \n", k, round(pid[k]));
		//printf("pid[%d] : %d \n", k, pid_i[k]);
	}
	driveAllMotors(motor, pid_i);
    
    //////
	float mes_etimer = (float)clock();
	for(int i=0; i<MOTOR_NUM; i++){
		wcsv.writeHorizon(val[i]);  /* val torque pid_i */
		wcsv.writeHorizon(torque[i]);
		wcsv.writeHorizon(pid_i[i]);
	}
	wcsv.writeVertical(mes_etimer-mes_stimer);
	mes_stimer = mes_etimer;
	//////
	return 0;
}

/*###### PWM ########*/
int OmniRobotDriven::PWMThroughMode() //val : output PWM
{
	int duty_cycle[MOTOR_NUM] = {(int)this->val[0], (int)this->val[1], (int)this->val[2], 0};
	if(MOTOR_NUM == 4) 
		duty_cycle[MOTOR_NUM-1] = (int)this->val[3];
	
	driveAllMotors(motor, duty_cycle);
	
	return 0;
}

/*Moving Average*/
float OmniRobotDriven::calcMovingAverage(float* sample, float new_sample, int n)
{
	sample[n] = new_sample;
	float sum = 0;
	for(int i=0; i<SAMPLE_NUM; i++){
		sum += sample[i];
	} 
	return sum*_SAMPLE_NUM;
}

/*PID Control*/
//位置型PID
void  OmniRobotDriven::PIDControllerPosition(float* pid, float* val_desired, float* val_sens)
{
	static float pre_error[MOTOR_NUM] = {.0f};
	float cycle_end = (float)clock();
	for(int i = 0; i<MOTOR_NUM; i++){
		float val_error = calcError(val_desired[i], val_sens[i]);
		if(error_integral[i] == .0f) {
			pre_error[i] = .0f;
		}
		if(abs(val_error) < this->thr_error) {
			val_error = .0f;
		}
		float d_error = calcDifferential(val_error, pre_error[i]);
		
		pid[i] = Kp_pos*val_error + Ki_pos*error_integral[i] + Kd_pos*d_error;
		pre_error[i] = val_error;
		error_integral[i] += val_error;
		//error_integral[i] =  calcIntegrationTrapezoidal(error_integral[i], val_error, pre_error[i], (cycle_end-this->cycle_start)/CLOCKS_PER_SEC);//
	}
	this->cycle_start = cycle_end;
}

//速度型PID
void  OmniRobotDriven::PIDControllerVelocity(float* pid, float* val_desired, float* val_sens)
{
	static float pre_error[MOTOR_NUM] = {.0f};

	float cycle_end = (float)clock();
	for(int i = 0; i<MOTOR_NUM; i++){
		float val_error = calcError(val_desired[i], val_sens[i]);
		if(val_desired[i] == .0f){
			val_error = .0f;
		}
		else if(abs(val_error) < this->thr_error){ 
			val_error = .0f;
		}
		error_integral[i] =  Ki_vel * calcIntegral(error_integral[i], val_error);//
		//error_integral[i] = Ki_vel * calcIntegrationTrapezoidal(error_integral[i], val_error, pre_error[i], (cycle_end-this->cycle_start)/CLOCKS_PER_SEC);//
		float d_error = Kd_vel * calcDifferential(val_error, pre_error[i]);
		
		pid[i] = Kp_vel*val_error + error_integral[i] + d_error;
		pid[i] += pre_pid[i];
		pre_pid[i] = pid[i];
		
		pre_error[i] = val_error;
	}
	
	this->cycle_start = cycle_end;
}

//台形積分(問題あり, 使用非推奨)
float OmniRobotDriven::calcIntegrationTrapezoidal(float val_integral, float val, float val_pre, float cycle_time) //台形則
{
	return val_integral + ((val+val_pre)*cycle_time*0.5);
}

/*Ctrl+Cでの非常停止プログラム*/
void exitHandler(int id, siginfo_t* info, void *ctx)
{
	printf("exit called by handle\n");
	omni_p->exitRobot();
}
