/*
 * Scheduler.c
 *
 *  Created on: 2024. 6. 27.
 *      Author: user
 */

#include "Scheduler.h"
#include "Driver_Stm.h"
#include "IfxPort.h"
#include "IfxPort_PinMap.h"
#include "DrvAdc.h"
#include "GtmTomPwmHl.h"

#include <stdio.h>
#include <stdint.h>

#define MidAdc_GetAdcGroup0SenseRaw()   (&stSensorAdcRaw)

typedef struct {
        uint32_t u32nuCnt1ms;
        uint32_t u32nuCnt10ms;
        uint32_t u32nuCnt50ms;
        uint32_t u32nuCnt100ms;
        uint32_t u32nuCnt500ms;
} TestCnt;

static void AppTask1ms(void);
static void AppTask10ms(void);
static void AppTask50ms(void);
static void AppTask100ms(void);
static void AppTask500ms(void);
float32 LPF(float32 t_old, float32 x, float32 Ts, float32 band);

TestCnt stTestCnt;

char s, s_old;
int Direction;
float32 y_old;
int PosCnt;
int theta, theta_old;

static void AppNoTask(void){
    IfxPort_setPinModeInput(IfxPort_P02_1.port, IfxPort_P02_1.pinIndex, IfxPort_InputMode_pullDown);
    IfxPort_setPinModeInput(IfxPort_P02_0.port, IfxPort_P02_0.pinIndex, IfxPort_InputMode_pullDown);

    uint8_t ENCA = IfxPort_getPinState(&MODULE_P02,1);
    uint8_t ENCB = IfxPort_getPinState(&MODULE_P02,0);
    if (ENCA == FALSE && ENCB == FALSE) s = '0';
    else if (ENCA == FALSE && ENCB == TRUE) s = '1';
    else if (ENCA == TRUE && ENCB == TRUE) s = '2';
    else if (ENCA == TRUE && ENCB == FALSE) s = '3';

    if (s - s_old == 1 || s - s_old == -3){ // 역방향
       PosCnt--;
       Direction = 1;
    }
    else if (s - s_old == -1 || s - s_old == 3){ // 정방향
       PosCnt++;
       Direction = -1;
    }
    s_old = s;

    int Pos_rad = PosCnt * 2 * 3.14159265358979323/48;  // 각속도    // 48 = 12 * 4
    int Pos_deg = (int)(PosCnt*360/48);                 // 각도

    uint8_t ENCA_old = ENCA;
    uint8_t ENCB_old = ENCB;
    theta = Pos_rad;
}

float32 t;
int ACnt, Wd;
float32 w, w_old, w_ref;
uint32 W_RPM;

int Vin;
float32 error_w, error_w_old, error_w_int, error_w_int_old;
float32 duty[2];
float32 kp=0.1, ki=0.3;


static void AppTask1ms(void){
   // _out_uart3(W_RPM);
    ACnt++;
    Wd = 2;
    float32 Ts = 0.001;

    t = ACnt * Ts;

    w = (theta-theta_old)/Ts;
    w = LPF(w_old, w, Ts, 500.0);
    w_old = w;
    W_RPM = (uint32)(60*w/(2*3.141592));
    theta_old = theta;


    if (t<=4) w_ref = 0;
    else if (t>4 && t<19)  w_ref = 0.6667*Wd*(2*3.14)*(t-4);
    else if (t>19 && t<26) w_ref = Wd*(3.14*2);
    else if (t>26 && t<41) w_ref = Wd*(3.14*2)-0.6667*Wd*(2*3.14)*(t-26);
    else if (t>41)         w_ref = 0;

    if (t <= 4) Vin=0;
    else if (t>4 && t<41){
        error_w = w_ref - w;
        error_w_int = error_w_old + (error_w)*Ts;   // error integral
        error_w_int_old = error_w_int;

        if (error_w_int > 10) error_w_int = 10;     // Anti wind up

        Vin = (kp*error_w + ki*error_w_int);

        if (Vin > 11) Vin=11;
        else if (Vin < 0) Vin=0;
    }
    else if (t > 41) Vin = 0;

    // duty
    if (t<=4) {
        duty[0] = 0.1;
        duty[1] = 0.1;
    }
    else if (t>4 && t<41){
        duty[0] = Vin/12.0;
        duty[1] = Vin/12.0;
    }
    else if (t>41) {
        duty[0] = 0;
        duty[1] = 0;
    }

    g_GtmTomPwmHl.tOn[0] = duty[0];
    g_GtmTomPwmHl.tOn[1] = duty[1];
    GtmTomPwmHl_run();
    stTestCnt.u32nuCnt1ms++;
}
static void AppTask10ms(void) {    stTestCnt.u32nuCnt10ms++;   }
static void AppTask50ms(void) {    stTestCnt.u32nuCnt50ms++;   }
char tmp[100];
static void AppTask100ms(void){
    sprintf(tmp,"%d",W_RPM);
    int i=0;
    while(tmp[i]!=NULL){
        _out_uart3(tmp[i++]);
    }
    _out_uart3('\r');
    _out_uart3('\n');
    stTestCnt.u32nuCnt100ms++;  }
static void AppTask500ms(void){    stTestCnt.u32nuCnt500ms++;  }

float32 LPF(float32 t_old, float32 x, float32 Ts, float32 band){
    double A1 = Ts/(Ts+1/band);
    float32 y = y_old + A1*(x-y_old);
    return y;
}

void AppScheduling(void){
    AppNoTask();

    if (stSchedulingInfo.u8nuScheduling1msFlag == 1u){
        stSchedulingInfo.u8nuScheduling1msFlag = 0u;
        AppTask1ms();

        if (stSchedulingInfo.u8nuScheduling10msFlag == 1u){
            stSchedulingInfo.u8nuScheduling10msFlag = 0u;
            AppTask10ms();
        }
        if (stSchedulingInfo.u8nuScheduling50msFlag == 1u){
            stSchedulingInfo.u8nuScheduling50msFlag = 0u;
            AppTask50ms();
        }
        if (stSchedulingInfo.u8nuScheduling100msFlag == 1u){
            stSchedulingInfo.u8nuScheduling100msFlag = 0u;
            AppTask100ms();
        }
        if (stSchedulingInfo.u8nuScheduling500msFlag == 1u){
            stSchedulingInfo.u8nuScheduling500msFlag = 0u;
            AppTask500ms();
        }
    }
}
