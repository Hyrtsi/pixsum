#pragma once

#include <inttypes.h>
#include <vector>

//---------------------------------------------------------------------------- 
// Class for providing fast region queries from an 8-bit pixel buffer. 
// Note: all coordinates are *inclusive* and clamped internally to the borders 
// of the buffer by the implementation.  
// 

// For example: getPixelSum(4,8,7,10) gets the sum of a 4x3 region where top left 
// corner is located at (4,8) and bottom right at (7,10). In other words 
// all coordinates are _inclusive_. 
// 
// If the resulting region after clamping is empty, the return value for all 
// functions should be 0. 
// 
// The width and height of the buffer dimensions < 4096 x 4096. 
//---------------------------------------------------------------------------- 

constexpr int32_t C_MAX_BUFFER_WIDTH = 4096;
constexpr int32_t C_MAX_BUFFER_HEIGHT = 4096;

class PixelSum
{
public:
  PixelSum(const unsigned char* buffer, int xWidth, int
           yHeight);
  ~PixelSum(void);
  PixelSum(const PixelSum& other);
  PixelSum& operator=(const PixelSum& other);

  unsigned int    getPixelSum(int x0, int y0, int x1, int y1) const;
  double          getPixelAverage(int x0, int y0, int x1, int y1) const;

  int             getNonZeroCount(int x0, int y0, int x1, int y1) const;
  double          getNonZeroAverage(int x0, int y0, int x1, int y1) const;

private:
  int32_t _xWidth;
  int32_t _yHeight;

  // Get a pixel value from the buffer for given (x,y) coordinate
  uint8_t pixel(int32_t x, int32_t y) const;

  // Sum of all pixels above and to the left of (x,y) inclusive
  uint32_t summedArea(int32_t x, int32_t y) const;
  
  // How many pixels have a zero above and to the left of (x,y), inclusive
  int32_t nNonZeroes(int32_t x, int32_t y) const;

  // Setting up the lookup tables
  void initializeSummedAreaTable(void);
  void initializeNonZeroTable(void);

  std::vector<uint8_t> _buffer;
  std::vector<int32_t> _summedArea;
  std::vector<int32_t> _nonZeroTable;
};