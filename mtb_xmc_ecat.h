/**
 * @file mtb_xmc_ecat.h
 * \version 1.0.0
 *
 * @cond
 *******************************************************************************
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


/**
 *<!-- The ECAT documentation structure -->
 * \defgroup group_ecat_general EtherCAT (ECAT)
 * \{
 *  \defgroup group_ecat_macros Macros
 *  \defgroup group_ecat_functions API Prototypes
 *  \endcond
 * \}
 *<!-- End of the ECAT documentation structure -->
 *
 * \addtogroup group_ecat_general
 * \{
 * @brief EtherCAT Low level driver for XMC4800/XMC4300 series.
 *
 * EtherCAT is an Ethernet-based fieldbus system.
 * The EtherCAT Slave Controller (ECAT) read the data addressed to them while the telegram passes through the device.
 * An EtherCAT Slave Controller (ESC) takes care of the EtherCAT communication as an interface between the EtherCAT
 * fieldbus and the slave application. EtherCAT uses standard IEEE 802.3 Ethernet frames, thus a standard network
 * controller can be used and no special hardware is required on master side. EtherCAT has a reserved EtherType of
 * 0x88A4 that distinguishes it from other Ethernet frames. Thus, EtherCAT can run in parallel to other Ethernet
 * protocols. EtherCAT does not require the IP protocol, however it can be encapsulated in IP/UDP. The EtherCAT
 * Slave Controller processes the frame in hardware. Thus, communication performance is independent from processor
 * power.
 *
 * The XMC_ECAT low level driver provides functions to configure and initialize the ECAT hardware peripheral.
 * For EtherCAT stack integration, the necessary hardware accees layer APIs shall be explicitly implemented depending
 * upon the stack provider. The XMC_ECAT lld layer provides only the hardware initialization functions for start up and
 * basic functionalities.
 *
 *  \}
 **/

#ifndef MTB_XMC_ECAT_H
#define MTB_XMC_ECAT_H

/***********************************************************************************************************************
 * HEADER FILES
 **********************************************************************************************************************/
#include "xmc_common.h"
#include "xmc_ecat.h"
#include "xmc4_e_eeprom.h"
#include "ecat_def.h"
#include "applInterface.h"
#include "cybsp.h"


