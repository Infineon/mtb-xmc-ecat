/**
 * @file mtb_xmc_ecat.c
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
#include "ecatappl.h"
#include "applInterface.h"
#include "XMC_ESCObjects.h"


/*******************************************************************************
* Global variables
*******************************************************************************/
/* Extern declarations for Beckhoff SSC functions and variables */
extern uint8_t aEepromData[];
extern UINT8 * pEEPROM;

/*******************************************************************************
* MACROS
*******************************************************************************/
#define MTB_XMC_ECAT_STATUS_SUCCESS             (0U)
#define MTB_XMC_ECAT_STATUS_ERROR               (1U)


/** Parameter check macros */
#define MTB_XMC_ECAT_PRIORITY_VALID(priority)                ((0U <= (priority)) && (63U >= (priority)))
#define MTB_XMC_ECAT_TIMER_PRIORITY_VALID(timer_priority)    ((0U <= (timer_priority)) && (63U >= (timer_priority)))
#define MTB_XMC_ECAT_STATUS_VALID(status)                     (0U == (status))

/*******************************************************************************
* API IMPLEMENTATION
*******************************************************************************/
__WEAK void process_app(TOBJ7000 * OUT_GENERIC, TOBJ6000 * IN_GENERIC)
{
    (void)OUT_GENERIC;
    (void)IN_GENERIC;
}


/* EtherCAT Beckhoff HW abstraction layer interface API to initialize XMC ESC */
UINT16 HW_Init(void)
{
    uint32_t prior_level_ecat = 0U;
    uint32_t prior_level_timer = 0U;
    UINT16 status = MTB_XMC_ECAT_STATUS_ERROR;
    /* EEPROM interface API registration */
    pEEPROM = &aEepromData[0U];

    /* EtherCAT slave enable and intialization using LLD function */
    XMC_ECAT_Init((XMC_ECAT_CONFIG_t *)(void *)aEepromData);

    gpio_init_p0_link_status();
    gpio_init_p0_rxd3();
    gpio_init_p0_rxd2();
    gpio_init_p0_rxd1();
    gpio_init_p0_rxd0();
    gpio_init_p0_rx_dv();
    gpio_init_p0_rx_clk();
    gpio_init_p0_rx_err();
    gpio_init_p0_tx_clk();

    #if defined (MTB_XMC_ECAT_PORT1_ENABLED)
    gpio_init_p1_link_status();
    gpio_init_p1_rxd3();
    gpio_init_p1_rxd2();
    gpio_init_p1_rxd1();
    gpio_init_p1_rxd0();
    gpio_init_p1_rx_dv();
    gpio_init_p1_rx_clk();
    gpio_init_p1_rx_err();
    gpio_init_p1_tx_clk();
    #endif /* MTB_XMC_ECAT_PORT1_ENABLED */

    gpio_init_ecat_mdio();

    MTB_XMC_ECAT_Initialization();

    gpio_init_p0_txd3();
    gpio_init_p0_txd2();
    gpio_init_p0_txd1();
    gpio_init_p0_txd0();
    gpio_init_p0_tx_ena();

    #if defined (MTB_XMC_ECAT_PORT1_ENABLED)
    gpio_init_p1_txd3();
    gpio_init_p1_txd2();
    gpio_init_p1_txd1();
    gpio_init_p1_txd0();
    gpio_init_p1_tx_ena();
    #endif /* MTB_XMC_ECAT_PORT1_ENABLED */

    gpio_init_ecat_phy_clk25();
    gpio_init_ecat_mclk();
    gpio_init_p0_led_link_act();
    #if defined (MTB_XMC_ECAT_PORT1_ENABLED)
    gpio_init_p1_led_link_act();
    #endif /* MTB_XMC_ECAT_PORT1_ENABLED */
    gpio_init_ecat_led_run();
    gpio_init_ecat_led_err();
    gpio_init_ecat_phy_reset();

    prior_level_ecat = NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
                                           MTB_XMC_ECAT_IRQN_PRIORITY,
                                           0U);
    XMC_ASSERT("MTB_XMC_ECAT priority level: priority is not valid\n", MTB_XMC_ECAT_PRIORITY_VALID(prior_level_ecat));

    prior_level_timer = NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
                                            MTB_XMC_ECAT_TIMER_IRQN_PRIORITY,
                                            0U);
    XMC_ASSERT("MTB_XMC_ECAT_TIMER priority level: priority is not valid\n",
               MTB_XMC_ECAT_TIMER_PRIORITY_VALID(prior_level_timer));

    if ((prior_level_ecat == MTB_XMC_ECAT_IRQN_PRIORITY) && (prior_level_timer == MTB_XMC_ECAT_TIMER_IRQN_PRIORITY))
    {
        /* ECAT interrupt priority settings */
        NVIC_SetPriority(MTB_XMC_ECAT_IRQN, prior_level_ecat);

        /* ECAT timer interrupt priority settings */
        NVIC_SetPriority(MTB_XMC_ECAT_Timer_IRQN, prior_level_timer);

        /* EtherCAT slave interrupt enable */
        ENABLE_ESC_INT();

        status = MTB_XMC_ECAT_STATUS_SUCCESS;
    }

    return (status);
}


