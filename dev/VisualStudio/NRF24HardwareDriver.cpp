/********************************************************************************
 *   File Name:
 *    main.cpp
 *
 *   Description:
 *    SPI development project entry
 *
 *   2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

/* C++ Includes */
#include <string>

/* Chimera Includes */
#include <Chimera/gpio.hpp>
#include <Chimera/spi.hpp>
#include <Chimera/system.hpp>
#include <Chimera/threading.hpp>
#include <Chimera/watchdog.hpp>

/* Radio Includes */
#include <RF24Node/hardware/driver.hpp>
#include <RF24Node/physical/physical.hpp>
#include <RF24Node/network/network.hpp>

/* FRAM Includes */
#include <mb85rs64v.hpp>

/* Logging Includes */
#include <Chimera/modules/ulog/serial_sink.hpp>
#include <uLog/ulog.hpp>

void startup_sequence( Chimera::GPIO::GPIOClass *const led );
void background_thread( void *arguments );
void radio_thread( void *arguments );
void logSink_thread( void *arguments );

Chimera::SPI::SPIClass_sPtr spi;
Chimera::SPI::DriverConfig cfg;

static constexpr uint16_t MASTER_NODE_ADDRESS = 0U;
static constexpr uint16_t CHILD_NODE_ADDRESS  = 1U;