#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup group_ecat_macros
 *  \{
 */
/**********************************************************************************************************************
* MACROS
**********************************************************************************************************************/
#define ECAT_TIMER_INC_P_MS (1U) /**< Timer increment value */

/** \} group_ecat_macros */

/** Internal documentation */
#define pEsc ((uint8_t *)ECAT0)  /**< EtherCAT module address */

extern uint8_t aEepromData[];

/** \addtogroup group_ecat_functions
 *  \{
 */
/***********************************************************************************************************************
 * API Prototypes
 **********************************************************************************************************************/

#define HW_EscReadDWord(DWordValue, Address)     ((DWordValue) = (UINT32)(((volatile UINT32 *)pEsc)[((Address)>>2)]))
/**< \brief 32Bit ESC read access. */

#define HW_EscReadDWordIsr(DWordValue, Address)  HW_EscReadDWord(DWordValue, Address)
/**< \brief Interrupt specific 32Bit ESC read access. */

#define HW_EscReadWord(WordValue, Address)       ((WordValue) = (((volatile UINT16 *)pEsc)[((Address)>>1)]))
/**< \brief 16Bit ESC read access. */

#define HW_EscReadWordIsr(WordValue, Address)    HW_EscReadWord(WordValue, Address)
/**< \brief Interrupt specific 16Bit ESC read access. */

#define HW_EscReadByte(ByteValue, Address)       ((ByteValue) = (((volatile UINT8 *)pEsc)[(Address)]))
/**< \brief 8Bit ESC read access. */

#define HW_EscReadByteIsr(ByteValue, Address)    HW_EscReadByte(ByteValue, Address)
/**< \brief Interrupt specific 8Bit ESC read access. */

#define HW_EscWriteDWord(DWordValue, Address)    ((((volatile UINT32 *)pEsc)[(Address>>2)]) = (DWordValue))
/**< \brief 16Bit ESC write access. */

#define HW_EscWriteDWordIsr(DWordValue, Address) HW_EscWriteWord(DWordValue, Address)
/**< \brief Interrupt specific 32Bit ESC write access. */

#define HW_EscWriteWord(WordValue, Address)      ((((volatile UINT16 *)pEsc)[((Address)>>1)]) = (WordValue))
/**< \brief 16Bit ESC write access. */

#define HW_EscWriteWordIsr(WordValue, Address)   HW_EscWriteWord(WordValue, Address)
/**< \brief Interrupt specific 16Bit ESC write access. */

#define HW_EscWriteByte(ByteValue, Address)      ((((volatile UINT8 *)pEsc)[(Address)]) = (ByteValue))
/**< \brief 8Bit ESC write access. */

#define HW_EscWriteByteIsr(ByteValue, Address)   HW_EscWriteByte(ByteValue, Address)
/**< \brief Interrupt specific 8Bit ESC write access. */

/**
 * @param_none
 *
 * @return_none
 *
 * \par<b>Description: </b><br>
 * This function stops and resets a timer for the EtherCAT. <br>
 *
 */
void mtb_xmc_ecat_timer_stop_reset(void);

/**
 * @param_none
 *
 * @return_none
 *
 * \par<b>Description: </b><br>
 * This function starts a timer for the EtherCAT. <br>
 *
 */
void mtb_xmc_ecat_timer_start(void);

/**
 * @param_none
 *
 * @return E_EEPROM_XMC4_STATUS_t
 *
 * \par<b>Description: </b><br>
 * This function initializes an EEPROM for the EtherCAT. <br>
 *
 */
E_EEPROM_XMC4_STATUS_t mtb_xmc_ecat_eeprom_init(void);

/**
 * @param_none
 *
 * @return Status 0, if the operation was successful.<BR>
 *         greater than 0, an error has occurred.
 *
 * \par<b>Description: </b><br>
 * This function initializes the EtherCAT using EEPROM, timer, ECAT slave hardware, and generic slave stack
 * initialization functions. <br>
 *
 */
uint32_t mtb_xmc_ecat_init(void);

/**
 * @param pData Pointer to local destination buffer.
 * @param Address EtherCAT Slave Controller address. Specifies the offset within the ESC memory area in Bytes.
 * @param Len Access size in Bytes
 * <BR>
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * 16Bit ESC read access. Reads from the EtherCAT Slave Controller. This function is used to access ESC
 * registers and the DPRAM area.
 *
 */
__STATIC_INLINE void HW_EscRead(MEM_ADDR * pData, UINT16 Address, UINT16 Len)
{
    memcpy(pData, &pEsc[Address], Len);
}


/**
 * @param pData Pointer to local destination buffer.
 * @param Address EtherCAT Slave Controller address. Specifies the offset within the ESC memory area in Bytes.
 * @param Len Access size in Bytes
 * <BR>
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * Interrupt specific 16Bit ESC read access. Reads from the EtherCAT Slave Controller. This function is used to
 * access ESC registers and the DPRAM area.
 *
 */
__STATIC_INLINE void HW_EscReadIsr(MEM_ADDR * pData, UINT16 Address, UINT16 Len)
{
    HW_EscRead(pData, Address, Len);
}


/**
 * @param pData Pointer to local destination mailbox buffer.
 * @param Address EtherCAT Slave Controller address. Specifies the offset within the ESC memory area in Bytes.
 * @param Len Access size in Bytes
 * <BR>
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * Reads data from the ESC and copies to slave mailbox memory. If the local mailbox memory is also located in the
 * application memory this function is equal to HW_EscRead.
 *
 */
__STATIC_INLINE void HW_EscReadMbxMem(MEM_ADDR * pData, UINT16 Address, UINT16 Len)
{
    HW_EscRead(pData, Address, Len);
}


/**
 * @param pData Pointer to local source buffer.
 * @param Address EtherCAT Slave Controller address. Specifies the offset within the ESC memory area in Bytes.
 * @param Len Access size in Bytes
 * <BR>
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * 16Bit ESC write access. Writes from the EtherCAT Slave Controller. This function is used to access ESC registers
 * and the DPRAM area.
 *
 */
__STATIC_INLINE void HW_EscWrite(MEM_ADDR * pData, UINT16 Address, UINT16 Len)
{
    memcpy(&pEsc[Address], pData, Len);
}


/**
 * @param pData Pointer to local source buffer.
 * @param Address EtherCAT Slave Controller address. Specifies the offset within the ESC memory area in Bytes.
 * @param Len Access size in Bytes
 * <BR>
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * Interrupt specific 16Bit ESC write access. Writes from the EtherCAT Slave Controller. This function is used to
 * access ESC registers and the DPRAM area.
 *
 */
__STATIC_INLINE void HW_EscWriteIsr(MEM_ADDR * pData, UINT16 Address, UINT16 Len)
{
    HW_EscWrite(pData, Address, Len);
}


/**
 * @param pData Pointer to local source mailbox buffer.
 * @param Address EtherCAT Slave Controller address. Specifies the offset within the ESC memory area in Bytes.
 * @param Len Access size in Bytes
 * <BR>
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * Writes data from the slave mailbox memory to ESC memory. If the local mailbox memory is also located in the
 * application memory this function is equal to HW_EscWrite.
 *
 */
__STATIC_INLINE void HW_EscWriteMbxMem(MEM_ADDR * pData, UINT16 Address, UINT16 Len)
{
    HW_EscWrite(pData, Address, Len);
}


/**
 * @param_none
 *
 * @return Content of register 0x220-0x221
 * <BR>
 *
 * \par<b>Description:</b><br>
 * Get the first two bytes of the AL Event register.
 *
 */
__STATIC_INLINE UINT16 HW_GetALEventRegister(void)
{
    return XMC_ECAT_GetALEventRegister();
}


/**
 * @param_none
 *
 * @return None
 * <BR>
 *
 * \par<b>Description:</b><br>
 * Get the first two bytes of the AL Event register from ISR. Special function for ESC access from interrupt
 * service routines if required, otherwise this function is defined as HW_GetALEventRegister.
 *
 */
__STATIC_INLINE UINT16 HW_GetALEventRegister_Isr(void)
{
    return HW_GetALEventRegister();
}


/**
 * @param_none
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * Function to enable the XMC EtherCAT slave controller interrupt.
 *
 */
void ENABLE_ESC_INT(void);

/**
 * @param_none
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * Function to disable the XMC EtherCAT slave controller interrupt.
 *
 */
void DISABLE_ESC_INT(void);

/**
 * @param_none
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * This routine is called from ECAT_APP on request of SSC stack once specific EtherCAT interrupts
 * (sync1/sync0) need to be enabled.
 *
 */
void mtb_xmc_enable_user_int(void);

/**
 * @param_none
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * This routine is called from ECAT_APP on request of SSC stack once specific EtherCAT interrupts
 * (sync1/sync0) need to be disabled.
 *
 */
void mtb_xmc_disable_user_int(void);

/**
 * @param_none
 *
 * @return Current timer value
 * <BR>
 *
 * \par<b>Description:</b><br>
 * Reads the hardware timer value in ticks. The timer ticks value (increments / ms) is defined in
 * ECAT_TIMER_INC_P_MS. This function is required, if no timer interrupt is supported (ECAT_TIMER_INT = 0)
 * and to calculate the bus cycle time.
 *
 */
UINT32 HW_GetTimer(void);

/**
 * @param_none
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * Clears the hardware timer value.
 *
 */
void HW_ClearTimer(void);


/**
 * @param_none
 *
 * @return 0 if initialization was successful,<BR>
 *         Else, if error has occurred while initialization
 * <BR>
 *
 * \par<b>Description:</b><br>
 * Initializes the host controller, process data interface (PDI) and allocates
 * resources which are required for hardware access.
 *
 */
UINT16 HW_Init(void);

/**
 * @param_none
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * Release allocated reserved resources.
 *
 */
void HW_Release(void);

/**
 * @param_none
 *
 * @return_none
 *
 * \par<b>Description:</b><br>
 * Resets the hardware. This function is only required if BOOTSTRAPMODE_SUPPORTED is set.
 *
 */
void HW_RestartTarget(void);

/**
 * @param wordaddr start word address within the EEPROM memory
 *
 * @return 0, if the operation was successful.<BR>
 *         greater than 0, an error has occurred.
 * <BR>
 *
 * \par<b>Description:</b><br>
 * Copies EEPROM data to the ESC EEPROM data register. This is only required if EEPROM_EMULATION is enabled and no
 * EEPROM content is created (CREATE_EEPROM_CONTENT== 0). This function shall copy EEPROM data to the ESC EEPROM data
 * register (0x508:0x50F/0x50B).The EEPROM data starting at the specified word address and the length specified with
 * "EEPROM_READ_SIZE". The data shall be copied to the ESC EEPROM buffer (ESC offset 0x508).
 *
 */
UINT16 HW_EepromRead(UINT32 wordaddr);

/**
 * @param wordaddr start word address within the EEPROM memory
 *
 * @return 0, if the operation was successful.<BR>
 *         greater than 0, an error has occurred.
 * <BR>
 *
 * \par<b>Description:</b><br>
 * Writes the EEPROM configuration information from ESC data register to EEPROM. This is only required if
 * EEPROM_EMULATION is enabled and no EEPROM content is created (CREATE_EEPROM_CONTENT== 0). This function shall copy
 * data from the ESC EEPROM data register (0x508:0x50F/0x50B).The EEPROM data starting at the specified word address and
 * the length specified with "EEPROM_WRITE_SIZE".
 *
 */
UINT16 HW_EepromWrite(UINT32 wordaddr);

/**
 * @param_none
 *
 * @return 0, if the operation was successful.<BR>
 *         greater than 0, an error has occurred.
 * <BR>
 *
 * \par<b>Description:</b><br>
 * Reloads the EEPROM configuration information to ESC data registers. This is only required if EEPROM_EMULATION is
 * enabled and no EEPROM content is created (CREATE_EEPROM_CONTENT== 0). This function shall copy the EEPROM reload
 * information to the ESC EEPROM data register (0x508:0x50F/0x50B).
 *
 */
UINT16 HW_EepromReload(void);

/** \} group_ecat_functions */

#ifdef __cplusplus
}
#endif


#endif /* MTB_XMC_ECAT_H */
