#pragma once

#include <gtest/gtest.h>
#include <TimeMapper/TimeMapper.h>

class Test : public ::testing::Test
{
protected:
    void SetUp() override;

    TimeMapper m_tm;
};
