/**
 * @file mtb_xmc_ecat_timer.c
 * \version 1.0.0
 *
 * @cond
 ********************************************************************************
 * \copyright
 * (c) (2025), Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************
 *
 * @endcond
 */

/*******************************************************************************
* HEADER FILES
*******************************************************************************/
#include "mtb_xmc_ecat.h"
#include "xmc_ccu4.h"
#include "ecatappl.h"

/*******************************************************************************
* MACROS
*******************************************************************************/
#define mtb_xmc_ecat_timer_isr MTB_XMC_ECAT_Timer_INTERRUPT_HANDLER /**< Timer's interrupt handler definition */

static uint32_t ticks = 0U;

/*******************************************************************************
* Functions
*******************************************************************************/
void mtb_xmc_ecat_timer_start(void)
{
    XMC_CCU4_SLICE_StartTimer(MTB_XMC_ECAT_Timer_HW);
}


void mtb_xmc_ecat_timer_stop_reset(void)
{
    XMC_CCU4_SLICE_StopTimer(MTB_XMC_ECAT_Timer_HW);
    ticks = 0U;
}


/* ECAT slave timer interrupt handler function */
void mtb_xmc_ecat_timer_isr(void)
{
    ticks++;
    #if defined (ECAT_TIMER_INT) && (ECAT_TIMER_INT == 1)
    ECAT_CheckTimer();
    #endif /* ECAT_TIMER_INT == 1 */
}


/* Beckhoff Hw abstraction layer interface API to get the current timer value in ticks */
UINT32 HW_GetTimer(void)
{
    return (UINT32)ticks;
}


void HW_ClearTimer(void)
{
    ticks = 0U;
}
