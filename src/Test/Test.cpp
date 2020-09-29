#include "Test.h"

void Test::SetUp()
{
    m_tm.addMarker(0, 0);
    m_tm.addMarker(1, 5);
    m_tm.addMarker(13, 8);
    m_tm.setEndTempo(10);
}

TEST_F(Test, BeforeFirstMarker)
{
    const auto sample = m_tm.sample(-3);
    const auto beat = m_tm.beat(-6);
    EXPECT_DOUBLE_EQ(sample, -15);
    EXPECT_DOUBLE_EQ(beat, -1.2);
}

TEST_F(Test, BetweenMarkers1)
{
    const auto sample = m_tm.sample(0.5);
    const auto beat = m_tm.beat(3);
    EXPECT_DOUBLE_EQ(sample, 2.5);
    EXPECT_DOUBLE_EQ(beat, 0.6);
}

TEST_F(Test, BetweenMarkers2)
{
    const auto sample = m_tm.sample(11);
    const auto beat = m_tm.beat(6.25);
    EXPECT_DOUBLE_EQ(sample, 7.5);
    EXPECT_DOUBLE_EQ(beat, 6);
}

TEST_F(Test, PastLastMarker)
{
    const auto sample = m_tm.sample(22);
    const auto beat = m_tm.beat(14);
    EXPECT_DOUBLE_EQ(sample, 8.9);
    EXPECT_DOUBLE_EQ(beat, 73);
}
