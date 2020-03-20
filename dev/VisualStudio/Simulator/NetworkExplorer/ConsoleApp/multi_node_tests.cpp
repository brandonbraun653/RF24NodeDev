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
static void ChildNodeThread_113( EndpointInitializer *init );

void RunMultiNodeTests()
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
  Initialize child 1 of node 003
  ------------------------------------------------*/
  EndpointInitializer childNode_113;
  childNode_113.initialized        = false;
  childNode_113.deviceAddress      = 0113;
  childNode_113.parentAddress      = 003;
  childNode_113.deviceName         = "Node-113";
  childNode_113.device             = nullptr;
  childNode_113.idleThreadFunction = ChildNodeThread_113;

  SystemNodes.push_back( childNode_113 );


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
    Let the world know you are still alive
    ------------------------------------------------*/
    if ( ( Chimera::millis() - hello_time ) > SayHelloRate )
    {
      logSink->flog( uLog::Level::LVL_INFO, "Hello\n");
      hello_time = Chimera::millis();
    }

    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}

using NetResult = RF24::Connection::Result;

std::atomic<NetResult> isConnected;
static void ChildNode_001_ConnectCallback( NetResult result )
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
  Connect to the configured parent node
  ------------------------------------------------*/
  isConnected = NetResult::CONNECTION_UNKNOWN;

  Chimera::delayMilliseconds( BootDelay );

  init->device->connect( ChildNode_001_ConnectCallback, ConnectTimeout );
  while ( isConnected == NetResult::CONNECTION_UNKNOWN )
  {
    init->device->processNetworking();
    Chimera::delayMilliseconds( 10 );
  }

  if ( isConnected == NetResult::CONNECTION_SUCCESS )
  {
    logSink->flog( uLog::Level::LVL_INFO, "Connected node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
                   cfg.network.parentStaticAddress );
  }
  else
  {
    logSink->flog( uLog::Level::LVL_INFO, "Did not connect to the network\n" );
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

static void ChildNodeThread_002( EndpointInitializer *init )
{
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
  Chimera::delayMilliseconds( BootDelay );
  //if ( init->device->connect( ConnectTimeout ) )
  //{
  //  logSink->flog( uLog::Level::LVL_INFO, "Connected node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
  //                 cfg.network.parentStaticAddress );
  //}
  //else
  //{
  //  logSink->flog( uLog::Level::LVL_INFO, "Did not connect to the network\n" );
  //}

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

static void ChildNodeThread_003( EndpointInitializer *init )
{
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
  Chimera::delayMilliseconds( BootDelay );
  //if ( init->device->connect( ConnectTimeout ) )
  //{
  //  logSink->flog( uLog::Level::LVL_INFO, "Connected node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
  //                 cfg.network.parentStaticAddress );
  //}
  //else
  //{
  //  logSink->flog( uLog::Level::LVL_INFO, "Did not connect to the network\n" );
  //}

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

static void ChildNodeThread_012( EndpointInitializer *init )
{
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
  Chimera::delayMilliseconds( BootDelay );
  //if ( init->device->connect( ConnectTimeout ) )
  //{
  //  logSink->flog( uLog::Level::LVL_INFO, "Connected node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
  //                 cfg.network.parentStaticAddress );
  //}
  //else
  //{
  //  logSink->flog( uLog::Level::LVL_INFO, "Did not connect to the network\n" );
  //}

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

static void ChildNodeThread_013( EndpointInitializer *init )
{
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
  Chimera::delayMilliseconds( BootDelay );
  //if ( init->device->connect( ConnectTimeout ) )
  //{
  //  logSink->flog( uLog::Level::LVL_INFO, "Connected node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
  //                 cfg.network.parentStaticAddress );
  //}
  //else
  //{
  //  logSink->flog( uLog::Level::LVL_INFO, "Did not connect to the network\n" );
  //}

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

static void ChildNodeThread_113( EndpointInitializer *init )
{
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
  Chimera::delayMilliseconds( BootDelay );
  //if ( init->device->connect( ConnectTimeout ) )
  //{
  //  logSink->flog( uLog::Level::LVL_INFO, "Connected node [%04o] to node [%04o]\n", cfg.network.nodeStaticAddress,
  //                 cfg.network.parentStaticAddress );
  //}
  //else
  //{
  //  logSink->flog( uLog::Level::LVL_INFO, "Did not connect to the network\n" );
  //}

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
