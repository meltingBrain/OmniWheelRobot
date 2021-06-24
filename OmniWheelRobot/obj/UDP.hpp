#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <string>
#include <cstring>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>     //exit()
#include <stdint.h>
#include <signal.h>     //signal()

using namespace std;

class UDPConnection{
	private: 
		int sock;
		struct sockaddr_in addr;

		const char *error_message;
	 
	public :
		UDPConnection(string address, int port);
		~UDPConnection();
		int closeUDP();
	 
		int bindUDP();
		template <typename stype> int sendUDP(const stype* send_data, const int array_length);
		template <typename rtype> int recvUDP(rtype* recv_data, const int array_length);
		int setUDPAsyc(int sync_mode);
		
		int getSocket(){return sock;}
		string getErrorMessage(){return error_message;}
		
		int runUDPConnection();
		
};
