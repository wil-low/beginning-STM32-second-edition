
/* Define to prevent recursive inclusion
 * ----------------------------------------*/
#ifndef _TM1638_PLATFORM_H_
#define _TM1638_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes
 * ---------------------------------------------------------------------*/
#include "TM1638.h"
#include <stdint.h>

/* Functionality Options
 * --------------------------------------------------------*/

/**
 * @brief  Specify IO Pins of AVR connected to TM1638
 */
#define TM1638_CLK_GPIO GPIOB
#define TM1638_CLK_PIN GPIO6
#define TM1638_DIO_GPIO GPIOB
#define TM1638_DIO_PIN GPIO7
#define TM1638_STB_GPIO GPIOB
#define TM1638_STB_PIN GPIO8

/**
 ==================================================================================
                               ##### Functions #####
 ==================================================================================
 */

/**
 * @brief  Initialize platform device to communicate TM1638.
 * @param  Handler: Pointer to handler
 * @retval None
 */
void TM1638_Platform_Init(TM1638_Handler_t *Handler);

#ifdef __cplusplus
}
#endif

#endif //! _TM1638_PLATFORM_H_
