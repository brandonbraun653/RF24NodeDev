/********************************************************************************
 *  File Name:
 *    main.cpp
 *
 *  Description:
 *    Development test bed / sandbox environment for simulating the RF24 network stack
 *
 *  2020 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

/* STL Includes */
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

/* Chimera Includes */
#include <Chimera/common>

/* Logger Includes */
#include <uLog/ulog.hpp>
#include <uLog/sinks/sink_cout.hpp>

/* Dev Includes */
#include <ping_tests.hpp>
#include <multi_node_tests.hpp>
#include <test_connection.hpp>

int main()
{
  ChimeraInit();

  /*------------------------------------------------
  Initialize the global logger that anyone can use
  ------------------------------------------------*/
  uLog::initialize();

  uLog::SinkHandle rootSink = std::make_shared<uLog::CoutSink>();
  rootSink->setLogLevel( uLog::Level::LVL_TRACE );
  rootSink->setName( "Global" );
  rootSink->enable();

  uLog::registerSink( rootSink );
  uLog::setGlobalLogLevel( uLog::Level::LVL_TRACE );
  uLog::setRootSink( rootSink );

  /*------------------------------------------------
  Enable which tests to run
  ------------------------------------------------*/
  rootSink->flog( uLog::Level::LVL_INFO, "Executing tests\n" );
  //RunPingTests();
  //RunMultiNodeTests();
  RunConnectionTests();
  
  return 0;
}
