/**
 * @file HSLAPixel.h
 *
 * @author University of Illinois CS 225 Course Staff
 * @version 2018r1-lab1 - Updated for CS 400
 */

#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>

namespace uiuc {

  /**
   * @tparam h storing the hue of the pixel in degrees between 0 and 360 using a double
   * @tparam s storing the saturation of the pixel as a decimal value between 0.0 and 1.0 using a double
   * @tparam l storing the luminance of the pixel as a decimal value between 0.0 and 1.0 using a double
   * @tparam a storing the alpha channel (blending opacity) as a decimal value between 0.0 and 1.0 using a double
   */
  class HSLAPixel {
  public:
    double h;
    double s;
    double l;
    double a;
  };

  enum Illini {
    ORANGE = 11,
    BLUE = 216
  };

  enum ComplementaryColorScheme {
    DarkGray = 281,
    DarkSeaGreen = 101
  };

}
