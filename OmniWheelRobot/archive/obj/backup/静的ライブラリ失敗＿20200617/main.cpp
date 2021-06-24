#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <time.h>
#include "ADS1256.hpp"
#include "stdio.h"

#include <string.h>

void  Handler(int signo)
{
    //System Exit
    printf("\r\nEND                  \r\n");
    DEV_ModuleExit();

    exit(0);
}

int main(void)
{
    UDOUBLE ADC[4],i;
    printf("demo\r\n");
    DEV_ModuleInit();
    clock_t start = clock();
    clock_t end = clock();

    // Exception handling:ctrl + c
    signal(SIGINT, Handler);

    if(ADS1256_init() == 1){
        printf("\r\nEND                  \r\n");
        DEV_ModuleExit();
        exit(0);
    }

    while(1){
        start = clock();
        // printf("0 : %f\r\n",ADS1256_GetChannalValue(0)*5.0/0x7fffff);
        // printf("1 : %f\r\n",ADS1256_GetChannalValue(1)*5.0/0x7fffff);
        // printf("2 : %f\r\n",ADS1256_GetChannalValue(2)*5.0/0x7fffff);
        // printf("3 : %f\r\n",ADS1256_GetChannalValue(3)*5.0/0x7fffff);
        // printf("4 : %f\r\n",ADS1256_GetChannalValue(4)*5.0/0x7fffff);
        // printf("5 : %f\r\n",ADS1256_GetChannalValue(5)*5.0/0x7fffff);
        // printf("6 : %f\r\n",ADS1256_GetChannalValue(6)*5.0/0x7fffff);
        // printf("7 : %f\r\n",ADS1256_GetChannalValue(7)*5.0/0x7fffff);
        
        ADS1256_GetAll(ADC);
        for(i=0;i<4;i++){
            printf("%d %f\r\n",i,ADC[i]*5.0/0x7fffff);
        }
        printf("\33[8A");//Move the cursor up 8 lines
        
        end = clock();
        double time = (double)(end -start) / CLOCKS_PER_SEC*1000.0;
        printf("time %lf [ms]\n",time);
    }
    return 0;
}
