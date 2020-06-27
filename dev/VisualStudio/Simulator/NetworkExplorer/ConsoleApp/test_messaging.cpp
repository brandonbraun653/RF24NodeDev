/* Chimera Includes */
#include <Chimera/common>

/* RF24 Includes */
#include <RF24Node/common>
#include <RF24Node/endpoint>
#include <RF24Node/src/physical/simulator/sim_physical.hpp>
#include <RF24Node/src/physical/simulator/pipe.hpp>
#include <RF24Node/src/physical/simulator/shockburst.hpp>
#include <RF24Node/src/physical/simulator/shockburst_types.hpp>

/* Logger Includes */
#include <uLog/ulog.hpp>
#include <uLog/sinks/sink_cout.hpp>

/* Windows Includes */
#include <windows.h>
#include <processthreadsapi.h>

static constexpr size_t BootDelay        = 500;
static constexpr size_t ConnectTimeout   = 10000;
static constexpr size_t AsyncUpdateRate  = 50;
static constexpr size_t ThreadUpdateRate = 15;
static constexpr size_t SayHelloRate     = 5000;

struct MessageType
{
  uint8_t number;
  uint8_t crc;
};

struct EndpointInitializer
{
  RF24::Endpoint::Interface_sPtr device;
  bool initialized;

  void ( *idleThreadFunction )( EndpointInitializer * );

  std::string deviceName;
  RF24::LogicalAddress deviceAddress;
  RF24::LogicalAddress parentAddress;
};

static std::vector<EndpointInitializer> SystemNodes;
static std::vector<std::thread> SystemThreads;

static void RootNodeThread( EndpointInitializer *init );
static void ChildNodeThread_001( EndpointInitializer *init );
static void ChildNodeThread_002( EndpointInitializer *init );
static void ChildNodeThread_012( EndpointInitializer *init );
static void ChildNodeThread_003( EndpointInitializer *init );
static void ChildNodeThread_013( EndpointInitializer *init );
static void ChildNodeThread_0113( EndpointInitializer *init );
static void ChildNodeThread_02113( EndpointInitializer *init );
static void ChildNodeThread_042113( EndpointInitializer *init );