int main( void )
{
  Chimera::System::initialize();

  spi = nullptr;
  memset( &cfg, 0, sizeof( cfg ) );

  Chimera::Threading::addThread( background_thread, "background", 1000, nullptr, 3, nullptr );
  //Chimera::Threading::addThread( radio_thread, "radio", 1000, nullptr, 3, nullptr );
  Chimera::Threading::addThread( logSink_thread, "logging", 1000, nullptr, 3, nullptr );
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

  /*------------------------------------------------
  Allocate the spi driver memory
  ------------------------------------------------*/
  spi = std::make_shared<Chimera::SPI::SPIClass>();

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

void logSink_thread( void *arguments )
{
  Chimera::Threading::signalSetupComplete();

  uLog::SinkType sink = std::make_shared<Chimera::Modules::uLog::SerialSink>();

  sink->setLogLevel( uLog::LogLevelType::LOG_LEVEL_DEBUG );

  auto sinkHandle = uLog::registerSink( sink );
  uLog::enableSink( sinkHandle );
  uLog::setGlobalLogLevel( uLog::LogLevelType::LOG_LEVEL_DEBUG );

  std::string test;
  std::array<char, 50> buffer;
  buffer.fill( 0 );

  while ( 1 ) 
  {
    snprintf( buffer.data(), buffer.size(), "Current tick: %d ms\r\n", Chimera::millis() );

    uLog::log( uLog::LogLevelType::LOG_LEVEL_DEBUG, buffer.data(), strlen( buffer.data() ) );
    uLog::flog( uLog::LogLevelType::LOG_LEVEL_DEBUG, "Nom nom nom\r\n" );

    Chimera::delayMilliseconds( 500 );
  }
}

void radio_thread( void *arguments )
{
  Chimera::Threading::signalSetupComplete();

  while ( !spi )
  {
    Chimera::delayMilliseconds( 100 );
  }

  if ( spi->lock( 100 ) != Chimera::CommonStatusCodes::OK )
  {
    Chimera::Watchdog::invokeTimeout();
  }

  /*------------------------------------------------
  GPIO Initialization
  ------------------------------------------------*/
  cfg.validity   = true;
  cfg.externalCS = true;

  cfg.SCKInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.SCKInit.alternate  = Thor::Driver::GPIO::AF6_SPI3;
  cfg.SCKInit.drive      = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.SCKInit.pin        = 10;
  cfg.SCKInit.port       = Chimera::GPIO::Port::PORTC;
  cfg.SCKInit.pull       = Chimera::GPIO::Pull::NO_PULL;
  cfg.SCKInit.validity   = true;

  cfg.MISOInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.MISOInit.alternate  = Thor::Driver::GPIO::AF6_SPI3;
  cfg.MISOInit.drive      = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.MISOInit.pin        = 11;
  cfg.MISOInit.port       = Chimera::GPIO::Port::PORTC;
  cfg.MISOInit.pull       = Chimera::GPIO::Pull::NO_PULL;
  cfg.MISOInit.validity   = true;

  cfg.MOSIInit.accessMode = Chimera::Hardware::AccessMode::THREADED;
  cfg.MOSIInit.alternate  = Thor::Driver::GPIO::AF6_SPI3;
  cfg.MOSIInit.drive      = Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL;
  cfg.MOSIInit.pin        = 12;
  cfg.MOSIInit.port       = Chimera::GPIO::Port::PORTC;
  cfg.MOSIInit.pull       = Chimera::GPIO::Pull::NO_PULL;
  cfg.MOSIInit.validity   = true;

  /*------------------------------------------------
  SPI Parameter Initialization
  ------------------------------------------------*/
  cfg.HWInit.bitOrder           = Chimera::SPI::BitOrder::MSB_FIRST;
  cfg.HWInit.clockFreq          = 8000000;
  cfg.HWInit.clockMode          = Chimera::SPI::ClockMode::MODE0;
  cfg.HWInit.controlMode        = Chimera::SPI::ControlMode::MASTER;
  cfg.HWInit.csMode             = Chimera::SPI::CSMode::MANUAL;
  cfg.HWInit.dataSize           = Chimera::SPI::DataSize::SZ_8BIT;
  cfg.HWInit.hwChannel          = 3;
  cfg.HWInit.txfrMode           = Chimera::SPI::TransferMode::INTERRUPT;

  spi->init( cfg );
  spi->unlock();

  /*------------------------------------------------
  Radio Initialization
  ------------------------------------------------*/
  auto result = Chimera::CommonStatusCodes::OK;
  auto radio  = std::make_shared<RF24::Hardware::Driver>();

  Chimera::GPIO::PinInit CEPinConfig;
  CEPinConfig.accessMode = Chimera::Hardware::AccessMode::THREADED;
  CEPinConfig.alternate  = Thor::Driver::GPIO::AF_NONE;
  CEPinConfig.drive      = Chimera::GPIO::Drive::OUTPUT_PUSH_PULL;
  CEPinConfig.pin        = 3;
  CEPinConfig.port       = Chimera::GPIO::Port::PORTC;
  CEPinConfig.pull       = Chimera::GPIO::Pull::NO_PULL;
  CEPinConfig.validity   = true;

  Chimera::GPIO::PinInit CSPinConfig;
  CSPinConfig.accessMode = Chimera::Hardware::AccessMode::THREADED;
  CSPinConfig.alternate  = Thor::Driver::GPIO::AF_NONE;
  CSPinConfig.drive      = Chimera::GPIO::Drive::OUTPUT_PUSH_PULL;
  CSPinConfig.pin        = 2;
  CSPinConfig.port       = Chimera::GPIO::Port::PORTC;
  CSPinConfig.pull       = Chimera::GPIO::Pull::NO_PULL;
  CSPinConfig.validity   = true;

  result |= radio->attachSPI( spi );
  result |= radio->initialize( CEPinConfig, CSPinConfig );

  /*------------------------------------------------
  Initialize the physical layer
  ------------------------------------------------*/
  auto physicalLayer = std::make_shared<RF24::Physical::Driver>();
  physicalLayer->attachHWDriver( radio );

  /*------------------------------------------------
  Initialize the network layer
  ------------------------------------------------*/
  auto networkLayer  = std::make_shared<RF24::Network::Network>();
  networkLayer->attachPhysicalDriver( physicalLayer );

  uint16_t nodeAddress = 0;
  uint32_t testPayload = 0xDEADBEEF;
  
  RF24::Network::Header testHeader;

#if defined( RF24_TEST_CHILD )
  nodeAddress = CHILD_NODE_ADDRESS;

  
  size_t packetTransmitTimeout = 5000;
  size_t packetTransmitStart   = Chimera::millis();

  testHeader.data.dstNode = MASTER_NODE_ADDRESS;
  testHeader.data.srcNode = CHILD_NODE_ADDRESS;
  testHeader.data.msgType = RF24::Network::MSG_TX_NORMAL;

#elif defined( RF24_TEST_PARENT )
  nodeAddress = MASTER_NODE_ADDRESS;

  std::array<uint8_t, RF24::Hardware::MAX_PAYLOAD_WIDTH> msgSink;
  msgSink.fill( 0 );
  uint16_t copiedBytes = 0u;
  uint32_t copiedPayload = 0u;

#endif

  networkLayer->begin( 90, nodeAddress );



  while ( 1 )
  {
    networkLayer->update();

#if defined( RF24_TEST_CHILD )
    // Currently this tries to transmit a payload periodically to the parent
    if ( ( Chimera::millis() - packetTransmitStart ) >= packetTransmitTimeout ) 
    {
      packetTransmitStart = Chimera::millis();

      networkLayer->write( testHeader, &testPayload, sizeof( testPayload ), MASTER_NODE_ADDRESS );
    }


#elif defined( RF24_TEST_PARENT )
    if ( networkLayer->available() ) 
    {
      msgSink.fill( 0 );
      copiedBytes = networkLayer->read( testHeader, msgSink.data(), msgSink.size() );

      if ( copiedBytes == sizeof( uint32_t ) )
      {
        memcpy( &copiedPayload, msgSink.data(), copiedBytes );
        printf( "\tReceived: 0x%04X\r\n\r\n", copiedPayload );
      }
    }
#endif

    Chimera::delayMilliseconds( 25 );
  }
}


namespace Chimera::Modules::uLog
{
  Chimera::Serial::IOPins SerialPins = {
    /* RX Pin */
    { Chimera::GPIO::Pull::NO_PULL, Chimera::GPIO::Port::PORTC, Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL,
      Chimera::GPIO::State::HI, 7, Thor::Driver::GPIO::AF8_USART6, Chimera::Hardware::AccessMode::THREADED, true },

    /* TX Pin */
    { Chimera::GPIO::Pull::NO_PULL, Chimera::GPIO::Port::PORTC, Chimera::GPIO::Drive::ALTERNATE_PUSH_PULL,
      Chimera::GPIO::State::HI, 6, Thor::Driver::GPIO::AF8_USART6, Chimera::Hardware::AccessMode::THREADED, true
    } 
  };
}