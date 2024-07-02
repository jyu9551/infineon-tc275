/*
 * DrvAdc..h
 *
 *  Created on: 2024. 6. 28.
 *      Author: user
 */

#ifndef DRVADC_H_
#define DRVADC_H_

#include "Ifx_Types.h"

typedef enum {
    ADC_GROUP4_CH0 = 0u,
    ADC_GROUP4_CH1,
    ADC_GROUP4_CH2,
    ADC_GROUP4_CH3,
    ADC_GROUP4_CH4,
    ADC_GROUP4_CH5,
    ADC_GROUP4_CH6,
    ADC_GROUP4_CH7,
    ADC_GROUP4_MAX,
} ADC_GROUP0;

typedef struct {
        uint32 UlSSense1_Raw;
        uint32 UlSSense2_Raw;
        uint32 UlSSense3_Raw;
} SensorAdcRaw;

extern SensorAdcRaw stSensorAdcRaw;

extern void DrvAdcInit(void);
extern void DrvAdc_GetAdcRawGroup0(void);

#endif /* DRVADC_H_ */