void RunMessagingTests()
{
  /*------------------------------------------------
  Initialize the root node
  ------------------------------------------------*/
  EndpointInitializer rootNode;
  rootNode.initialized        = false;
  rootNode.deviceAddress      = RF24::RootNode0;
  rootNode.parentAddress      = RF24::Network::RSVD_ADDR_INVALID;
  rootNode.deviceName         = "Node-000";
  rootNode.device             = nullptr;
  rootNode.idleThreadFunction = RootNodeThread;

  SystemNodes.push_back( rootNode );

  /*------------------------------------------------
  Initialize child 1 of the root node
  ------------------------------------------------*/
  EndpointInitializer childNode_001;
  childNode_001.initialized        = false;
  childNode_001.deviceAddress      = 001;
  childNode_001.parentAddress      = RF24::RootNode0;
  childNode_001.deviceName         = "Node-001";
  childNode_001.device             = nullptr;
  childNode_001.idleThreadFunction = ChildNodeThread_001;

  SystemNodes.push_back( childNode_001 );

  /*------------------------------------------------
  Initialize child 2 of the root node
  ------------------------------------------------*/
  EndpointInitializer childNode_002;
  childNode_002.initialized        = false;
  childNode_002.deviceAddress      = 002;
  childNode_002.parentAddress      = RF24::RootNode0;
  childNode_002.deviceName         = "Node-002";
  childNode_002.device             = nullptr;
  childNode_002.idleThreadFunction = ChildNodeThread_002;

  SystemNodes.push_back( childNode_002 );

  /*------------------------------------------------
  Initialize child 3 of the root node
  ------------------------------------------------*/
  EndpointInitializer childNode_003;
  childNode_003.initialized        = false;
  childNode_003.deviceAddress      = 003;
  childNode_003.parentAddress      = RF24::RootNode0;
  childNode_003.deviceName         = "Node-003";
  childNode_003.device             = nullptr;
  childNode_003.idleThreadFunction = ChildNodeThread_003;

  SystemNodes.push_back( childNode_003 );

  /*------------------------------------------------
  Initialize child 1 of node 002
  ------------------------------------------------*/
  EndpointInitializer childNode_012;
  childNode_012.initialized        = false;
  childNode_012.deviceAddress      = 012;
  childNode_012.parentAddress      = 002;
  childNode_012.deviceName         = "Node-012";
  childNode_012.device             = nullptr;
  childNode_012.idleThreadFunction = ChildNodeThread_012;

  SystemNodes.push_back( childNode_012 );

  /*------------------------------------------------
  Initialize child 1 of node 003
  ------------------------------------------------*/
  EndpointInitializer childNode_013;
  childNode_013.initialized        = false;
  childNode_013.deviceAddress      = 013;
  childNode_013.parentAddress      = 003;
  childNode_013.deviceName         = "Node-013";
  childNode_013.device             = nullptr;
  childNode_013.idleThreadFunction = ChildNodeThread_013;

  SystemNodes.push_back( childNode_013 );

  /*------------------------------------------------
  Initialize child 1 of node 013
  ------------------------------------------------*/
  EndpointInitializer childNode_0113;
  childNode_0113.initialized        = false;
  childNode_0113.deviceAddress      = 0113;
  childNode_0113.parentAddress      = 013;
  childNode_0113.deviceName         = "Node-0113";
  childNode_0113.device             = nullptr;
  childNode_0113.idleThreadFunction = ChildNodeThread_0113;

  SystemNodes.push_back( childNode_0113 );

  /*------------------------------------------------
  Initialize child 2 of node 0113
  ------------------------------------------------*/
  EndpointInitializer childNode_02113;
  childNode_02113.initialized        = false;
  childNode_02113.deviceAddress      = 02113;
  childNode_02113.parentAddress      = 0113;
  childNode_02113.deviceName         = "Node-02113";
  childNode_02113.device             = nullptr;
  childNode_02113.idleThreadFunction = ChildNodeThread_02113;

  SystemNodes.push_back( childNode_02113 );

  /*------------------------------------------------
  Initialize child 4 of node 02113
  ------------------------------------------------*/
  EndpointInitializer childNode_042113;
  childNode_042113.initialized        = false;
  childNode_042113.deviceAddress      = 042113;
  childNode_042113.parentAddress      = 02113;
  childNode_042113.deviceName         = "Node-042113";
  childNode_042113.device             = nullptr;
  childNode_042113.idleThreadFunction = ChildNodeThread_042113;

  SystemNodes.push_back( childNode_042113 );


  /*------------------------------------------------
  Start all the threads
  ------------------------------------------------*/
  for ( auto& item : SystemNodes )
  {
    SystemThreads.push_back( std::thread( item.idleThreadFunction, &item ) );
  }
 
  /*------------------------------------------------
  Idle away until the end of the universe
  ------------------------------------------------*/
  while ( true )
  {
    Chimera::delayMilliseconds( 100 );
  }
}