/* Beckhoff Hw abstraction layer interface API to release the consumed resources */
__WEAK void HW_Release(void)
{
}


/* Beckhoff Hw abstraction layer interface API to reset the XMC ECAT module */
__WEAK void HW_RestartTarget(void)
{
}


/* EtherCAT initialization function */
uint32_t mtb_xmc_ecat_init(void)
{
    uint32_t status = MTB_XMC_ECAT_STATUS_ERROR;

    mtb_xmc_ecat_eeprom_init();

    status = HW_Init();
    XMC_ASSERT("MTB_XMC_ECAT HW_Init status: HW_Init returns error status\n", MTB_XMC_ECAT_STATUS_VALID(status));

    /* Call Beckhoff HW abstraction layer API to start the stack interface initialization*/
    if (status == (uint32_t)MTB_XMC_ECAT_STATUS_SUCCESS)
    {
        status = MainInit();
        XMC_ASSERT("MTB_XMC_ECAT MainInit status: MainInit returns error status\n", MTB_XMC_ECAT_STATUS_VALID(status));

        if (status == (uint32_t)MTB_XMC_ECAT_STATUS_SUCCESS)
        {
            pAPPL_EEPROM_Reload = HW_EepromReload;
            pAPPL_EEPROM_Write  = HW_EepromWrite;

            /* Start the 1 millisec timer */
            mtb_xmc_ecat_timer_start();
        }
    }

    return (status);
}


/* Enabling of user specific EtherCAT Interrupt Routines. */
__WEAK void mtb_xmc_enable_user_int(void)
{
}


/* Disabling of user specific EtherCAT Interrupt Routines. */
__WEAK void mtb_xmc_disable_user_int(void)
{
}


/* ECAT slave interrupt handler function */
void ECAT0_0_IRQHandler(void)
{
    PDI_Isr();
}


void ENABLE_ESC_INT(void)
{
    NVIC_ClearPendingIRQ(MTB_XMC_ECAT_IRQN);
    NVIC_EnableIRQ(MTB_XMC_ECAT_IRQN);
    mtb_xmc_enable_user_int();
    NVIC_ClearPendingIRQ(MTB_XMC_ECAT_Timer_IRQN);
    NVIC_EnableIRQ(MTB_XMC_ECAT_Timer_IRQN);
}


void DISABLE_ESC_INT(void)
{
    uint32_t ics;
    ics = XMC_EnterCriticalSection();

    mtb_xmc_disable_user_int();

    NVIC_DisableIRQ(MTB_XMC_ECAT_IRQN);
    NVIC_ClearPendingIRQ(MTB_XMC_ECAT_IRQN);
    NVIC_DisableIRQ(MTB_XMC_ECAT_Timer_IRQN);
    NVIC_ClearPendingIRQ(MTB_XMC_ECAT_Timer_IRQN);

    XMC_ExitCriticalSection(ics);
}
