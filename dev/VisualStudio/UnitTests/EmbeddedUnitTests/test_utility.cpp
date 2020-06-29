/********************************************************************************
*  File Name:
*    test_utility.cpp
*
*  Description:
*    Tests the utility functions for the RF24Node library
*
*  2019-2020 | Brandon Braun | brandonbraun653@gmail.com
********************************************************************************/

/* GTest Includes */
#include "gtest/gtest.h"

/* C++ Includes */
#include <vector>

/* RF24 Includes */
#include <RF24Node/src/common/definitions.hpp>
#include <RF24Node/src/common/types.hpp>
#include <RF24Node/src/common/utility.hpp>
#include <RF24Node/src/network/definitions.hpp>

using namespace RF24;

/*-------------------------------------------------------------------------------
Test Vector Structures
-------------------------------------------------------------------------------*/
struct AddressTestSuite
{
  LogicalAddress address;
  LogicalAddress parent;
  LogicalLevel level;
  bool isAChild;
  bool isARoot;
  bool isValid;
  bool isReserved;
};

struct DescendantTestSuite
{
  LogicalAddress parent;
  LogicalAddress child;
  bool isDescendant;
  bool isDirect;
};

struct LevelIDTestSuite
{
  LogicalAddress address;
  LogicalID idAtLevel0;
  LogicalID idAtLevel1;
  LogicalID idAtLevel2;
  LogicalID idAtLevel3;
  LogicalID idAtLevel4;
  LogicalID idAtLevel5;
};

struct LevelAddressTestSuite
{
  LogicalAddress address;
  LogicalLevel level;
  LogicalAddress expected;
};

/*-------------------------------------------------------------------------------
Test Data
-------------------------------------------------------------------------------*/
/*------------------------------------------------
Note: For all the literal addresses below, prepending
with a zero forces the numbers to be in octal format.
------------------------------------------------*/
/* clang-format off */
static std::vector<AddressTestSuite> PropertyTestCandidates = {
  /*Address                                  Parent                  Level          Child      Root    Validity   Rsvd  */
  { 006666,                       Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,    false },  // Completely invalid nodes
  { 007001,                       Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,    false },
  { 000701,                       Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,    false },
  { 000071,                       Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,    false },
  { 000007,                       Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,    false },
  { 001536,                       Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,    false },
  { 002371,                       Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,    false },
  { 003722,                       Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,    false },
  { 006243,                       Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,    false },
  { 000000,                       Network::RSVD_ADDR_INVALID,                   0,  false,     true,     true,    false },  // Root nodes
  { 077770,                       Network::RSVD_ADDR_INVALID,                   0,  false,     true,     true,    false },
  { 007770,                       Network::RSVD_ADDR_INVALID,                   0,  false,     true,     true,    false },
  { 001000,                       Network::RSVD_ADDR_INVALID,                   0,  false,     true,     true,    false },
  { 000200,                       Network::RSVD_ADDR_INVALID,                   0,  false,     true,     true,    false },
  { 000030,                       Network::RSVD_ADDR_INVALID,                   0,  false,     true,     true,    false },
  { 000001,                                  RF24::RootNode0,                   1,   true,    false,     true,    false },  // Child nodes
  { 000002,                                  RF24::RootNode0,                   1,   true,    false,     true,    false },
  { 000003,                                  RF24::RootNode0,                   1,   true,    false,     true,    false },
  { 000004,                                  RF24::RootNode0,                   1,   true,    false,     true,    false },
  { 000005,                                  RF24::RootNode0,                   1,   true,    false,     true,    false },
  { 000023,                                           000003,                   2,   true,    false,     true,    false },
  { 000135,                                           000035,                   3,   true,    false,     true,    false },
  { 001234,                                           000234,                   4,   true,    false,     true,    false },
  { 005555,                                           000555,                   4,   true,    false,     true,    false },
  { 055555,                                           005555,                   5,   true,    false,     true,    false },
  { Network::RSVD_ADDR_MULTICAST, Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,     true },  // Reserved nodes
  { Network::RSVD_ADDR_ROUTED,    Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,     true },
  { Network::RSVD_ADDR_INVALID,   Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,     true },
  { Network::RSVD_ADDR_LOOKUP,    Network::RSVD_ADDR_INVALID,  NODE_LEVEL_INVALID,  false,    false,    false,     true },
};