static void RootNodeThread( EndpointInitializer *init )
{
  SetThreadDescription( GetCurrentThread(), L"RootNodeThread" );

  /*------------------------------------------------
  Initialize the device logger
  ------------------------------------------------*/
  uLog::SinkHandle logSink = std::make_shared<uLog::CoutSink>();
  logSink->setLogLevel( uLog::Level::LVL_TRACE );
  logSink->setName( init->deviceName );
  logSink->enable();
  uLog::registerSink( logSink );

  /*------------------------------------------------
  Create and initialize the device
  ------------------------------------------------*/
  RF24::Endpoint::SystemInit cfg;
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = init->deviceAddress;
  cfg.network.parentStaticAddress = init->parentAddress;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;
  cfg.physical.deviceName     = init->deviceName;

  init->device = RF24::Endpoint::createShared( cfg );

  init->device->attachLogger( logSink );
  init->device->configure( cfg );
  init->device->setName( init->deviceName );

  /*------------------------------------------------
  Create an initialize data buffers
  ------------------------------------------------*/
  MessageType msg;


  /*------------------------------------------------
  Device Processing Thread
  ------------------------------------------------*/
  Chimera::delayMilliseconds( BootDelay );
  size_t current_time = Chimera::millis();
  size_t hello_time = Chimera::millis();

  while ( true )
  {
    if ( ( Chimera::millis() - current_time ) > AsyncUpdateRate )
    {
      init->device->doAsyncProcessing();
      current_time = Chimera::millis();
    }

    /*------------------------------------------------
    Make a single transmission to a child node
    ------------------------------------------------*/
    if ( ( Chimera::millis() - hello_time ) > SayHelloRate )
    {
      msg.crc = 0;
      msg.number = 2;

      init->device->write( 001, &msg, sizeof( MessageType ) );

      hello_time = Chimera::millis();
    }

    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}

using NetResult = RF24::Connection::Result;
using NetId = RF24::Connection::BindSite;

/*------------------------------------------------
Node 001
------------------------------------------------*/
static std::atomic<NetResult> isConnected;
static void ChildNode_001_ConnectCallback( NetResult result, NetId id )
{
  isConnected = result;
}

static void ChildNodeThread_001( EndpointInitializer *init )
{
  SetThreadDescription( GetCurrentThread(), L"ChildNode-001" );

  /*------------------------------------------------
  Initialize the device logger
  ------------------------------------------------*/
  uLog::SinkHandle logSink = std::make_shared<uLog::CoutSink>();
  logSink->setLogLevel( uLog::Level::LVL_TRACE );
  logSink->setName( init->deviceName );
  logSink->enable();
  uLog::registerSink( logSink );

  /*------------------------------------------------
  Create and initialize the device
  ------------------------------------------------*/
  RF24::Endpoint::SystemInit cfg;
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = init->deviceAddress;
  cfg.network.parentStaticAddress = init->parentAddress;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;
  cfg.physical.deviceName     = init->deviceName;

  init->device = RF24::Endpoint::createShared( cfg );

  init->device->attachLogger( logSink );
  init->device->configure( cfg );
  init->device->setName( init->deviceName );

  /*------------------------------------------------
  Initialize messaging data
  ------------------------------------------------*/
  MessageType rxData;

  /*------------------------------------------------
  Connect to the configured parent node
  ------------------------------------------------*/
  isConnected = NetResult::CONNECT_PROC_UNKNOWN;

  Chimera::delayMilliseconds( BootDelay );

  init->device->connectAsync( ChildNode_001_ConnectCallback, ConnectTimeout );
  while ( isConnected == NetResult::CONNECT_PROC_UNKNOWN )
  {
    init->device->processNetworking();
    Chimera::delayMilliseconds( 10 );
  }

  if ( isConnected == NetResult::CONNECT_PROC_SUCCESS )
  {
    logSink->flog( uLog::Level::LVL_INFO, "PASSED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }
  else
  {
    logSink->flog( uLog::Level::LVL_INFO, "FAILED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }

  /*------------------------------------------------
  Device Processing Thread
  ------------------------------------------------*/
  size_t current_time = Chimera::millis();

  while ( true )
  {
    if ( ( Chimera::millis() - current_time ) > AsyncUpdateRate )
    {
      init->device->doAsyncProcessing();
      current_time = Chimera::millis();
    }

    if ( init->device->packetAvailable() )
    {
      size_t packetSize = init->device->nextPacketLength();

      if ( packetSize == sizeof( MessageType ) )
      {
        init->device->read( &rxData, sizeof( MessageType ) );
      }
    }

    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}

/*------------------------------------------------
Node 002
------------------------------------------------*/
static std::atomic<NetResult> isConnected_002;
static void ChildNode_002_ConnectCallback( NetResult result, NetId id )
{
  isConnected_002 = result;
}

static void ChildNodeThread_002( EndpointInitializer *init )
{
  SetThreadDescription( GetCurrentThread(), L"ChildNode-002" );

  /*------------------------------------------------
  Initialize the device logger
  ------------------------------------------------*/
  uLog::SinkHandle logSink = std::make_shared<uLog::CoutSink>();
  logSink->setLogLevel( uLog::Level::LVL_TRACE );
  logSink->setName( init->deviceName );
  logSink->enable();
  uLog::registerSink( logSink );

  /*------------------------------------------------
  Create and initialize the device
  ------------------------------------------------*/
  RF24::Endpoint::SystemInit cfg;
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = init->deviceAddress;
  cfg.network.parentStaticAddress = init->parentAddress;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;
  cfg.physical.deviceName     = init->deviceName;

  init->device = RF24::Endpoint::createShared( cfg );

  init->device->attachLogger( logSink );
  init->device->configure( cfg );
  init->device->setName( init->deviceName );

  /*------------------------------------------------
  Connect to the configured parent node
  ------------------------------------------------*/
  isConnected_002 = NetResult::CONNECT_PROC_UNKNOWN;

  Chimera::delayMilliseconds( BootDelay );

  init->device->connectAsync( ChildNode_002_ConnectCallback, ConnectTimeout );
  while ( isConnected_002 == NetResult::CONNECT_PROC_UNKNOWN )
  {
    init->device->processNetworking();
    Chimera::delayMilliseconds( 10 );
  }

  if ( isConnected_002 == NetResult::CONNECT_PROC_SUCCESS )
  {
    logSink->flog( uLog::Level::LVL_INFO, "PASSED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }
  else
  {
    logSink->flog( uLog::Level::LVL_INFO, "FAILED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }

  /*------------------------------------------------
  Device Processing Thread
  ------------------------------------------------*/
  size_t current_time = Chimera::millis();

  while ( true )
  {
    if ( ( Chimera::millis() - current_time ) > AsyncUpdateRate )
    {
      init->device->doAsyncProcessing();
      current_time = Chimera::millis();
    }

    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}

/*------------------------------------------------
Node 003
------------------------------------------------*/
static std::atomic<NetResult> isConnected_003;
static void ChildNode_003_ConnectCallback( NetResult result, NetId id )
{
  isConnected_003 = result;
}

static void ChildNodeThread_003( EndpointInitializer *init )
{
  SetThreadDescription( GetCurrentThread(), L"ChildNode-003" );

  /*------------------------------------------------
  Initialize the device logger
  ------------------------------------------------*/
  uLog::SinkHandle logSink = std::make_shared<uLog::CoutSink>();
  logSink->setLogLevel( uLog::Level::LVL_TRACE );
  logSink->setName( init->deviceName );
  logSink->enable();
  uLog::registerSink( logSink );

  /*------------------------------------------------
  Create and initialize the device
  ------------------------------------------------*/
  RF24::Endpoint::SystemInit cfg;
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = init->deviceAddress;
  cfg.network.parentStaticAddress = init->parentAddress;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;
  cfg.physical.deviceName     = init->deviceName;

  init->device = RF24::Endpoint::createShared( cfg );

  init->device->attachLogger( logSink );
  init->device->configure( cfg );
  init->device->setName( init->deviceName );

  /*------------------------------------------------
  Connect to the configured parent node
  ------------------------------------------------*/
  isConnected_003 = NetResult::CONNECT_PROC_UNKNOWN;

  Chimera::delayMilliseconds( BootDelay );

  init->device->connectAsync( ChildNode_003_ConnectCallback, ConnectTimeout );
  while ( isConnected_003 == NetResult::CONNECT_PROC_UNKNOWN )
  {
    init->device->processNetworking();
    Chimera::delayMilliseconds( 10 );
  }

  if ( isConnected_003 == NetResult::CONNECT_PROC_SUCCESS )
  {
    logSink->flog( uLog::Level::LVL_INFO, "PASSED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }
  else
  {
    logSink->flog( uLog::Level::LVL_INFO, "FAILED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }

  /*------------------------------------------------
  Device Processing Thread
  ------------------------------------------------*/
  size_t current_time = Chimera::millis();

  while ( true )
  {
    if ( ( Chimera::millis() - current_time ) > AsyncUpdateRate )
    {
      init->device->doAsyncProcessing();
      current_time = Chimera::millis();
    }

    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}

/*------------------------------------------------
Node 012
------------------------------------------------*/
static std::atomic<NetResult> isConnected_012;
static void ChildNode_012_ConnectCallback( NetResult result, NetId id )
{
  isConnected_012 = result;
}

static void ChildNodeThread_012( EndpointInitializer *init )
{
  SetThreadDescription( GetCurrentThread(), L"ChildNode-012" );

  /*------------------------------------------------
  Initialize the device logger
  ------------------------------------------------*/
  uLog::SinkHandle logSink = std::make_shared<uLog::CoutSink>();
  logSink->setLogLevel( uLog::Level::LVL_TRACE );
  logSink->setName( init->deviceName );
  logSink->enable();
  uLog::registerSink( logSink );

  /*------------------------------------------------
  Create and initialize the device
  ------------------------------------------------*/
  RF24::Endpoint::SystemInit cfg;
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = init->deviceAddress;
  cfg.network.parentStaticAddress = init->parentAddress;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;
  cfg.physical.deviceName     = init->deviceName;

  init->device = RF24::Endpoint::createShared( cfg );

  init->device->attachLogger( logSink );
  init->device->configure( cfg );
  init->device->setName( init->deviceName );

  /*------------------------------------------------
  Connect to the configured parent node
  ------------------------------------------------*/
  isConnected_012 = NetResult::CONNECT_PROC_UNKNOWN;

  Chimera::delayMilliseconds( BootDelay );

  init->device->connectAsync( ChildNode_012_ConnectCallback, ConnectTimeout );
  while ( isConnected_012 == NetResult::CONNECT_PROC_UNKNOWN )
  {
    init->device->processNetworking();
    Chimera::delayMilliseconds( 10 );
  }

  if ( isConnected_012 == NetResult::CONNECT_PROC_SUCCESS )
  {
    logSink->flog( uLog::Level::LVL_INFO, "PASSED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }
  else
  {
    logSink->flog( uLog::Level::LVL_INFO, "FAILED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }

  /*------------------------------------------------
  Device Processing Thread
  ------------------------------------------------*/
  size_t current_time = Chimera::millis();

  while ( true )
  {
    if ( ( Chimera::millis() - current_time ) > AsyncUpdateRate )
    {
      init->device->doAsyncProcessing();
      current_time = Chimera::millis();
    }

    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}

/*------------------------------------------------
Node 013
------------------------------------------------*/
static std::atomic<NetResult> isConnected_013;
static void ChildNode_013_ConnectCallback( NetResult result, NetId id )
{
  isConnected_013 = result;
}

static void ChildNodeThread_013( EndpointInitializer *init )
{
  SetThreadDescription( GetCurrentThread(), L"ChildNode-013" );

  /*------------------------------------------------
  Initialize the device logger
  ------------------------------------------------*/
  uLog::SinkHandle logSink = std::make_shared<uLog::CoutSink>();
  logSink->setLogLevel( uLog::Level::LVL_TRACE );
  logSink->setName( init->deviceName );
  logSink->enable();
  uLog::registerSink( logSink );

  /*------------------------------------------------
  Create and initialize the device
  ------------------------------------------------*/
  RF24::Endpoint::SystemInit cfg;
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = init->deviceAddress;
  cfg.network.parentStaticAddress = init->parentAddress;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;
  cfg.physical.deviceName     = init->deviceName;

  init->device = RF24::Endpoint::createShared( cfg );

  init->device->attachLogger( logSink );
  init->device->configure( cfg );
  init->device->setName( init->deviceName );

  /*------------------------------------------------
  Connect to the configured parent node
  ------------------------------------------------*/
  isConnected_013 = NetResult::CONNECT_PROC_UNKNOWN;

  Chimera::delayMilliseconds( BootDelay );

  init->device->connectAsync( ChildNode_013_ConnectCallback, ConnectTimeout );
  while ( isConnected_013 == NetResult::CONNECT_PROC_UNKNOWN )
  {
    init->device->processNetworking();
    Chimera::delayMilliseconds( 10 );
  }

  if ( isConnected_013 == NetResult::CONNECT_PROC_SUCCESS )
  {
    logSink->flog( uLog::Level::LVL_INFO, "PASSED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }
  else
  {
    logSink->flog( uLog::Level::LVL_INFO, "FAILED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }

  /*------------------------------------------------
  Device Processing Thread
  ------------------------------------------------*/
  size_t current_time = Chimera::millis();

  while ( true )
  {
    if ( ( Chimera::millis() - current_time ) > AsyncUpdateRate )
    {
      init->device->doAsyncProcessing();
      current_time = Chimera::millis();
    }

    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}

/*------------------------------------------------
Node 0113
------------------------------------------------*/
static std::atomic<NetResult> isConnected_0113;
static void ChildNode_0113_ConnectCallback( NetResult result, NetId id )
{
  isConnected_0113 = result;
}

static void ChildNodeThread_0113( EndpointInitializer *init )
{
  SetThreadDescription( GetCurrentThread(), L"ChildNode-0113" );

  /*------------------------------------------------
  Initialize the device logger
  ------------------------------------------------*/
  uLog::SinkHandle logSink = std::make_shared<uLog::CoutSink>();
  logSink->setLogLevel( uLog::Level::LVL_TRACE );
  logSink->setName( init->deviceName );
  logSink->enable();
  uLog::registerSink( logSink );

  /*------------------------------------------------
  Create and initialize the device
  ------------------------------------------------*/
  RF24::Endpoint::SystemInit cfg;
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = init->deviceAddress;
  cfg.network.parentStaticAddress = init->parentAddress;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;
  cfg.physical.deviceName     = init->deviceName;

  init->device = RF24::Endpoint::createShared( cfg );

  init->device->attachLogger( logSink );
  init->device->configure( cfg );
  init->device->setName( init->deviceName );

  /*------------------------------------------------
  Connect to the configured parent node
  ------------------------------------------------*/
  isConnected_0113 = NetResult::CONNECT_PROC_UNKNOWN;

  Chimera::delayMilliseconds( BootDelay );

  init->device->connectAsync( ChildNode_0113_ConnectCallback, ConnectTimeout );
  while ( isConnected_0113 == NetResult::CONNECT_PROC_UNKNOWN )
  {
    init->device->processNetworking();
    Chimera::delayMilliseconds( 10 );
  }

  if ( isConnected_0113 == NetResult::CONNECT_PROC_SUCCESS )
  {
    logSink->flog( uLog::Level::LVL_INFO, "PASSED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }
  else
  {
    logSink->flog( uLog::Level::LVL_INFO, "FAILED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }

  /*------------------------------------------------
  Device Processing Thread
  ------------------------------------------------*/
  size_t current_time = Chimera::millis();

  while ( true )
  {
    if ( ( Chimera::millis() - current_time ) > AsyncUpdateRate )
    {
      init->device->doAsyncProcessing();
      current_time = Chimera::millis();
    }

    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}

/*------------------------------------------------
Node 02113
------------------------------------------------*/
static std::atomic<NetResult> isConnected_02113;
static void ChildNode_02113_ConnectCallback( NetResult result, NetId id )
{
  isConnected_02113 = result;
}

static void ChildNodeThread_02113( EndpointInitializer *init )
{
  SetThreadDescription( GetCurrentThread(), L"ChildNode-02113" );

  /*------------------------------------------------
  Initialize the device logger
  ------------------------------------------------*/
  uLog::SinkHandle logSink = std::make_shared<uLog::CoutSink>();
  logSink->setLogLevel( uLog::Level::LVL_TRACE );
  logSink->setName( init->deviceName );
  logSink->enable();
  uLog::registerSink( logSink );

  /*------------------------------------------------
  Create and initialize the device
  ------------------------------------------------*/
  RF24::Endpoint::SystemInit cfg;
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = init->deviceAddress;
  cfg.network.parentStaticAddress = init->parentAddress;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;
  cfg.physical.deviceName     = init->deviceName;

  init->device = RF24::Endpoint::createShared( cfg );

  init->device->attachLogger( logSink );
  init->device->configure( cfg );
  init->device->setName( init->deviceName );

  /*------------------------------------------------
  Connect to the configured parent node
  ------------------------------------------------*/
  isConnected_02113 = NetResult::CONNECT_PROC_UNKNOWN;

  Chimera::delayMilliseconds( BootDelay );

  init->device->connectAsync( ChildNode_02113_ConnectCallback, ConnectTimeout );
  while ( isConnected_02113 == NetResult::CONNECT_PROC_UNKNOWN )
  {
    init->device->processNetworking();
    Chimera::delayMilliseconds( 10 );
  }

  if ( isConnected_02113 == NetResult::CONNECT_PROC_SUCCESS )
  {
    logSink->flog( uLog::Level::LVL_INFO, "PASSED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }
  else
  {
    logSink->flog( uLog::Level::LVL_INFO, "FAILED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }

  /*------------------------------------------------
  Device Processing Thread
  ------------------------------------------------*/
  size_t current_time = Chimera::millis();

  while ( true )
  {
    if ( ( Chimera::millis() - current_time ) > AsyncUpdateRate )
    {
      init->device->doAsyncProcessing();
      current_time = Chimera::millis();
    }

    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}

/*------------------------------------------------
Node 042113
------------------------------------------------*/
static std::atomic<NetResult> isConnected_042113;
static void ChildNode_042113_ConnectCallback( NetResult result, NetId id )
{
  isConnected_042113 = result;
}

static void ChildNodeThread_042113( EndpointInitializer *init )
{
  SetThreadDescription( GetCurrentThread(), L"ChildNode-042113" );

  /*------------------------------------------------
  Initialize the device logger
  ------------------------------------------------*/
  uLog::SinkHandle logSink = std::make_shared<uLog::CoutSink>();
  logSink->setLogLevel( uLog::Level::LVL_TRACE );
  logSink->setName( init->deviceName );
  logSink->enable();
  uLog::registerSink( logSink );

  /*------------------------------------------------
  Create and initialize the device
  ------------------------------------------------*/
  RF24::Endpoint::SystemInit cfg;
  cfg.network.mode                = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress   = init->deviceAddress;
  cfg.network.parentStaticAddress = init->parentAddress;
  cfg.network.rxQueueBuffer       = nullptr;
  cfg.network.rxQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer       = nullptr;
  cfg.network.txQueueSize         = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate       = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel      = 96;
  cfg.physical.deviceName     = init->deviceName;

  init->device = RF24::Endpoint::createShared( cfg );

  init->device->attachLogger( logSink );
  init->device->configure( cfg );
  init->device->setName( init->deviceName );

  /*------------------------------------------------
  Connect to the configured parent node
  ------------------------------------------------*/
  isConnected_042113 = NetResult::CONNECT_PROC_UNKNOWN;

  Chimera::delayMilliseconds( BootDelay );

  init->device->connectAsync( ChildNode_042113_ConnectCallback, ConnectTimeout );
  while ( isConnected_042113 == NetResult::CONNECT_PROC_UNKNOWN )
  {
    init->device->processNetworking();
    Chimera::delayMilliseconds( 10 );
  }

  if ( isConnected_042113 == NetResult::CONNECT_PROC_SUCCESS )
  {
    logSink->flog( uLog::Level::LVL_INFO, "PASSED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }
  else
  {
    logSink->flog( uLog::Level::LVL_INFO, "FAILED connecting node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }

  /*------------------------------------------------
  Device Processing Thread
  ------------------------------------------------*/
  size_t current_time = Chimera::millis();

  while ( true )
  {
    if ( ( Chimera::millis() - current_time ) > AsyncUpdateRate )
    {
      init->device->doAsyncProcessing();
      current_time = Chimera::millis();
    }

    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}
