/********************************************************************************
*  File Name:
*    test_conversion.cpp
*
*  Description:
*    Tests the conversion functions for the RF24Node library
*
*  2019-2020 | Brandon Braun | brandonbraun653@gmail.com
********************************************************************************/

/* STL Includes */
#include <algorithm>
#include <vector>

/* GTest Includes */
#include "gtest/gtest.h"

/* RF24 Includes */
#include <RF24Node/src/common/conversion.hpp>

using namespace RF24::Physical::Conversion;
using namespace RF24::Hardware;

static constexpr uint16_t BASE_PORT = 13000;
static constexpr RF24::PhysicalAddress INVALID_PHY_ADDR = std::numeric_limits<RF24::PhysicalAddress>::max();

#define GET_NET_PORT( id, pipe ) ( BASE_PORT + ( ( id ) * ( RF24::Hardware::MAX_NUM_PIPES + 1 ) ) + static_cast<uint16_t>( ( pipe ) ) )

#if defined( RF24_SIMULATOR )
TEST( Encode, NormalInput )
{
  EXPECT_GE( encodeAddress( "127.0.0.1", 8008 ), 0 );
}



struct EncodeDecodeTest
{
  RF24::LogicalAddress address;     /**< The address of the node under test */
  RF24::Hardware::PipeNumber pipe;  /**< Pipe on the node under test*/
  std::string ip;                   /**< Expected IP Address of the node under test */
  uint16_t port;                    /**< Expected Port of the node under test */
};

/* clang-format off */
static std::vector<EncodeDecodeTest> CodecTestCandidates = {
/* Address                Pipe Number                      IP Address                                   Port                              */     
  { 00000,    RF24::Hardware::PipeNumber::PIPE_NUM_0,     "127.0.0.1",      GET_NET_PORT( 00000, RF24::Hardware::PipeNumber::PIPE_NUM_0 ) },
  { 00000,    RF24::Hardware::PipeNumber::PIPE_NUM_1,     "127.0.0.1",      GET_NET_PORT( 00000, RF24::Hardware::PipeNumber::PIPE_NUM_1 ) },
};
/* clang-format on */


/*------------------------------------------------
This test ensures that encoded ports and ip addresses can
successfully be deconstructed back to the original data.
------------------------------------------------*/
//TEST( Encode, SIM_EncodeDecodeRecode )
//{
//  for ( EncodeDecodeTest& test : CodecTestCandidates )
//  {
//    auto encodedData = encodeAddress( test.ip, test.port );
//
//    EXPECT_EQ( decodeIP( encodedData ), test.ip );
//    EXPECT_EQ( decodePort( encodedData ), test.port );
//  }
//}

/*------------------------------------------------
This test validates that all generated physical addresses
are unique across the entire logical address range.
------------------------------------------------*/
//TEST( Encode, SIM_AddressingUniqueness )
//{
//  std::vector<RF24::Port> generatedPorts;
//  std::vector<RF24::PhysicalAddress> generatedAddrs;
//  
//  auto constexpr minAddress = std::numeric_limits<RF24::LogicalAddress>::min();
//  auto constexpr maxAddress = std::numeric_limits<RF24::LogicalAddress>::max();
//
//  auto constexpr minPipe = static_cast<size_t>( PIPE_NUM_0 );
//  auto constexpr maxPipe = static_cast<size_t>( PIPE_NUM_5 );
//
//  /* Generate the addresses and ports */
//  for ( auto addr = minAddress; addr < maxAddress; addr++ )
//  {
//    for ( auto pipe = minPipe; pipe <= maxPipe; pipe++ )
//    {
//      auto phyAddrCandidate = getPhysicalAddress( addr, static_cast<RF24::Hardware::PipeNumber>( pipe ) );
//
//      if ( phyAddrCandidate != INVALID_PHY_ADDR )
//      {
//        generatedAddrs.push_back( phyAddrCandidate );
//        generatedPorts.push_back( decodePort( phyAddrCandidate ) );
//      }
//    }
//  }
//
//  /* Sort the data in prep for finding the duplicates */
//  std::sort( generatedPorts.begin(), generatedPorts.end() );
//  std::sort( generatedAddrs.begin(), generatedAddrs.end() );
//
//  /* If no duplicates are found, the returned iterator will equal the end of the vector */
//  auto duplicateIterator = std::unique( generatedPorts.begin(), generatedPorts.end() );
//  EXPECT_EQ( duplicateIterator, generatedPorts.end() );
// 
//
//
//  EXPECT_EQ( std::unique( generatedAddrs.begin(), generatedAddrs.end() ), generatedAddrs.end() );
//}



#endif  /* RF24_SIMULATOR */