static std::vector<LevelIDTestSuite> LevelIDTestCandidates = {
  /*Address       L0                 L1                  L2                  L3                  L4                  L5         */
  { 077777,  NODE_ID_INVALID,   NODE_ID_INVALID,    NODE_ID_INVALID,    NODE_ID_INVALID,    NODE_ID_INVALID,    NODE_ID_INVALID },
  { 066666,  NODE_ID_INVALID,   NODE_ID_INVALID,    NODE_ID_INVALID,    NODE_ID_INVALID,    NODE_ID_INVALID,    NODE_ID_INVALID },
  { 055555,  NODE_ID_INVALID,         NODE_ID_5,          NODE_ID_5,          NODE_ID_5,          NODE_ID_5,          NODE_ID_5 },
  { 044444,  NODE_ID_INVALID,         NODE_ID_4,          NODE_ID_4,          NODE_ID_4,          NODE_ID_4,          NODE_ID_4 },
  { 033333,  NODE_ID_INVALID,         NODE_ID_3,          NODE_ID_3,          NODE_ID_3,          NODE_ID_3,          NODE_ID_3 },
  { 022222,  NODE_ID_INVALID,         NODE_ID_2,          NODE_ID_2,          NODE_ID_2,          NODE_ID_2,          NODE_ID_2 },
  { 011111,  NODE_ID_INVALID,         NODE_ID_1,          NODE_ID_1,          NODE_ID_1,          NODE_ID_1,          NODE_ID_1 },
  { 000000,     NODE_ID_ROOT,   NODE_ID_INVALID,    NODE_ID_INVALID,    NODE_ID_INVALID,    NODE_ID_INVALID,    NODE_ID_INVALID },
  { 054320,     NODE_ID_ROOT,   NODE_ID_INVALID,          NODE_ID_2,          NODE_ID_3,          NODE_ID_4,          NODE_ID_5 },
  { 065432,  NODE_ID_INVALID,         NODE_ID_2,          NODE_ID_3,          NODE_ID_4,          NODE_ID_5,    NODE_ID_INVALID }
};

static std::vector<DescendantTestSuite> DescendantTestCandidates = {
  /*Parent                        Child   Descendant   Direct*/
  { 006666,                       000000,    false,    false },  // Completely invalid nodes
  { 007001,                       000000,    false,    false },
  { 000701,                       000000,    false,    false },
  { 000071,                       000000,    false,    false },
  { 000007,                       000000,    false,    false },
  { 001536,                       000000,    false,    false },
  { 002371,                       000000,    false,    false },
  { 003722,                       000000,    false,    false },
  { 006243,                       000000,    false,    false },
  { Network::RSVD_ADDR_MULTICAST, 000000,    false,    false },  // Reserved nodes
  { Network::RSVD_ADDR_ROUTED,    000000,    false,    false },
  { Network::RSVD_ADDR_INVALID,   000000,    false,    false },
  { Network::RSVD_ADDR_LOOKUP,    000000,    false,    false },
  { 077770,                       000001,     true,    false },  // Root nodes
  { 007770,                       000001,     true,    false },
  { 001000,                       000001,     true,    false },
  { 000200,                       000001,     true,    false },
  { 000030,                       000001,     true,    false },
  { 000001,                       052341,     true,    false },  // Indirect descendant
  { 000011,                       042311,     true,    false },
  { 000111,                       034111,     true,    false },
  { 000234,                       022234,     true,    false },
  { 000000,                       000002,     true,     true },
  { 000000,                       000024,     true,    false },
  { 000000,                       000424,     true,    false },
  { 000000,                       003424,     true,    false },
  { 000000,                       053424,     true,    false },
  { 001111,                       021111,     true,     true },  // Direct descendant
  { 000425,                       002425,     true,     true },
  { 000013,                       000313,     true,     true },
  { 000004,                       000054,     true,     true },
  { 000000,                       000001,     true,     true },
  { 011111,                       011111,    false,    false },  // Identical
  { 001234,                       001234,    false,    false },
  { 000435,                       000435,    false,    false },
  { 000025,                       000025,    false,    false },
  { 000003,                       000003,    false,    false }
};
/* clang-format on */

/*-------------------------------------------------------------------------------
Tests
-------------------------------------------------------------------------------*/
TEST( Addressing, PropertyTests )
{
  for ( AddressTestSuite& test : PropertyTestCandidates )
  {
    EXPECT_EQ( isAddressChild( test.address ), test.isAChild );
    EXPECT_EQ( isAddressRoot( test.address ), test.isARoot );
    EXPECT_EQ( isAddressValid( test.address ), test.isValid );
    EXPECT_EQ( isAddressReserved( test.address ), test.isReserved );
    EXPECT_EQ( getParent( test.address ), test.parent );
    EXPECT_EQ( getLevel( test.address ), test.level );
  }
}

