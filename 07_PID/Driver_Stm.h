/*
 * Driver_Stm.h
 *
 *  Created on: 2024. 6. 27.
 *      Author: user
 */

#ifndef DRIVER_STM_H_
#define DRIVER_STM_H_

#include "Ifx_Types.h"
#include "IfxStm.h"
#include "IfxCpu_Irq.h"

typedef struct {
        uint8 u8nuScheduling1msFlag;
        uint8 u8nuScheduling10msFlag;
        uint8 u8nuScheduling50msFlag;
        uint8 u8nuScheduling100msFlag;
        uint8 u8nuScheduling500msFlag;
} SchedulingFlag;

extern SchedulingFlag stSchedulingInfo;
extern void Driver_Stm_Init(void);

#endif /* DRIVER_STM_H_ */
