#include "pch.h"
#include "PixelSum.hpp"
#include <algorithm> // std::min, std::max

PixelSum::PixelSum(const unsigned char * buffer, int xWidth, int yHeight)
{
  int32_t nPixels = xWidth * yHeight;
  
  _xWidth = xWidth;
  _yHeight = yHeight;

  // Reserve space for the containers
  _buffer.resize(nPixels);
  _summedArea.resize(nPixels);
  _nonZeroTable.resize(nPixels);

  // Copy the buffer
  for (int32_t i = 0; i < nPixels; ++i)
  {
    _buffer[i] = buffer[i];
  }

  // Pre-calculate summed area table for O(1) pixel sums
  initializeSummedAreaTable();

  // Pre-calculate cumulative amount of zeroes in the buffer
  // for fast nonzero pixel count check
  initializeNonZeroTable();
}

PixelSum::~PixelSum(void)
{
  // Nothing to do here
}

PixelSum::PixelSum(const PixelSum & other)
{
  _xWidth = other._xWidth;
  _yHeight = other._yHeight;
  _buffer = other._buffer;
  _summedArea = other._summedArea;
  _nonZeroTable = other._nonZeroTable;
}

PixelSum& PixelSum::operator=(const PixelSum & other)
{
  _xWidth = other._xWidth;
  _yHeight = other._yHeight;
  _buffer = other._buffer;
  _summedArea = other._summedArea;
  _nonZeroTable = other._nonZeroTable;

  return *this;
}

// Implemented as in https://en.wikipedia.org/wiki/Summed-area_table
unsigned int PixelSum::getPixelSum(int x0, int y0, int x1, int y1) const
{
  // (1) order (x0,y0), (x1, y1) coordinates
  int32_t xMin = std::min(x0, x1);
  int32_t xMax = std::max(x0, x1);
  int32_t yMin = std::min(y0, y1);
  int32_t yMax = std::max(y0, y1);

  // Early exit if the other argument is beyond the top-left corner
  if (xMin <= 0 && yMin <= 0)
  {
    return summedArea(xMax, yMax);
  }

  // Make (x0,y0) inclusive. Otherwise it's not included
  int32_t x0_c = xMin - 1;
  int32_t y0_c = yMin - 1;

  // Calculate the four areas as explained in the wikipedia article
  return summedArea(xMax, yMax) + summedArea(x0_c, y0_c)
    - summedArea(xMax, y0_c) - summedArea(x0_c, yMax);
}

double PixelSum::getPixelAverage(int x0, int y0, int x1, int y1) const
{
  // Average is sum of pixels divided by number of pixels

  // There will never be division by zero because of abs and +1i
  int32_t nPixels = (abs(x1 - x0) + 1) * (abs(y1 - y0) + 1);
  int32_t pxsum = getPixelSum(x0, y0, x1, y1);
  return (double)pxsum / nPixels;
}

// Implemented as per in
// https://en.wikipedia.org/wiki/Summed-area_table
int PixelSum::getNonZeroCount(int x0, int y0, int x1, int y1) const
{
  // (1) order (x0,y0), (x1, y1) coordinates
  int32_t xMin = std::min(x0, x1);
  int32_t xMax = std::max(x0, x1);
  int32_t yMin = std::min(y0, y1);
  int32_t yMax = std::max(y0, y1);

  // Inclusive indices, same than in pixel sum
  int32_t x0_c = xMin - 1;
  int32_t y0_c = yMin - 1;
   
  // A,B,C,D as in the wikipedia article
  int32_t A = nNonZeroes(x0_c, y0_c);
  int32_t B = nNonZeroes(xMax, y0_c);
  int32_t C = nNonZeroes(x0_c, yMax);
  int32_t D = nNonZeroes(xMax, yMax);
  
  return A + D - B - C;
}

// Similarly than pixel sum average
double PixelSum::getNonZeroAverage(int x0, int y0, int x1, int y1) const
{
  int32_t nNonZeroPixels = getNonZeroCount(x0,y0,x1,y1);

  // There must be an early exit if there are no non-zero pixels
  if (nNonZeroPixels == 0)
  {
    return 0.0;
  }

  uint32_t nonZeroSum = getPixelSum(x0, y0, x1, y1);
  double nonZeroAverage = (double)nonZeroSum / (double)nNonZeroPixels;

  return nonZeroAverage;
}

uint8_t PixelSum::pixel(int32_t x, int32_t y) const
{
  return _buffer[x + y * _xWidth];
}

uint32_t PixelSum::summedArea(int32_t x, int32_t y) const
{
  // Out of bounds checks

  if (x < 0 || y < 0)
  {
    return 0LU;
  }

  int32_t xx = x;
  int32_t yy = y;
  if (x > _xWidth - 1)
  {
    xx = _xWidth - 1;
  }

  if (y > _yHeight - 1)
  {
    yy = _yHeight - 1;
  }

  return _summedArea[xx + yy * _xWidth];
}

int32_t PixelSum::nNonZeroes(int32_t x, int32_t y) const
{
  // out of bounds checks
  if (x < 0 || y < 0)
  {
    return 0LU;
  }

  int32_t xx = x;
  int32_t yy = y;
  if (x > _xWidth - 1)
  {
    xx = _xWidth - 1;
  }

  if (y > _yHeight - 1)
  {
    yy = _yHeight - 1;
  }
  return _nonZeroTable[xx + _xWidth * yy];
}

/*
Begin counting from top-left corner (0,0)
Each element is sum of all pixels above and left of current point.
Thus, we can be clever when initializing this table:

I(x,y) = i(x,y) + I(x,y-1) + I(x-1,y) - I(x-1,y-1),
where I(x,y) is summed area above and top-left for given pixel, inclusive
i(x,y) is pixel value of (x,y) from buffer

So this can be done in single pass over image
*/
void PixelSum::initializeSummedAreaTable(void)
{
  for (int32_t i = 0; i < _xWidth; ++i)
  {
    for (int32_t j = 0; j < _yHeight; ++j)
    {
      // Overindexing checks done inside summedArea so we don't need to worry about them here
        _summedArea[i + _xWidth * j] = pixel(i, j) +
        summedArea(i, j - 1) +
        summedArea(i - 1, j) -
        summedArea(i - 1, j - 1);
    }
  }
}

void PixelSum::initializeNonZeroTable(void)
{
  for (int32_t i = 0; i < _xWidth; ++i)
  {
    for (int32_t j = 0; j < _yHeight; ++j)
    {
      // Fill the table similarly than for summed area
      // Sorry bout the ternary. It checks if the  current pixel (i,j) is nonzero :)

      _nonZeroTable[i + _xWidth * j] = ((pixel(i, j) != 0) ? 1 : 0) +
        nNonZeroes(i, j - 1) +
        nNonZeroes(i - 1, j) -
        nNonZeroes(i - 1, j - 1);
    }
  }
}