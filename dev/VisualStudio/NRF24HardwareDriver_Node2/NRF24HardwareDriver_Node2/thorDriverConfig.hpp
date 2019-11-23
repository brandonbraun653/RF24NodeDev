/********************************************************************************
 *   File Name:
 *    thorDriverConfig.hpp
 *
 *   Description:
 *    Project specific Thor driver enable/disable macros
 *
 *   2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

#pragma once
#ifndef THOR_DRIVER_CONFIGURATION_HPP
#define THOR_DRIVER_CONFIGURATION_HPP

/*------------------------------------------------
Enable/Disable peripheral drivers here to save memory
------------------------------------------------*/
#define THOR_DRIVER_DMA         1
#define THOR_DRIVER_FLASH       1
#define THOR_DRIVER_GPIO        1
#define THOR_DRIVER_IWDG        1
#define THOR_DRIVER_NVIC        1
#define THOR_DRIVER_PWR         1
#define THOR_DRIVER_RCC         1
#define THOR_DRIVER_SPI         1
#define THOR_DRIVER_UART        1
#define THOR_DRIVER_USART       1
#define THOR_DRIVER_WWDG        1

#endif /* !THOR_DRIVER_CONFIGURATION_HPP */
