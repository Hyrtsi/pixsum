#include "pch.h"
#include "../PixelSum/PixelSum.hpp"

static uint8_t * initializeBuffer(int32_t w, int32_t h);
//static destroyBuffer(uint8_t* buffer);

TEST(PixelSumTest, SinglePixels) {
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_EQ(0, px.getPixelSum(0, 0, 0, 0));
  EXPECT_EQ(0, px.getPixelSum(-5,-5,-5,-5));
  EXPECT_EQ(0, px.getPixelSum(5,5,5,5));
  EXPECT_EQ(0, px.getPixelSum(5000,5000,5000,5000));
  EXPECT_EQ(4, px.getPixelSum(1,0,1,0));
}

TEST(PixelSumTest, RegionsInBounds)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_EQ(7, px.getPixelSum(0, 0, 5, 0));
  EXPECT_EQ(7, px.getPixelSum(0, 0, 4, 0));
  EXPECT_EQ(6, px.getPixelSum(0, 0, 3, 0));
  EXPECT_EQ(3, px.getPixelSum(2, 0, 5, 0));
  EXPECT_EQ(3, px.getPixelSum(2, 0, 5, 5));
  EXPECT_EQ(4, px.getPixelSum(0, 0, 1, 0));
  EXPECT_EQ(0, px.getPixelSum(0, 0, 0, 1));
}

TEST(PixelSumTest, RegionsOutBounds)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_EQ(0, px.getPixelSum(-100,-100, -50,-50));
  EXPECT_EQ(7, px.getPixelSum(-100, 0, 4, 0));
  EXPECT_EQ(6, px.getPixelSum(0, 0, 3, 5000));
  EXPECT_EQ(0, px.getPixelSum(2048, 2048, 5000, 5000));
}

TEST(PixelSumTest, FlippedLimits)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_EQ(px.getPixelSum(0, 0, 5, 0), px.getPixelSum(5, 0, 0, 0));
  EXPECT_EQ(px.getPixelSum(-5, 10, 500, 25), px.getPixelSum(500, 25, -5, 10));
  EXPECT_EQ(px.getPixelSum(1,2,3,4), px.getPixelSum(3,4,1,2));
  EXPECT_EQ(px.getPixelSum(800, 5000, 5, 0), px.getPixelSum(5, 0, 800, 5000));
}

TEST(PixelAverageTest, SinglePixels)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_DOUBLE_EQ(0.0, px.getPixelAverage(0,0,0,0));
  EXPECT_DOUBLE_EQ(4.0, px.getPixelAverage(1, 0, 1, 0));
  EXPECT_DOUBLE_EQ(1.0, px.getPixelAverage(4, 0, 4, 0));
  EXPECT_DOUBLE_EQ(0.0, px.getPixelAverage(-5, 0, -5, 0));
  EXPECT_DOUBLE_EQ(0.0, px.getPixelAverage(5000, 0, 5000, 0));
}

TEST(PixelAverageTest, InsideBounds)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_DOUBLE_EQ(2.0, px.getPixelAverage(0, 0, 1, 0));
  EXPECT_DOUBLE_EQ((4.0/3.0), px.getPixelAverage(0, 0, 2, 0));
  EXPECT_DOUBLE_EQ(1.50, px.getPixelAverage(0, 0, 3, 0));
  EXPECT_DOUBLE_EQ(1.40, px.getPixelAverage(0, 0, 4, 0));
  EXPECT_DOUBLE_EQ((7.0/6.0), px.getPixelAverage(0, 0, 5, 0));
  EXPECT_DOUBLE_EQ(1.0, px.getPixelAverage(0, 0, 6, 0));
}

TEST(PixelAverageTest, FlippedLimits)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_DOUBLE_EQ(px.getPixelAverage(1, 0, 0, 0), px.getPixelAverage(0, 0, 1, 0));
  EXPECT_DOUBLE_EQ(px.getPixelAverage(-1, 0, 0, 0), px.getPixelAverage(0, 0, -1, 0));
  EXPECT_DOUBLE_EQ(px.getPixelAverage(5000,0,0,0), px.getPixelAverage(0, 0, 5000, 0));
  EXPECT_DOUBLE_EQ(px.getPixelAverage(1, -1, 0, 0), px.getPixelAverage(0, 0, 1, -1));
}

