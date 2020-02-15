/********************************************************************************
 *   File Name:
 *       main.cpp
 *
 *   Description:
 *       Entry for hardware development and tests
 *
 *   2020 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

/* C++ Includes */
#include <string>

/* Chimera Includes */
#include <Chimera/gpio>
#include <Chimera/spi>
#include <Chimera/system>
#include <Chimera/thread>
#include <Chimera/watchdog>

/* Thor Includes */
#include <Thor/drivers/gpio.hpp>

/* RF24 Includes */
#include <RF24Node/common/conversion.hpp>
#include <RF24Node/common/definitions.hpp>
#include <RF24Node/endpoint/endpoint.hpp>
#include <RF24Node/endpoint/types.hpp>
#include <RF24Node/physical/simulator/pipe.hpp>
#include <RF24Node/physical/simulator/shockburst.hpp>
#include <RF24Node/physical/simulator/shockburst_types.hpp>

/* Logger Includes */
#include <uLog/ulog.hpp>
#include <uLog/sinks/sink_cout.hpp>
#include <uLog/sinks/sink_vgdb_semihosting.hpp>

/* Visual GDB Includes */
#include "FastSemihosting.h"


using namespace Chimera::Threading;

static void background_thread( void *arg );
static void startup_blinky_sequence( const Chimera::GPIO::GPIO_sPtr &led );
static void initialize_rf24_config();
static void MasterNodeThread( void *arg );
static void SlaveNodeThread( void *arg );


static RF24::Endpoint::Config cfg;

int main( void )
{
  /*------------------------------------------------
  Boot up the device hardware
  ------------------------------------------------*/
  ChimeraInit();

  /*------------------------------------------------
  Initialize the logger system
  ------------------------------------------------*/
  uLog::initialize();

  uLog::SinkHandle rootSink = std::make_shared<uLog::VGDBSemihostingSink>();
  rootSink->setLogLevel( uLog::Level::LVL_DEBUG );
  rootSink->setName( "Root" );
  rootSink->enable();

  uLog::registerSink( rootSink );
  uLog::setGlobalLogLevel( uLog::Level::LVL_DEBUG );
  uLog::setRootSink( rootSink );

  rootSink->flog( uLog::Level::LVL_INFO, "Boot up the world!\n" );

  /*------------------------------------------------
  Create the system threads
  ------------------------------------------------*/
  Thread blinkyThread;
  blinkyThread.initialize( background_thread, nullptr, Priority::LEVEL_3, 500, "blinky" );
  blinkyThread.start();

  #if defined( RF24_DEVICE_1 )
  Thread masterThread;
  masterThread.initialize( MasterNodeThread, nullptr, Priority::LEVEL_3, 2000, "master" );
  masterThread.start();
  #endif 

  #if defined( RF24_DEVICE_2 )
  Thread slaveThread;
  slaveThread.initialize( SlaveNodeThread, nullptr, Priority::LEVEL_3, 2000, "slave" );
  slaveThread.start();
  #endif 

  startScheduler();
}

void startup_blinky_sequence( const Chimera::GPIO::GPIO_sPtr &led )
{
  for ( uint8_t i = 0; i < 5; i++ )
  {
    led->setState( Chimera::GPIO::State::HIGH, 100 );
    Chimera::delayMilliseconds( 65 );
    led->setState( Chimera::GPIO::State::LOW, 100 );
    Chimera::delayMilliseconds( 25 );
  }

  Chimera::delayMilliseconds( 350 );
}

void initialize_rf24_config()
{
  /*------------------------------------------------
  Configure the NRF24 radio
  ------------------------------------------------*/
  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;

  /*------------------------------------------------
  GPIO Initialization
  ------------------------------------------------*/
  cfg.physical.spiConfig.validity   = true;
  cfg.physical.spiConfig.externalCS = true;

  cfg.physical.spiConfig.SCKInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.physical.spiConfig.SCKInit.alternate  = Thor::Driver::GPIO::AF6_SPI3;
  cfg.physical.spiConfig.SCKInit.drive      = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.physical.spiConfig.SCKInit.pin        = 10;
  cfg.physical.spiConfig.SCKInit.port       = Chimera::GPIO::Port::PORTC;
  cfg.physical.spiConfig.SCKInit.pull       = Chimera::GPIO::Pull::NO_PULL;
  cfg.physical.spiConfig.SCKInit.validity   = true;

  cfg.physical.spiConfig.MISOInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.physical.spiConfig.MISOInit.alternate  = Thor::Driver::GPIO::AF6_SPI3;
  cfg.physical.spiConfig.MISOInit.drive      = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.physical.spiConfig.MISOInit.pin        = 11;
  cfg.physical.spiConfig.MISOInit.port       = Chimera::GPIO::Port::PORTC;
  cfg.physical.spiConfig.MISOInit.pull       = Chimera::GPIO::Pull::NO_PULL;
  cfg.physical.spiConfig.MISOInit.validity   = true;

  cfg.physical.spiConfig.MOSIInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.physical.spiConfig.MOSIInit.alternate  = Thor::Driver::GPIO::AF6_SPI3;
  cfg.physical.spiConfig.MOSIInit.drive      = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.physical.spiConfig.MOSIInit.pin        = 12;
  cfg.physical.spiConfig.MOSIInit.port       = Chimera::GPIO::Port::PORTC;
  cfg.physical.spiConfig.MOSIInit.pull       = Chimera::GPIO::Pull::NO_PULL;
  cfg.physical.spiConfig.MOSIInit.validity   = true;

  cfg.physical.spiConfig.CSInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.physical.spiConfig.CSInit.alternate  = Thor::Driver::GPIO::AF_NONE;
  cfg.physical.spiConfig.CSInit.drive      = Chimera::GPIO::Drive::OUTPUT_PUSH_PULL;
  cfg.physical.spiConfig.CSInit.pin        = 2;
  cfg.physical.spiConfig.CSInit.port       = Chimera::GPIO::Port::PORTC;
  cfg.physical.spiConfig.CSInit.pull       = Chimera::GPIO::Pull::NO_PULL;
  cfg.physical.spiConfig.CSInit.validity   = true;

  cfg.physical.chipEnableConfig.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.physical.chipEnableConfig.alternate  = Thor::Driver::GPIO::AF_NONE;
  cfg.physical.chipEnableConfig.drive      = Chimera::GPIO::Drive::OUTPUT_PUSH_PULL;
  cfg.physical.chipEnableConfig.pin        = 3;
  cfg.physical.chipEnableConfig.port       = Chimera::GPIO::Port::PORTC;
  cfg.physical.chipEnableConfig.pull       = Chimera::GPIO::Pull::NO_PULL;
  cfg.physical.chipEnableConfig.validity   = true;

  /*------------------------------------------------
  SPI Parameter Initialization
  ------------------------------------------------*/
  cfg.physical.spiConfig.HWInit.validity    = true;
  cfg.physical.spiConfig.HWInit.bitOrder    = Chimera::SPI::BitOrder::MSB_FIRST;
  cfg.physical.spiConfig.HWInit.clockFreq   = 8000000;
  cfg.physical.spiConfig.HWInit.clockMode   = Chimera::SPI::ClockMode::MODE0;
  cfg.physical.spiConfig.HWInit.controlMode = Chimera::SPI::ControlMode::MASTER;
  cfg.physical.spiConfig.HWInit.csMode      = Chimera::SPI::CSMode::MANUAL;
  cfg.physical.spiConfig.HWInit.dataSize    = Chimera::SPI::DataSize::SZ_8BIT;
  cfg.physical.spiConfig.HWInit.hwChannel   = 3;
  cfg.physical.spiConfig.HWInit.txfrMode    = Chimera::SPI::TransferMode::INTERRUPT;
}

