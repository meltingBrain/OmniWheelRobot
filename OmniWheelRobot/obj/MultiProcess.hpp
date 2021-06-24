#include <cstdio>
#include <cstdint>

#include <string>
#include <cstring>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>

#include <sys/time.h>
#include <sys/resource.h>

#include <signal.h>

#define prtErr(err)  (printf("Error : %s \n", err.c_str()))
#define prt(str) (printf("\n %s \n", str ))
#define CHECK 100 //includeチェック用

#ifndef MOTOR_NUM
	#define MOTOR_NUM 4
#endif
#ifndef RECV_ARRAY_NUM
	#define RECV_ARRAY_NUM (MOTOR_NUM) + 1
#endif

#define IS_ACTIVATE_OMNI  1
#define IS_ACTIVATE_MOTOR 1

#include "TextReader.hpp"
#include "OmniRobot.hpp"
#include "UDP.hpp"

inline void wait(int stime){usleep(stime);} //[us]