TEST(PixelAverageTest, OutsideBounds)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_DOUBLE_EQ((7.0 / 121.0), px.getPixelAverage(-5, -5, 5, 5));
  EXPECT_DOUBLE_EQ(0.0, px.getPixelAverage(-5, -5, -1, -1));
  EXPECT_DOUBLE_EQ(0.0, px.getPixelAverage(7000, 3000, 8000, 3500));
  EXPECT_DOUBLE_EQ((3.0 / (98 * 2.0)), px.getPixelAverage(3, 0, 100, 1));
}


TEST(NonZeroCountTest, Basic)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_EQ(3, px.getNonZeroCount(0,0,5,0));
  EXPECT_EQ(3, px.getNonZeroCount(0,0,10,0));
  EXPECT_EQ(1, px.getNonZeroCount(0,0,1,0));
  EXPECT_EQ(2, px.getNonZeroCount(3,0,6,0));
  EXPECT_EQ(3, px.getNonZeroCount(0,0,5,5));
}

TEST(NonZeroCountTest, FlippedInput)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_EQ(px.getNonZeroCount(5, 0, 0, 1), px.getNonZeroCount(0,1, 5, 0));
  EXPECT_EQ(px.getNonZeroCount(-1,5,0,8), px.getNonZeroCount(0, 8,-1,5));
  EXPECT_EQ(px.getNonZeroCount(0,25,0,5), px.getNonZeroCount(0, 5,0,25));
  EXPECT_EQ(px.getNonZeroCount(2500,7000,0,1000), px.getNonZeroCount(0, 1000,2500,7000));
}

TEST(NonZeroCountTest, SinglePixels)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_EQ(0, px.getNonZeroCount(0, 0, 0, 0));
  EXPECT_EQ(1, px.getNonZeroCount(1, 0, 1, 0));
  EXPECT_EQ(0, px.getNonZeroCount(-1, -1, -1, -1));
  EXPECT_EQ(0, px.getNonZeroCount(5000, 3000, 5000, 3000));
}

TEST(NonZeroAverageTest, Basic)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_DOUBLE_EQ((7.0 / 3.0), px.getNonZeroAverage(0, 0, 5, 0));
  EXPECT_DOUBLE_EQ((7.0 / 3.0), px.getNonZeroAverage(-1, 0, 5, 0));
  EXPECT_DOUBLE_EQ((7.0 / 3.0), px.getNonZeroAverage(-5, -5, 5, 0));
  EXPECT_DOUBLE_EQ((7.0 / 3.0), px.getNonZeroAverage(0, 0, 50, 0));
  EXPECT_DOUBLE_EQ((7.0 / 3.0), px.getNonZeroAverage(0, 0, 5, 10));
  EXPECT_DOUBLE_EQ((7.0 / 3.0), px.getNonZeroAverage(1, 0, 5, 0));
}

TEST(NonZeroAverageTest, Flipped)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_DOUBLE_EQ(px.getNonZeroAverage(5, 0, 0, 0), px.getNonZeroAverage(0, 0, 5, 0));
  EXPECT_DOUBLE_EQ(px.getNonZeroAverage(5, 0, -1, 0), px.getNonZeroAverage(-1, 0, 5, 0));
  EXPECT_DOUBLE_EQ(px.getNonZeroAverage(2800, 6000, 2500, 5000), px.getNonZeroAverage(2500,5000,2800,6000));
}

TEST(NonZeroAverageTest, SinglePix)
{
  const int32_t w = 2048;
  const int32_t h = 2048;
  uint8_t* buffer = initializeBuffer(w, h);
  PixelSum px(buffer, w, h);
  delete[] buffer;

  EXPECT_DOUBLE_EQ(1.0, px.getNonZeroAverage(4, 0, 4, 0));
  EXPECT_DOUBLE_EQ(0.0, px.getNonZeroAverage(0, 0, 0, 0));
  EXPECT_DOUBLE_EQ(0.0, px.getNonZeroAverage(-5, -5, -5, -5));
  EXPECT_DOUBLE_EQ(0.0, px.getNonZeroAverage(8000, 6000, 8000, 6000));

}

// Setting up the data

uint8_t * initializeBuffer(int32_t w, int32_t h)
{
  uint8_t* buffer = new uint8_t[w*h]{ 0U };

  buffer[0] = 0U;
  buffer[1] = 4U;
  buffer[2] = 0U;
  buffer[3] = 2U;
  buffer[4] = 1U;
  buffer[5] = 0U;

  return buffer;
}