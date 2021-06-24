#include "MultiProcess.hpp"

pid_t pid;  //忌み子(消すな)

void  pHandler(int signo) //シグナル用コールバック関数
{
    //System Exit
    kill(0, SIGINT); //ctrl+c
    
    exit(0);
}

int runChildProcess(int pipefd[2]) //UDP通信, パイプにデータを書き込む側
{
	//pipe & poll setting
	close(pipefd[0]); //子プロセスではパイプの出口側を閉じる
	struct pollfd w_poll[1];
	w_poll[0].fd = pipefd[1]; //パイプの入り口をポーリングに指定する. 
	//fileno(); 
	w_poll[0].events = POLLWRNORM | POLLERR; //ポーリングイベントに"書き込み可能"と"エラー"を指定. 
	
	//get remote IP address & IP Port from text
	string address = "";
	int port = 0;
	string str[2];
	bool status = txtRead::readTextLine(str, "./config/UDP_CONFIG.txt");
	if(!status)
		return -1;
	address = str[0];
	port    = stoi(str[1]); //string -> int
	printf("\rAddress : %s\n", address.c_str());
	printf("Port : %d\n", port);
	
	//UDP Connect
	UDPConnection udp(address,port);	
	if(udp.getSocket() == -1){
		printf("%s \n", udp.getErrorMessage().c_str());
		return -1;
	}
	if(udp.bindUDP()<0){
		printf("Failed to Bind End Point\n");
		exit(0);
	}
	udp.setUDPAsyc(0);
	printf("UDP Connected \n");
	sleep(10);
	
	float wdata[RECV_ARRAY_NUM] = {0}; //パイプに書き込むデータ
	int array_length = sizeof(wdata)/sizeof(wdata[0]);
	while(1){
		clock_t start = clock();
		//printf("\nUDP PROCESS : %d \n" , start);
		int recv_status = udp.recvUDP(wdata, array_length); //recv_status = 受信したデータのバイト数
		if(recv_status < 0){
			if(errno = 11)
			{
				printf("  Error Message : %s\n", udp.getErrorMessage().c_str());
			}
			else{
				printf("\nReceive Error \n");
				printf("  Error Message : %s\n", udp.getErrorMessage().c_str());
				udp.closeUDP();
				exit(1);
			}
		}
		else{
			//pipe write
			/*for(int i = 0; i < RECV_ARRAY_NUM;i++){
				printf("upd data[%d] : %f \n", i, wdata[i]);
			}*/
			
			poll(w_poll, 1, -1);
			if(w_poll[0].events & POLLWRNORM){ //書き込み可能かの判定
				if(write(pipefd[1], &wdata,sizeof(wdata)) < 0) //パイプへの書き込み
				{
					printf("Failed to Write \n");
				}
				/*else{
					for(int i = 0; i < RECV_ARRAY_NUM;i++){
						printf("write_data[%d] : %f \n", i, wdata[i]);
					}
				}*/
			}
			//else printf("\nwrite passed\n");	
		}
		
		clock_t end = clock();
	
        double time = (double)(end -start) / CLOCKS_PER_SEC*1000.0;
        usleep(10*1000-time*1000);
        //printf("UDP time %lf [ms]\r\n",time);//子プロセスの周期表示
	}
	
	return 0;
}

int runParentProcess(int pipefd[2]) //オムニロボットの駆動制御, パイプからデータを受け取る側 
{
	close(pipefd[1]); //親プロセスではパイプの入り口側を閉じる
	struct pollfd r_poll[1]; 
	r_poll[0].fd = pipefd[0]; //ポーリングにパイプの出口を指定.
	r_poll[0].events = POLLIN | POLLERR; //イベントに"読み出し可能", "エラー"を指定

	OmniRobotDriven omni;
	
	float rdata[RECV_ARRAY_NUM] = {0}; //パイプからの受信データ
	int array_length = sizeof(rdata)/sizeof(rdata[0]);
	while(1){
		clock_t start = clock();
		//printf("\nPARENT PROCESS : %d \n" , start);
		
		//polling
        poll(r_poll, 1, 0);
		if(r_poll[0].revents & POLLIN){//読み込み可能か判定
			if(read(pipefd[0], &rdata,sizeof(rdata))< 0){//RECV_ARRAY_NUMだと壊れちゃう
				printf("Failed to Read \n");
				printf("Error : %s\n", strerror(errno));
			}
			else {
				for(int j = 0; j < RECV_ARRAY_NUM; j++){
					printf("Read_data[%d / %d] : %f \n", j+1, RECV_ARRAY_NUM, rdata[j]);
				}
			}
		}
		
		//drive
		omni.driveOmniRobot(rdata);
		
        clock_t end = clock();
        double time = (double)(end -start) / CLOCKS_PER_SEC*1000.0;
        //printf("OMNI time %lf [ms]\r\n",time); //親プロセスの周期表示
	}
	return 0;
}

int main(void)
{
	int pipefd[2]; //プロセス間データ転送パイプのID
	const char *error_message;

	signal(SIGINT, pHandler); //Ctrl+Cによる終了時の処理を指定
	if(pipe(pipefd) != 0)
	{
		printf("Error : Failed to Create \"READ PIPE\" \n");
		exit(1);
	} 
	pid = fork(); //マルチプロセスの生成(フォーク形式)
	if(pid < 0){
		printf("Error : Failed to Generate Multi Process \n");
		exit(1);
	}
	if(pid == 0){/*########### P I P E  W R I T E #############*/
		printf("##CHILD PROCESS START##\n");//Write
		if(setpriority(PRIO_PROCESS,0, 0)<0){ //プロセスの優先度を 0 に設定
			printf("Error : Failed to Set Priority (Child Process)");
		}
		printf("Child priority : %d \n", getpriority(PRIO_PROCESS,0));
		sleep(2);
		runChildProcess(pipefd);
		printf("##CHILED PROCESS END##\n");
		
		_exit(0);
	}
	else{/*########### P I P E  R E A D #############*/
		
		printf("##PARENT PROCESS START##\n");//Read
		if(setpriority(PRIO_PROCESS,0, 1)<0){ //プロセスの優先度を 1 に設定
			printf("Error : Failed to Set Priority (Parent Process)");
		}	
		printf("priority : %d \n", getpriority(PRIO_PROCESS,0));
		sleep(2);
		runParentProcess(pipefd);
		printf("##PARENT PROCESS END##\n");
		
		exit(0);
	}

	return 0;
}
