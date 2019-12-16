
/* Boost Includes */
#include <boost/asio.hpp>
#include <boost/chrono.hpp>

/* Project Includes */
#include "exported_test_functions.hpp"

/* RF24 Includes */
#include <RF24Node/common/definitions.hpp>
#include <RF24Node/endpoint/endpoint.hpp>
#include <RF24Node/endpoint/types.hpp>
#include <RF24Node/physical/simulator/conversion.hpp>
#include <RF24Node/physical/simulator/pipe.hpp>
#include <RF24Node/physical/simulator/shockburst.hpp>
#include <RF24Node/physical/simulator/shockburst_types.hpp>

/* Logger Includes */
#include <uLog/ulog.hpp>
#include <uLog/sinks/sink_cout.hpp>

static boost::asio::io_service ioService;


void test_PipeCommunication()
{
  using namespace RF24::Network;


  uLog::SinkType consoleSink = std::make_shared<uLog::CoutSink>();
  consoleSink->setLogLevel( uLog::Level::LVL_DEBUG );

  uLog::initialize();
  uLog::enableSink( uLog::registerSink( consoleSink ) );
  uLog::setGlobalLogLevel( uLog::Level::LVL_DEBUG );
  uLog::flog( uLog::Level::LVL_DEBUG, "Program start\r\n" );

  /*------------------------------------------------
  Create a master node
  ------------------------------------------------*/
  RF24::Endpoint master;

  RF24::EndpointConfig cfg;
  cfg.network.mode = Mode::NET_MODE_STATIC;
  cfg.network.nodeStaticAddress = RF24::RootNode0;
  cfg.network.parentStaticAddress = RF24::InvalidNode;
  cfg.network.rxQueueBuffer = nullptr;
  cfg.network.rxQueueSize = 5 * RF24::Hardware::PACKET_WIDTH;
  cfg.network.txQueueBuffer = nullptr;
  cfg.network.txQueueSize = 5 * RF24::Hardware::PACKET_WIDTH;

  cfg.physical.dataRate = RF24::Hardware::DataRate::DR_1MBPS;
  cfg.physical.powerAmplitude = RF24::Hardware::PowerAmplitude::PA_HIGH;
  cfg.physical.rfChannel = 96;

  master.configure( cfg );

  /*------------------------------------------------
  Create a slave node
  ------------------------------------------------*/
  RF24::Endpoint slave;

  cfg.network.nodeStaticAddress = 0001;

  slave.configure( cfg );
  slave.connect();

  while (!slave.isConnected())
  {
    uLog::flog(uLog::Level::LVL_INFO, "Still waiting for slave to connect...\n" );
    boost::this_thread::sleep_for( boost::chrono::milliseconds( 1000 ) );
  }

  while ( true )
  {
   

    boost::this_thread::sleep_for( boost::chrono::milliseconds( 100 ) );
  }

}