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
#include <Chimera/gpio.hpp>
#include <Chimera/spi.hpp>
#include <Chimera/system.hpp>
#include <Chimera/threading.hpp>
#include <Chimera/watchdog.hpp>

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


static void background_thread( void *arg );
static void startup_sequence( Chimera::GPIO::GPIOClass *const led );
static void MasterNodeThread( void *arg );
static void SlaveNodeThread( void *arg );

int main( void )
{
  using namespace Chimera::Threading;

  ChimeraInit();

  uLog::initialize();

  uLog::SinkHandle rootSink = std::make_shared<uLog::VGDBSemihostingSink>();
  rootSink->setLogLevel( uLog::Level::LVL_DEBUG );
  rootSink->setName( "Root" );
  rootSink->enable();

  uLog::registerSink( rootSink );
  uLog::setGlobalLogLevel( uLog::Level::LVL_DEBUG );
  uLog::setRootSink( rootSink );

  Thread blinkyThread;
  blinkyThread.initialize( background_thread, nullptr, Priority::LEVEL_3, 500, "blinky" );
  blinkyThread.start();

  Thread masterThread;
  masterThread.initialize( MasterNodeThread, nullptr, Priority::LEVEL_3, 2000, "master" );
  masterThread.start();

  Thread slaveThread;
  slaveThread.initialize( SlaveNodeThread, nullptr, Priority::LEVEL_3, 2000, "slave" );
  slaveThread.start();


  startScheduler();
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
  /*------------------------------------------------
  Initialize the LED gpio
  ------------------------------------------------*/
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

void MasterNodeThread( void *arg )
{
  uLog::SinkHandle masterSink = std::make_shared<uLog::VGDBSemihostingSink>();
  masterSink->setLogLevel( uLog::Level::LVL_DEBUG );
  masterSink->setName( "Master" );
  masterSink->enable();
  uLog::registerSink( masterSink );

  RF24::Endpoint::Device master;
  RF24::Endpoint::Config cfg;
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = RF24::RootNode0;
  cfg.network.parentStaticAddress = RF24::Network::RSVD_ADDR_INVALID;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;

  master.attachLogger( masterSink );
  master.configure( cfg );
  master.setName( "Master" );

  while ( true )
  {
    master.doAsyncProcessing();
    Chimera::delayMilliseconds( 25 );
  }
}

void SlaveNodeThread( void *arg )
{
  uLog::SinkHandle slaveSink = std::make_shared<uLog::VGDBSemihostingSink>();
  slaveSink->setLogLevel( uLog::Level::LVL_DEBUG );
  slaveSink->setName( "Slave" );
  slaveSink->enable();
  uLog::registerSink( slaveSink );

  RF24::Endpoint::Device slave;
  RF24::Endpoint::Config cfg;
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = 0001;
  cfg.network.parentStaticAddress = RF24::RootNode0;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;

  slave.attachLogger( slaveSink );
  slave.configure( cfg );
  slave.setName( "Slave" );

  if ( slave.connect( 1000 ) )
  {
    slaveSink->flog( uLog::Level::LVL_INFO, "Holy crap it worked?!\n" );
  }
  else
  {
    slaveSink->flog( uLog::Level::LVL_INFO, "Did not connect for some reason\n" );
  }

  while ( true )
  {
    slave.doAsyncProcessing();
    Chimera::delayMilliseconds( 25 );
  }
}
