#include <engine/Search.hpp>

#include <gtest/gtest.h>

using namespace chess;
using namespace std;

namespace { // internal only

class SearchTests : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		constants_initialize();
	}
};

TEST_F(SearchTests, TestThatMiniMax_Bla)
{
}

} //anonymous namespace
