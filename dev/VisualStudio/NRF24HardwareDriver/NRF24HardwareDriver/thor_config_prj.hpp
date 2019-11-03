/********************************************************************************
*   File Name:
*       thor_config_prj.hpp
*       
*   Description:
*       Provides low level processor configuration options for a project using Thor
*   
*   2019 | Brandon Braun | brandonbraun653@gmail.com
********************************************************************************/

#pragma once
#ifndef THOR_CONFIGURATION_PROJECT_HPP
#define THOR_CONFIGURATION_PROJECT_HPP

/* C++ Includes */
#include <cstdint>
#include <cstdlib>

namespace Thor::Config
{
  namespace Clock
  {

    static constexpr size_t SystemClock_Hz = 140000000;
    static constexpr bool OverdriveEnabled = false;

  }

}

#endif /* !THOR_CONFIGURATION_PROJECT_HPP */