TEST( Addressing, LevelIDTests )
{
  for ( LevelIDTestSuite& test : LevelIDTestCandidates )
  {
    EXPECT_EQ( getIdAtLevel( test.address, 0 ), test.idAtLevel0 );
    EXPECT_EQ( getIdAtLevel( test.address, 1 ), test.idAtLevel1 );
    EXPECT_EQ( getIdAtLevel( test.address, 2 ), test.idAtLevel2 );
    EXPECT_EQ( getIdAtLevel( test.address, 3 ), test.idAtLevel3 );
    EXPECT_EQ( getIdAtLevel( test.address, 4 ), test.idAtLevel4 );
    EXPECT_EQ( getIdAtLevel( test.address, 5 ), test.idAtLevel5 );
  }
}

TEST( Addressing, DescendantTests )
{
  for ( DescendantTestSuite& test : DescendantTestCandidates )
  {
    const auto parent = test.parent;
    const auto child = test.child;

    EXPECT_EQ( isDescendent( parent, child ), test.isDescendant );
    EXPECT_EQ( isDirectDescendent( parent, child ), test.isDirect );
  }
}

TEST( Addressing, AddressAtLevel )
{
  using namespace RF24::Network;

  /* clang-format off */
  std::vector<LevelAddressTestSuite> testVector = {
    /* Address                Level             Expected     */
    { RootNode0,          NODE_LEVEL_0,    RootNode0         }, // Root nodes are always just themselves
    { RootNode0,          NODE_LEVEL_1,    RSVD_ADDR_INVALID },
    { RootNode0,          NODE_LEVEL_2,    RSVD_ADDR_INVALID },
    { RootNode0,          NODE_LEVEL_3,    RSVD_ADDR_INVALID },
    { RootNode0,          NODE_LEVEL_4,    RSVD_ADDR_INVALID },
    { RootNode0,          NODE_LEVEL_5,    RSVD_ADDR_INVALID },
    
    { RSVD_ADDR_INVALID,  NODE_LEVEL_0,    RSVD_ADDR_INVALID }, // Invalid nodes don't have addresses at any level
    { RSVD_ADDR_INVALID,  NODE_LEVEL_1,    RSVD_ADDR_INVALID },
    { RSVD_ADDR_INVALID,  NODE_LEVEL_2,    RSVD_ADDR_INVALID },
    { RSVD_ADDR_INVALID,  NODE_LEVEL_3,    RSVD_ADDR_INVALID },
    { RSVD_ADDR_INVALID,  NODE_LEVEL_4,    RSVD_ADDR_INVALID },
    { RSVD_ADDR_INVALID,  NODE_LEVEL_5,    RSVD_ADDR_INVALID },
    { 067247,             NODE_LEVEL_0,    RSVD_ADDR_INVALID },
    { 067247,             NODE_LEVEL_1,    RSVD_ADDR_INVALID },
    { 067247,             NODE_LEVEL_2,    RSVD_ADDR_INVALID },
    { 067247,             NODE_LEVEL_3,    RSVD_ADDR_INVALID },
    { 067247,             NODE_LEVEL_4,    RSVD_ADDR_INVALID },
    { 067247,             NODE_LEVEL_5,    RSVD_ADDR_INVALID },
    
    { 054321,             NODE_LEVEL_0,    RSVD_ADDR_INVALID }, // Fully qualified address at all levels
    { 054321,             NODE_LEVEL_1,    01                },
    { 054321,             NODE_LEVEL_2,    021               },
    { 054321,             NODE_LEVEL_3,    0321              },
    { 054321,             NODE_LEVEL_4,    04321             },
    { 054321,             NODE_LEVEL_5,    054321            },

    { 000321,             NODE_LEVEL_0,    RSVD_ADDR_INVALID }, // Partially qualified address
    { 000321,             NODE_LEVEL_1,    01                },
    { 000321,             NODE_LEVEL_2,    021               },
    { 000321,             NODE_LEVEL_3,    0321              },
    { 000321,             NODE_LEVEL_4,    RSVD_ADDR_INVALID },
    { 000321,             NODE_LEVEL_5,    RSVD_ADDR_INVALID },
  };
  /* clang-format on */

  for ( LevelAddressTestSuite& test : testVector )
  {
    EXPECT_EQ( getAddressAtLevel( test.address, test.level ), test.expected );
  }
}
