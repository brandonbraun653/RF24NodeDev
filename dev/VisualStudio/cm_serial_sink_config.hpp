/********************************************************************************
 *   File Name:
 *    serial_sink_config.hpp
 *
 *   Description:
 *    Configuration file for defaulting the serial sink behavior
 *
 *   2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

#pragma once
#ifndef CM_SERIAL_SINK_CONFIG_HPP
#define CM_SERIAL_SINK_CONFIG_HPP

/* C++ Includes */
#include <cstdlib>

#include <Chimera/types/serial_types.hpp>

namespace Chimera::Modules::uLog
{
  /*------------------------------------------------
  Configuration options for the default serial sink
  ------------------------------------------------*/
  static constexpr size_t SerialChannel                        = 2u;
  static constexpr Chimera::Serial::BaudRate SerialBaud        = Chimera::Serial::BaudRate::SERIAL_BAUD_115200;
  static constexpr Chimera::Serial::CharWid SerialCharWid      = Chimera::Serial::CharWid::CW_8BIT;
  static constexpr Chimera::Serial::FlowControl SerialFlowCtrl = Chimera::Serial::FlowControl::FCTRL_NONE;
  static constexpr Chimera::Serial::Parity SerialParity        = Chimera::Serial::Parity::PAR_NONE;
  static constexpr Chimera::Serial::StopBits SerialStopBits    = Chimera::Serial::StopBits::SBITS_ONE;

  /*------------------------------------------------
  This must be defined project side because it's so
  hardware specific. There is no generalized pin config.
  ------------------------------------------------*/
  extern Chimera::Serial::IOPins SerialPins;
}

#endif  /* CM_SERIAL_SINK_CONFIG_HPP */
