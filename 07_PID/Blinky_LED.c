/*
 * LEDBlink.c
 *
 *  Created on: 2024. 6. 27.
 *      Author: user
 */

#include "IfxPort.h"
#include "Bsp.h"

#define LED         &MODULE_P10,2
#define WAIT_TIME   500

void initLED(void) {
    // Init LED
    IfxPort_setPinModeOutput(LED, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    // Switch OFF the LED
    IfxPort_setPinLow(LED);
}

void blinkLED(void) {
    // Toggle the state of the LED
    IfxPort_togglePin(LED);

    // wait 500 millisceonds
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME));
}

void toggleLED(void){
    IfxPort_togglePin(LED);
}
