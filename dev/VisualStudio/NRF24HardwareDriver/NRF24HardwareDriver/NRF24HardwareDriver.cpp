/********************************************************************************
 *   File Name:
 *    main.cpp
 *
 *   Description:
 *    SPI development project entry
 *
 *   2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

/* Chimera Includes */
#include <Chimera/gpio.hpp>
#include <Chimera/spi.hpp>
#include <Chimera/system.hpp>
#include <Chimera/threading.hpp>
#include <Chimera/watchdog.hpp>

/* Radio Includes */
#include <RF24Node/hardware/driver.hpp>

void startup_sequence( Chimera::GPIO::GPIOClass *const led );
void background_thread( void *arguments );
void radio_thread( void *arguments );

int main( void )
{
  Chimera::System::initialize();

  Chimera::Threading::addThread( background_thread, "background", 1000, nullptr, 3, nullptr );
  Chimera::Threading::addThread( radio_thread, "radio", 1000, nullptr, 3, nullptr );
  Chimera::Threading::startScheduler();
  return 0;
}

void startup_sequence( Chimera::GPIO::GPIOClass *const led )
{
  for ( uint8_t i = 0; i < 5; i++ )
  {
    led->setState( Chimera::GPIO::State::HIGH );
    Chimera::delayMilliseconds( 65 );
    led->setState( Chimera::GPIO::State::LOW );
    Chimera::delayMilliseconds( 25 );
  }

  Chimera::delayMilliseconds( 350 );
}

void background_thread( void *arguments )
{
  Chimera::Threading::signalSetupComplete();

  Chimera::GPIO::PinInit ledInit;
  ledInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  ledInit.drive      = Chimera::GPIO::Drive::OUTPUT_PUSH_PULL;
  ledInit.port       = Chimera::GPIO::Port::PORTA;
  ledInit.pull       = Chimera::GPIO::Pull::NO_PULL;
  ledInit.pin        = 5;

  auto led = Chimera::GPIO::GPIOClass();
  led.init( ledInit );
  led.setState( Chimera::GPIO::State::HIGH );

  startup_sequence( &led );

  auto watchdog = Chimera::Watchdog::WatchdogClass();
  watchdog.initialize( 500, 100 );
  watchdog.start();

  while ( 1 )
  {
    watchdog.kick();
    led.setState( Chimera::GPIO::State::HIGH );
    Chimera::delayMilliseconds( 150 );
    led.setState( Chimera::GPIO::State::LOW );
    Chimera::delayMilliseconds( 150 );
  }
}

void radio_thread( void *arguments )
{
  Chimera::Threading::signalSetupComplete();

  Chimera::SPI::DriverConfig cfg;
  Chimera::SPI::SPIClass_sPtr spi = std::make_shared<Chimera::SPI::SPIClass>();

  /*------------------------------------------------
  GPIO Initialization
  ------------------------------------------------*/
  cfg.CSInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.CSInit.alternate  = Thor::Driver::GPIO::AF_NONE;
  cfg.CSInit.drive      = Chimera::GPIO::Drive::OUTPUT_PUSH_PULL;
  cfg.CSInit.pin        = 2;
  cfg.CSInit.port       = Chimera::GPIO::Port::PORTD;
  cfg.CSInit.pull       = Chimera::GPIO::Pull::NO_PULL;

  cfg.SCKInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.SCKInit.alternate  = Thor::Driver::GPIO::AF6_SPI3;
  cfg.SCKInit.drive      = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.SCKInit.pin        = 10;
  cfg.SCKInit.port       = Chimera::GPIO::Port::PORTC;
  cfg.SCKInit.pull       = Chimera::GPIO::Pull::NO_PULL;

  cfg.MISOInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.MISOInit.alternate  = Thor::Driver::GPIO::AF6_SPI3;
  cfg.MISOInit.drive      = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.MISOInit.pin        = 11;
  cfg.MISOInit.port       = Chimera::GPIO::Port::PORTC;
  cfg.MISOInit.pull       = Chimera::GPIO::Pull::NO_PULL;

  cfg.MOSIInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.MOSIInit.alternate  = Thor::Driver::GPIO::AF6_SPI3;
  cfg.MOSIInit.drive      = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.MOSIInit.pin        = 12;
  cfg.MOSIInit.port       = Chimera::GPIO::Port::PORTC;
  cfg.MOSIInit.pull       = Chimera::GPIO::Pull::NO_PULL;

  /*------------------------------------------------
  SPI Parameter Initialization
  ------------------------------------------------*/
  cfg.HWInit.bitOrder    = Chimera::SPI::BitOrder::MSB_FIRST;
  cfg.HWInit.clockFreq   = 8000000;
  cfg.HWInit.clockMode   = Chimera::SPI::ClockMode::MODE0;
  cfg.HWInit.controlMode = Chimera::SPI::ControlMode::MASTER;
  cfg.HWInit.csMode      = Chimera::SPI::CSMode::AUTO_AFTER_TRANSFER;
  cfg.HWInit.dataSize    = Chimera::SPI::DataSize::SZ_8BIT;
  cfg.HWInit.hwChannel   = 3;
  cfg.HWInit.txfrMode    = Chimera::SPI::TransferMode::INTERRUPT;
  cfg.validity           = true;

  /*------------------------------------------------
  Radio Initialization
  ------------------------------------------------*/
  auto result = Chimera::CommonStatusCodes::OK;
  auto radio = RF24::Hardware::Driver();

  result |= radio.attachSPI( spi, cfg );
  result |= radio.initialize();

  if ( result != Chimera::CommonStatusCodes::OK )
  {
    Chimera::Watchdog::invokeTimeout();
  }

  while ( 1 )
  {
    Chimera::delayMilliseconds( 100 );
  }
}

