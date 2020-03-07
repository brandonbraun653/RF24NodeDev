/********************************************************************************
*  File Name:
*    test_conversion.cpp
*
*  Description:
*    Tests the conversion functions for the RF24Node library
*
*  2019 | Brandon Braun | brandonbraun653@gmail.com
********************************************************************************/

/* GTest Includes */
#include "gtest/gtest.h"

/* RF24 Includes */
#include <RF24Node/src/common/conversion.hpp>

using namespace RF24::Physical::Conversion;

TEST( Encode, NormalInput )
{
  EXPECT_GE( encodeAddress( "127.0.0.1", 8008 ), 0 );
}

TEST( Encode, EncodeDecodeRecode )
{
  const std::string originalIP = "127.0.0.1";
  const uint16_t originalPort = 1234;

  auto encodedData = encodeAddress( originalIP, originalPort );

  EXPECT_EQ( decodeIP( encodedData ), originalIP );
  EXPECT_EQ( decodePort( encodedData ), originalPort );
}