void background_thread( void *arguments )
{
  /*------------------------------------------------
  Initialize the LED gpio
  ------------------------------------------------*/
  Chimera::GPIO::PinInit ledInit;
  ledInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  ledInit.drive      = Chimera::GPIO::Drive::OUTPUT_PUSH_PULL;
  ledInit.port       = Chimera::GPIO::Port::PORTA;
  ledInit.pull       = Chimera::GPIO::Pull::NO_PULL;
  ledInit.pin        = 5;

  auto led = Chimera::GPIO::create_shared_ptr();
  led->init( ledInit, 100 );
  led->setState( Chimera::GPIO::State::HIGH, 100 );

  startup_blinky_sequence( led );

  auto watchdog = Chimera::Watchdog::create_shared_ptr();
  watchdog->initialize( 500, 100 );
  watchdog->start();

  while ( 1 )
  {
    watchdog->kick();
    led->setState( Chimera::GPIO::State::HIGH, 100 );
    Chimera::delayMilliseconds( 150 );
    led->setState( Chimera::GPIO::State::LOW, 100 );
    Chimera::delayMilliseconds( 150 );
  }
}

#if defined( RF24_DEVICE_1 )
void MasterNodeThread( void *arg )
{
  /*------------------------------------------------
  Configure the log sink for this thread
  ------------------------------------------------*/
  uLog::SinkHandle masterSink = std::make_shared<uLog::VGDBSemihostingSink>();
  masterSink->setLogLevel( uLog::Level::LVL_DEBUG );
  masterSink->setName( "Master" );
  masterSink->enable();
  uLog::registerSink( masterSink );

  /*------------------------------------------------
  Initialize the master config
  ------------------------------------------------*/
  initialize_rf24_config();

  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = RF24::RootNode0;
  cfg.network.parentStaticAddress = RF24::Network::RSVD_ADDR_INVALID;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  /*------------------------------------------------
  Create the radio
  ------------------------------------------------*/
  RF24::Endpoint::Device master;
  master.attachLogger( masterSink );
  master.configure( cfg );
  master.setName( "Master" );

  while ( true )
  {
    master.doAsyncProcessing();
    Chimera::delayMilliseconds( 25 );
  }
}
#endif /* RF24_DEVICE_1 */

#if defined( RF24_DEVICE_2 )
void SlaveNodeThread( void *arg )
{
  uLog::SinkHandle slaveSink = std::make_shared<uLog::VGDBSemihostingSink>();
  slaveSink->setLogLevel( uLog::Level::LVL_DEBUG );
  slaveSink->setName( "Slave" );
  slaveSink->enable();
  uLog::registerSink( slaveSink );

  /*------------------------------------------------
  Initialize the slave config
  ------------------------------------------------*/
  initialize_rf24_config();
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = 0001;
  cfg.network.parentStaticAddress = RF24::RootNode0;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  /*------------------------------------------------
  Create the radio
  ------------------------------------------------*/
  RF24::Endpoint::Device slave;
  slave.attachLogger( slaveSink );
  slave.configure( cfg );
  slave.setName( "Slave" );

  if ( slave.connect( 1000 ) )
  {
    slaveSink->flog( uLog::Level::LVL_INFO, "%d-APP: Holy crap it worked?!\n", Chimera::millis() );
  }
  else
  {
    slaveSink->flog( uLog::Level::LVL_INFO, "%d-APP: Did not connect for some reason\n", Chimera::millis() );
  }

  while ( true )
  {
    slave.doAsyncProcessing();
    Chimera::delayMilliseconds( 25 );
  }
}
#endif /* RF24_DEVICE_2 */
