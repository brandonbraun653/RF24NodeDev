/********************************************************************************
*  File Name:
*    exported_test_functions.cpp
*
*  Description:
*
*
*  2020 | Brandon Braun | brandonbraun653@gmail.com
********************************************************************************/

/* C++ Includes */
#include <atomic>
#include <thread>
#include <vector>

/* Project Includes */
#include "exported_test_functions.hpp"

/* Chimera Includes */
#include <Chimera/common>

/* RF24 Includes */
#include <RF24Node/src/common/conversion.hpp>
#include <RF24Node/src/common/definitions.hpp>
#include <RF24Node/src/endpoint/endpoint.hpp>
#include <RF24Node/src/endpoint/types.hpp>
#include <RF24Node/src/physical/simulator/pipe.hpp>
#include <RF24Node/src/physical/simulator/shockburst.hpp>
#include <RF24Node/src/physical/simulator/shockburst_types.hpp>

/* Logger Includes */
#include <uLog/ulog.hpp>
#include <uLog/sinks/sink_cout.hpp>

static void MasterNodeThread();
static void SlaveNodeThread();
static void RootNodeThread();
static void ChildNodeThread_001();
static void ChildNodeThread_002();
static void ChildNodeThread_012();
static void ChildNodeThread_003();
static void ChildNodeThread_013();

static constexpr size_t AsyncUpdateRate = 50;
static constexpr size_t ThreadUpdateRate = 15;

static RF24::Endpoint::Device_sPtr rootNode;
static RF24::Endpoint::Device_sPtr childNode_001;
static RF24::Endpoint::Device_sPtr childNode_002;
static RF24::Endpoint::Device_sPtr childNode_012;
static RF24::Endpoint::Device_sPtr childNode_003;
static RF24::Endpoint::Device_sPtr childNode_013;


struct EndpointInitializer
{
  RF24::Endpoint::Device_sPtr device;
  std::thread idleThread;
  std::atomic<bool> initialized;
};

void test_PipeCommunication()
{
  ChimeraInit();

  uLog::initialize();

  uLog::SinkHandle rootSink = std::make_shared<uLog::CoutSink>();
  rootSink->setLogLevel( uLog::Level::LVL_DEBUG );
  rootSink->setName( "Root" );
  rootSink->enable();

  uLog::registerSink( rootSink );
  uLog::setGlobalLogLevel( uLog::Level::LVL_DEBUG );
  uLog::setRootSink( rootSink );

  /*------------------------------------------------
  Create master/slave hardware threads
  ------------------------------------------------*/
  std::thread masterThread = std::thread( MasterNodeThread );
  std::thread slaveThread = std::thread( SlaveNodeThread );

  std::thread rootNodeThread = std::thread( RootNodeThread );
  std::thread childNodeThread_001 = std::thread( ChildNodeThread_001 );
  std::thread childNodeThread_002 = std::thread( ChildNodeThread_002 );
  std::thread childNodeThread_012 = std::thread( ChildNodeThread_012 );
  std::thread childNodeThread_003 = std::thread( ChildNodeThread_003 );
  std::thread childNodeThread_013 = std::thread( ChildNodeThread_013 );

  while ( true )
  {
    Chimera::delayMilliseconds( 100 );
  }

}


void SandboxThread()
{

}



