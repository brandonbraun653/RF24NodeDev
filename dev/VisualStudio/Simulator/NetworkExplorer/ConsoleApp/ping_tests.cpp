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

static constexpr size_t AsyncUpdateRate  = 50;
static constexpr size_t ThreadUpdateRate = 15;

static void MasterNodePingThread();
static void SlaveNodePingThread();

void RunPingTests()
{
  auto masterThread = std::thread( MasterNodePingThread );
  auto slaveThread  = std::thread( SlaveNodePingThread );

  while ( true )
  {
    Chimera::delayMilliseconds( 100 );
  }
}

static void MasterNodePingThread()
{
  uLog::SinkHandle masterSink = std::make_shared<uLog::CoutSink>();
  masterSink->setLogLevel( uLog::Level::LVL_DEBUG );
  masterSink->setName( "Master" );
  masterSink->enable();
  uLog::registerSink( masterSink );

  RF24::Endpoint::SystemInit cfg;
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
  cfg.physical.deviceName     = "Master";

  auto master = RF24::Endpoint::createShared( cfg );

  master->attachLogger( masterSink );
  master->configure( cfg );
  master->setName( cfg.physical.deviceName );

  /*------------------------------------------------
  Main processing loop for the slave node
  ------------------------------------------------*/
  const size_t start_time       = Chimera::millis();
  size_t asyncUpdateProcessTime = start_time;
  size_t testCodeProcessTime    = start_time;

  while ( true )
  {
    /*------------------------------------------------
    Handle the incoming RF data
    ------------------------------------------------*/
    if ( ( Chimera::millis() - asyncUpdateProcessTime ) > AsyncUpdateRate )
    {
      master->doAsyncProcessing();
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

static void SlaveNodePingThread()
{
  uLog::SinkHandle slaveSink = std::make_shared<uLog::CoutSink>();
  slaveSink->setLogLevel( uLog::Level::LVL_DEBUG );
  slaveSink->setName( "Slave" );
  slaveSink->enable();
  uLog::registerSink( slaveSink );


  RF24::Endpoint::SystemInit cfg;
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
  cfg.physical.deviceName     = "Slave";

  auto slave = RF24::Endpoint::createShared( cfg );

  slave->attachLogger( slaveSink );
  slave->configure( cfg );
  slave->setName( cfg.physical.deviceName );

  if ( slave->connect( 1000 ) )
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
  const size_t start_time       = Chimera::millis();
  size_t asyncUpdateProcessTime = start_time;
  size_t testCodeProcessTime    = start_time;

  std::string_view hello_world = "hello world!";

  while ( true )
  {
    /*------------------------------------------------
    Handle the incoming RF data
    ------------------------------------------------*/
    if ( ( Chimera::millis() - asyncUpdateProcessTime ) > AsyncUpdateRate )
    {
      slave->doAsyncProcessing();
      asyncUpdateProcessTime = Chimera::millis();
    }

    /*------------------------------------------------
    Process any test code used for development
    ------------------------------------------------*/
    if ( ( Chimera::millis() - testCodeProcessTime ) > 1000 )
    {
      // slave->write( RF24::RootNode0, hello_world.data(), hello_world.size() );
      slaveSink->flog( uLog::Level::LVL_INFO, "%d-Pinging the root node\n", Chimera::millis() );
      if ( slave->ping( RF24::RootNode0, 150 ) )
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
