/* font utf-8  */

#include "UDP.hpp"

/* コンストラクタ(IPアドレス, IPポート) */
UDPConnection::UDPConnection(string address, int port)
{
	this->error_message = "";
	
	/*Socketの生成*/
	this->sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(this->sock == -1){
		this->error_message = strerror(errno);
		printf("Error : Failed to Genelate UDP Socket");
	}
	else {
		this->addr.sin_family      = AF_INET;
		this->addr.sin_addr.s_addr = inet_addr(address.c_str());
		this->addr.sin_port        = htons(port);
	}
}

/* デストラクタ */
UDPConnection::~UDPConnection()
{
	closeUDP();
}

int UDPConnection::closeUDP()
{
	if(close(this->sock) != 0){
		printf("Error : Failed to Close UDP Connection\n");
		this->error_message = strerror(errno);
		return -1;
	}
	else{
		printf("\n\n##### Socket Closed ####\n");
		return 0;
	}
}

/* 受信先とのバインド */
int UDPConnection::bindUDP()
{
	if(bind(this->sock, (struct sockaddr *)&this->addr, sizeof(this->addr)) < 0)
	{
		this->error_message = strerror(errno);
		return -1;
	}
	
	return 0;
}

/*送信*/
template <typename stype>  
int UDPConnection::sendUDP(const stype* send_data, const int array_length)
{
	const int byte_num  =  sizeof(stype) * array_length;
	char bytes[byte_num];
	
	memset(bytes, 0, sizeof(bytes)); //bytesの初期化
	memcpy(bytes, send_data, sizeof(bytes)); //送信したいデータをバイトに変換
	
	int send_status = sendto(this->sock, send_data, array_length, 0, (struct sockaddr *)&addr, sizeof(addr)); //送信
	if (send_status < 0) 
		this->error_message = strerror(errno);
	return send_status;
}
//template関数の明示的実体化(ヘッダとソースを分ける際に必要)
template int UDPConnection::sendUDP<char>(const char* send_data, const int array_length);
template int UDPConnection::sendUDP<int>(const int* send_data, const int array_length);
template int UDPConnection::sendUDP<float>(const float* send_data, const int array_length);
template int UDPConnection::sendUDP<double>(const double* send_data, const int array_length);

/* データ受信(型不問)*/
// 第一引数に応じて受信データの型が決まる, 送信元の型と第一引数の型を一致させる必要あり
template <typename rtype> 
int UDPConnection::recvUDP(rtype* recv_data, const int array_length)
{
	const int byte_num = sizeof(rtype)*array_length;
	char bytes[byte_num]; 	//char    = 1 byte
							//char[2] = 1 byte * 2 個 = 2 byte

	memset(bytes, 0, sizeof(bytes)); //bytesの初期化
	int recv_status = recv(this->sock, bytes, sizeof(bytes), 0); //UDP受信
	if(recv_status > 0){
		memcpy(recv_data, bytes, sizeof(bytes)); //受信したバイトを数値データへ変換
		return recv_status;
	}
	else if(recv_status == 0){//async ON 時のみ有効
		printf("rdata: NA \n");
		memset(bytes, 0, sizeof(bytes));
		memcpy(recv_data, bytes, sizeof(bytes));
		return 418;
	}	
	else{
		this->error_message = strerror(errno);
		return -1;
	}
}
//template関数の明示的実体化(ヘッダとソースを分ける際に必要)
template int UDPConnection::recvUDP<char>(char* recv_data, const int array_length);
template int UDPConnection::recvUDP<int>(int* recv_data, const int array_length);
template int UDPConnection::recvUDP<float>(float* recv_data, const int array_length);
template int UDPConnection::recvUDP<double>(double* recv_data, const int array_length);

/*非同期通信のON/OFF ( ON : 1 | OFF : 0 )*/
int UDPConnection::setUDPAsyc(int sync_mode)
{
	if(ioctl(sock, FIONBIO, &sync_mode)<0){
		this->error_message = strerror(errno);
		return -1;
	}
	return 0;
}

/*実行部の関数実装(未使用)*/
int UDPConnection::runUDPConnection()
{
	
}

