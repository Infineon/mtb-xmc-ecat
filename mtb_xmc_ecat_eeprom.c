/**
 * @file mtb_xmc_ecat_eeprom.c
 * \version 1.1.0
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
#include "ecatslv.h"

/**
 * EEPROM_READ_SIZE: Only required if EEPROM emulation is active. This value defines the number of bytes which will
 * be read per opertion. */
#ifndef EEPROM_READ_SIZE
#define EEPROM_READ_SIZE                          (0x8U)
#endif

#if ESC_EEPROM_SIZE > MTB_XMC_ECAT_EEPROM_SIZE
#error \
    MTB_XMC_ECAT_EEPROM_SIZE size insufficient to hold requested ESC_EEPROM_SIZE from SSC tool. Check size in the ECAT personality
#endif

/*******************************************************************************
* Global variables
*******************************************************************************/
/* Extern declarations for Beckhoff SSC functions and variables */
extern uint8_t aEepromData[];
extern UINT8 * pEEPROM;


static E_EEPROM_XMC4_t eeprom_context;

/*******************************************************************************
* APIs Implementation
*******************************************************************************/
E_EEPROM_XMC4_STATUS_t mtb_xmc_ecat_eeprom_init(void)
{
    /* Initialize the Flash Emulated EEPROM and copy the latest content to the internal working RAM buffer */
    E_EEPROM_XMC4_STATUS_t e_status = E_EEPROM_XMC4_Init(&eeprom_context, MTB_XMC_ECAT_EEPROM_SIZE);

    if (e_status == E_EEPROM_XMC4_STATUS_OK)
    {
        /* Check if the slave configuration data is available in the Flash Emulated EEPROM*/
        if (E_EEPROM_XMC4_IsFlashEmpty())
        {
            /* Prepare the default configuration out of the SSC for EEPROM */
            E_EEPROM_XMC4_WriteArray(0U, aEepromData, (uint16_t)ESC_EEPROM_SIZE);

            /* Program the RAM contents to Emulated EEPROM*/
            e_status = E_EEPROM_XMC4_UpdateFlashContents();
        }
        else
        {
            /* Read the latest contents from RAM buffer which has the latest contents of EEPROM */
            E_EEPROM_XMC4_ReadArray(0U, aEepromData, (uint16_t)ESC_EEPROM_SIZE);
        }
    }

    return (e_status);
}


__WEAK UINT16 HW_EepromRead(UINT32 wordaddr)
{
    /* This function is only required if EEPROM_EMULATION is enabled and no EEPROM content is created
     * (CREATE_EEPROM_CONTENT == 0). In our implementation CREATE_EEPROM_CONTENT == 1. */
    (void)wordaddr;
    return 0U;
}


/* Beckhoff Hw abstraction layer interface API to write new configuration parameters to slave EEPROM area */
UINT16 HW_EepromWrite(UINT32 wordaddr)
{
    uint32_t status = 0U;
    uint16_t EEPROMReg = 0U; /* Regvalue 0x502 - 0x5003 */

    if ((wordaddr <= (uint32_t)ESC_EEPROM_SIZE) && (pEEPROM != NULL))
    {
        uint16_t * pData = (uint16_t *)(void *)pEEPROM;
        HW_EscRead((MEM_ADDR *)(void *)&pData[(wordaddr)], (uint16_t)ESC_EEPROM_DATA_OFFSET,
                   (uint16_t)EEPROM_WRITE_SIZE);
        if ((wordaddr == (uint32_t)7U) || (wordaddr == (((uint32_t)ESC_EEPROM_SIZE >> (uint32_t)1U) - (uint32_t)1U)))
        {
            /* lets program the complete EEPROM new prepare Data for EEPROM */
            if (E_EEPROM_XMC4_WriteArray((uint16_t)0U, aEepromData, (uint16_t)ESC_EEPROM_SIZE) == true)
            {
                HW_EscReadWord(EEPROMReg, ESC_EEPROM_CONTROL_OFFSET);
                HW_EscWriteWord(EEPROMReg & (~ESC_EEPROM_BUSY_MASK), ESC_EEPROM_CONTROL_OFFSET);

                __disable_irq();
                /* Program the RAM contents to Emulated EEPROM*/
                status = (uint32_t)E_EEPROM_XMC4_UpdateFlashContents();
                __enable_irq();
            }
        }
    }

    return (UINT16)status;
}


/* Beckhoff Hw abstraction layer interface API to reload slave EEPROM area */
UINT16 HW_EepromReload(void)
{
    UINT16 EEPROMReg = 0U; /* Regvalue 0x502 - 0x5003 */
    UINT32 cmd = (uint32_t)EEPROMReg  & (uint32_t)ESC_EEPROM_CMD_MASK;
    UINT32 addr;
    HW_EscReadDWord(addr, (uint32_t)ESC_EEPROM_ADDRESS_OFFSET);
    addr = SWAPDWORD(addr);

    /* EEPROM emulation ( 8Bytes EEPROM data) */
    if ((addr <= (uint32_t)ESC_EEPROM_SIZE) && (pEEPROM != NULL))
    {
        UINT16 * pData = (UINT16 *)(void *)pEEPROM;
        do
        {
            HW_EscWrite((MEM_ADDR *)(void *)&pData[(addr)], (uint16_t)ESC_EEPROM_DATA_OFFSET,
                        (uint16_t)EEPROM_READ_SIZE);

            /* Clear error bits */
            EEPROMReg &= (uint16_t)(~(uint16_t)(ESC_EEPROM_ERROR_MASK));

            /* ack current reload segment */
            HW_EscWriteWord(EEPROMReg, (uint32_t)ESC_EEPROM_CONTROL_OFFSET);

            /* read EEPROM control (to check if the reload is still pending) */
            HW_EscReadWord(EEPROMReg, ESC_EEPROM_CONTROL_OFFSET);
            cmd = (uint32_t)EEPROMReg  & (uint32_t)ESC_EEPROM_CMD_MASK;

            HW_EscReadDWord(addr, (uint32_t)ESC_EEPROM_ADDRESS_OFFSET);
        } while(cmd == (uint32_t)ESC_EEPROM_CMD_RELOAD_MASK);
    }
    else
    {
        /* Set Error  */
        EEPROMReg |= (uint16_t)ESC_EEPROM_ERROR_CMD_ACK;
    }

    return 0U;
}
