#include <iostream>
#include <cmath>
#include <cstdlib>

#include "uiuc/PNG.h"
#include "uiuc/HSLAPixel.h"
#include "ImageTransform.h"

/* ******************
(Begin multi-line comment...)

Write your name and email address in the comment space here:

Name:
Email:

(...end multi-line comment.)
******************** */

using uiuc::PNG;
using uiuc::HSLAPixel;
using uiuc::Illini;
using uiuc::ComplementaryColorScheme;

const int MAXIMUM_PIXEL_AWAY = 160;
const double LUMINANCE_DECREMENT_FACTOR_MORE_THAN_MAX_PIXEL_AWAY = 0.8;
const double LUMINANCE_DECREMENT_FACTOR_LESS_THAN_MAX_PIXEL_AWAY = 0.005;
const double LUMINANCE_INCREMENT_FACTOR = 0.2;
const double MAXIMUM_LUMINANCE = 1.0;

/**
 * Returns an image that has been transformed to grayscale.
 *
 * The saturation of every pixel is set to 0, removing any color.
 *
 * @return The grayscale image.
 */
PNG grayscale(PNG image) {
  /// This function is already written for you so you can see how to
  /// interact with our PNG class.
  for (unsigned x = 0; x < image.width(); x++) {
    for (unsigned y = 0; y < image.height(); y++) {
      HSLAPixel& pixel = image.getPixel(x, y);

      // `pixel` is a reference to the memory stored inside of the PNG `image`,
      // which means you're changing the image directly. No need to `set`
      // the pixel since you're directly changing the memory of the image.
      pixel.s = 0;
    }
  }

  return image;
}

/**
 * Returns eucledian distance of given coordinates and its respective center.
 *
 * @param x
 * @param y
 * @param centerX
 * @param centerY
 *
 * @return eucledian distance
 */
inline double eucledianDistanceOf(
  const unsigned& x, const unsigned& y,
  const int& centerX, const int& centerY) {
  auto newX = x - centerX;
  auto newY = y - centerY;
  return sqrt((newX * newX) + (newY * newY));
}

inline void updateLuminanceBasedOnDistance(
  HSLAPixel* pixel, const unsigned& x, const unsigned& y,
  const int& centerX, const int& centerY) {
  auto eucledianDistance = eucledianDistanceOf(x, y, centerX, centerY);
  auto luminanceFactor = 0.0;
  if (eucledianDistance < MAXIMUM_PIXEL_AWAY) {
    luminanceFactor = LUMINANCE_DECREMENT_FACTOR_LESS_THAN_MAX_PIXEL_AWAY * eucledianDistance * pixel->l;
  } else {
    luminanceFactor = LUMINANCE_DECREMENT_FACTOR_MORE_THAN_MAX_PIXEL_AWAY * pixel->l;
  }
  pixel->l = pixel->l - luminanceFactor;
}

/**
 * Returns an image with a spotlight centered at (`centerX`, `centerY`).
 *
 * A spotlight adjusts the luminance of a pixel based on the distance the pixel
 * is away from the center by decreasing the luminance by 0.5% per 1 pixel euclidean
 * distance away from the center.
 *
 * For example, a pixel 3 pixels above and 4 pixels to the right of the center
 * is a total of `sqrt((3 * 3) + (4 * 4)) = sqrt(25) = 5` pixels away and
 * its luminance is decreased by 2.5% (0.975x its original value).  At a
 * distance over 160 pixels away, the luminance will always decreased by 80%.
 *
 * The modified PNG is then returned.
 *
 * @param image A PNG object which holds the image data to be modified.
 * @param centerX The center x coordinate of the crosshair which is to be drawn.
 * @param centerY The center y coordinate of the crosshair which is to be drawn.
 *
 * @return The image with a spotlight.
 */
PNG createSpotlight(PNG image, int centerX, int centerY) {
  for (unsigned x = 0; x < image.width(); x++) {
    for (unsigned y = 0; y < image.height(); y++) {
      auto pixel = &image.getPixel(x, y);
      updateLuminanceBasedOnDistance(pixel, x, y, centerX, centerY);
    }
  }
  return image;
}

/**
 * Check if hue is within complementary colour scheme.
 *
 * @param h Hue of a pixel.
 *
 * @return if the pixel with certain hue is within complementary colour scheme.
 */
inline bool withinComplementaryColorScheme(const unsigned& h) {
  return h >= ComplementaryColorScheme::DarkSeaGreen
    && h < ComplementaryColorScheme::DarkGray;
}

/**
 * Get Illini colour of a pixel with certain hue.
 *
 * @param h Hue of a pixel.
 *
 * @return Illini colour of the pixel.
 */
inline int getIlliniColourBasedOnHue(const unsigned& h) {
  if (withinComplementaryColorScheme(h)) {
    return Illini::BLUE;
  }
  return Illini::ORANGE;
}

inline void updatePixelWithIlliniColours(HSLAPixel* pixel) {
  pixel->h = getIlliniColourBasedOnHue(pixel->h);
  return;
}

/**
 * Returns a image transformed to Illini colors.
 *
 * The hue of every pixel is set to the a hue value of either orange or
 * blue, based on if the pixel's hue value is closer to orange than blue.
 *
 * @param image A PNG object which holds the image data to be modified.
 *
 * @return The illinify'd image.
**/
PNG illinify(PNG image) {
  for (unsigned x = 0; x < image.width(); x++) {
    for (unsigned y = 0; y < image.height(); y++) {
      auto pixel = &image.getPixel(x, y);
      updatePixelWithIlliniColours(pixel);
    }
  }
  return image;
}

inline void updatePixelWithStencil(HSLAPixel* base, HSLAPixel* stencil) {
  const auto& luminanceIsMaxedOut = stencil->l == MAXIMUM_LUMINANCE;
  const auto& withinLimit = base->l + LUMINANCE_INCREMENT_FACTOR < MAXIMUM_LUMINANCE;
  if (luminanceIsMaxedOut && withinLimit) {
    base->l = base->l + LUMINANCE_INCREMENT_FACTOR;
  }
  return;
}


/**
* Returns an immge that has been watermarked by another image.
*
* The luminance of every pixel of the second image is checked, if that
* pixel's luminance is 1 (100%), then the pixel at the same location on
* the first image has its luminance increased by 0.2.
*
* @param firstImage  The first of the two PNGs, which is the base image.
* @param secondImage The second of the two PNGs, which acts as the stencil.
*
* @return The watermarked image.
*/
PNG watermark(PNG firstImage, PNG secondImage) {
  for (unsigned x = 0; x < firstImage.width(); x++) {
    for (unsigned y = 0; y < firstImage.height(); y++) {
      auto pixel = &firstImage.getPixel(x, y);
      auto stencil = &secondImage.getPixel(x, y);
      updatePixelWithStencil(pixel, stencil);
    }
  }
  return firstImage;
}