void MasterNodeThread()
{
  uLog::SinkHandle masterSink = std::make_shared<uLog::CoutSink>();
  masterSink->setLogLevel( uLog::Level::LVL_DEBUG );
  masterSink->setName( "Master" );
  masterSink->enable();
  uLog::registerSink( masterSink );

  RF24::Endpoint::Device master;
  RF24::Endpoint::SystemInit cfg;
  cfg.network.mode = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress = RF24::RootNode0;
  cfg.network.parentStaticAddress = RF24::Network::RSVD_ADDR_INVALID;
  cfg.network.rxQueueBuffer = nullptr;
  cfg.network.rxQueueSize = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer = nullptr;
  cfg.network.txQueueSize = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel = 96;
  cfg.physical.deviceName = "Master";

  master.attachLogger( masterSink );
  master.configure( cfg );
  master.setName( cfg.physical.deviceName );

  /*------------------------------------------------
  Main processing loop for the slave node
  ------------------------------------------------*/
  const size_t start_time = Chimera::millis();
  size_t asyncUpdateProcessTime = start_time;
  size_t testCodeProcessTime = start_time;

  while ( true )
  {
    /*------------------------------------------------
    Handle the incoming RF data
    ------------------------------------------------*/
    if ( ( Chimera::millis() - asyncUpdateProcessTime ) > AsyncUpdateRate )
    {
      master.doAsyncProcessing();
      asyncUpdateProcessTime = Chimera::millis();
    }

    /*------------------------------------------------
    Process any test code used for development
    ------------------------------------------------*/
    if ( ( Chimera::millis() - testCodeProcessTime ) > 1000 )
    {
      ;
    }


    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}

void SlaveNodeThread()
{
  uLog::SinkHandle slaveSink = std::make_shared<uLog::CoutSink>();
  slaveSink->setLogLevel( uLog::Level::LVL_DEBUG );
  slaveSink->setName( "Slave" );
  slaveSink->enable();
  uLog::registerSink( slaveSink );

  RF24::Endpoint::Device slave;
  RF24::Endpoint::SystemInit cfg;
  cfg.network.mode = RF24::Network::Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress = 0001;
  cfg.network.parentStaticAddress = RF24::RootNode0;
  cfg.network.rxQueueBuffer = nullptr;
  cfg.network.rxQueueSize = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer = nullptr;
  cfg.network.txQueueSize = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel = 96;
  cfg.physical.deviceName = "Slave";

  slave.attachLogger( slaveSink );
  slave.configure( cfg );
  slave.setName( cfg.physical.deviceName );

  if ( slave.connect( 1000 ) )
  {
    slaveSink->flog( uLog::Level::LVL_INFO, "Holy crap it worked?!\n" );
  }
  else
  {
    slaveSink->flog( uLog::Level::LVL_INFO, "Did not connect for some reason\n" );
  }

  /*------------------------------------------------
  Main processing loop for the slave node
  ------------------------------------------------*/
  const size_t start_time = Chimera::millis();
  size_t asyncUpdateProcessTime = start_time;
  size_t testCodeProcessTime = start_time;

  std::string_view hello_world = "hello world!";

  while ( true )
  {
    /*------------------------------------------------
    Handle the incoming RF data
    ------------------------------------------------*/
    if ( ( Chimera::millis() - asyncUpdateProcessTime ) > AsyncUpdateRate )
    {
      slave.doAsyncProcessing();
      asyncUpdateProcessTime = Chimera::millis();
    }

    /*------------------------------------------------
    Process any test code used for development
    ------------------------------------------------*/
    if ( ( Chimera::millis() - testCodeProcessTime ) > 1000 )
    {
      //slave.write( RF24::RootNode0, hello_world.data(), hello_world.size() );
      slaveSink->flog( uLog::Level::LVL_INFO, "%d-Pinging the root node\n", Chimera::millis() );
      if ( slave.ping( RF24::RootNode0, 150 ) )
      {
        slaveSink->flog( uLog::Level::LVL_INFO, "%d-Success!\n", Chimera::millis() );
      }
      else
      {
        slaveSink->flog( uLog::Level::LVL_INFO, "%d-Failure :(\n", Chimera::millis() );

      }

      testCodeProcessTime = Chimera::millis();
    }


    Chimera::delayMilliseconds( ThreadUpdateRate );
  }
}

static void RootNodeThread()
{

}

static void ChildNodeThread_001()
{

}

static void ChildNodeThread_002()
{

}

static void ChildNodeThread_012()
{

}

static void ChildNodeThread_003()
{

}

static void ChildNodeThread_013()
{

}
