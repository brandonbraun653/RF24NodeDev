
/* Boost Includes */
#include <boost/asio.hpp>

/* Project Includes */
#include "exported_test_functions.hpp"

/* RF24 Includes */
#include <RF24Node/physical/simulator/pipe.hpp>

static boost::asio::io_service ioService;


void test_PipeCommunication()
{
  using namespace RF24::Physical::Sim;

  RXPipe rxPipe( ioService );
  TXPipe txPipe( ioService );


}