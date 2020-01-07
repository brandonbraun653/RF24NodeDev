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
#include <thread>

/* Boost Includes */
#include <boost/asio.hpp>
#include <boost/chrono.hpp>

/* Project Includes */
#include "exported_test_functions.hpp"

/* Chimera Includes */
#include <Chimera/chimera.hpp>

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

static void MasterNodeThread();
static void SlaveNodeThread();

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

  while ( true )
  {
    boost::this_thread::sleep_for( boost::chrono::milliseconds( 100 ) );
  }

}

void MasterNodeThread()
{
  uLog::SinkHandle masterSink = std::make_shared<uLog::CoutSink>();
  masterSink->setLogLevel( uLog::Level::LVL_DEBUG );
  masterSink->setName( "Master" );
  masterSink->enable();
  uLog::registerSink( masterSink );

  RF24::Endpoint master;
  RF24::EndpointConfig cfg;
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

  master.attachLogger( masterSink );
  master.configure( cfg );


  while ( true )
  {
    master.processNetworking();

    Chimera::delayMilliseconds( 25 );
  }
}

void SlaveNodeThread()
{
  Chimera::delayMilliseconds( 150 );

  uLog::SinkHandle slaveSink = std::make_shared<uLog::CoutSink>();
  slaveSink->setLogLevel( uLog::Level::LVL_DEBUG );
  slaveSink->setName( "Slave" );
  slaveSink->enable();
  uLog::registerSink( slaveSink );

  RF24::Endpoint slave;
  RF24::EndpointConfig cfg;
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


  slave.attachLogger( slaveSink );
  slave.configure( cfg );
  slave.connect( 1000 );

  while ( true )
  {

    Chimera::delayMilliseconds( 25 );
  }
}
