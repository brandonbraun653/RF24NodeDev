/********************************************************************************
*   File Name:
*       main.cpp
*       
*   Description:
*       Entry for hardware development and tests
*   
*   2020 | Brandon Braun | brandonbraun653@gmail.com
********************************************************************************/

#include <Chimera/chimera.hpp>
#include <Chimera/gpio.hpp>
#include <Chimera/threading.hpp>

static void led_blink( void *arg );


int main( void )
{
  using namespace Chimera::Threading;

  ChimeraInit();

  Thread blinkyThread;
  blinkyThread.initialize( led_blink, nullptr, Priority::LEVEL_3, 500, "blinky" );
  blinkyThread.start();

  startScheduler();
}


static void led_blink( void *arg )
{
  Chimera::GPIO::PinInit ledConfig;
  ledConfig.accessMode = Chimera::Hardware::AccessMode::BARE_METAL;
  ledConfig.drive      = Chimera::GPIO::Drive::OUTPUT_PUSH_PULL;
  ledConfig.pin        = 5;
  ledConfig.port       = Chimera::GPIO::Port::PORTA;
  ledConfig.pull       = Chimera::GPIO::Pull::NO_PULL;
  ledConfig.state      = Chimera::GPIO::State::LOW;
  ledConfig.validity   = true;

  Chimera::GPIO::GPIOClass led;
  led.init( ledConfig );
  led.setState( Chimera::GPIO::State::LOW );


  while ( 1 )
  {
    led.setState( Chimera::GPIO::State::HIGH );
    Chimera::delayMilliseconds( 100 );
    led.setState( Chimera::GPIO::State::LOW );
    Chimera::delayMilliseconds( 100 );
  }
}
