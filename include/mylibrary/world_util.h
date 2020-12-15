//
// Created by nishu on 4/22/2020.
//

#ifndef FINALPROJECT_WORLD_UTIL_H
#define FINALPROJECT_WORLD_UTIL_H

#include <Box2D/Box2D.h>

#include "tile.h"

namespace scrabble {
  class WorldUtil {

   private:
    constexpr static const float kPixelToMeter = 0.01f;

   public:

    /**
     * Converts a vector from the physics world to the game world
     * @param vec the vector to be converted in meters
     * @return the vector converted into pixels
     */
    static b2Vec2 meterToPixel(b2Vec2 vec) {
      return (1.0f / kPixelToMeter) * vec;
    }

    /**
     * Converts a vector from the game world to the physics world
     * @param vec the vector to be converted in pixels
     * @return the vector converted to meters
     */
    static b2Vec2 pixelToMeter(b2Vec2 vec) {
      return kPixelToMeter * vec;
    }

    /**
     * Checks if the point is within a bound
     * @param check_x the x-point
     * @param check_y the y-point
     * @param top_left_x the top left x value of the bound
     * @param bottom_right_x the bottom right x value of the bound
     * @param top_left_y the top left y value of the bound
     * @param bottom_right_y the bottom right y value of the board
     * @return true/false if the point is within the bound
     */
    static bool PointWithinBound(float check_x, float check_y, float top_left_x,
                          float bottom_right_x, float top_left_y, float bottom_right_y) {
      return !(check_x < top_left_x || check_x > bottom_right_x ||
              check_y < top_left_y || check_y > bottom_right_y);
    }
  };

  /* -------- used by various objects for custom sorting predicate -------- */

  /**
   * Sorts a vector by increasing x value
   */
  struct SortByX {
    inline bool operator() (const b2Vec2& vector_one, const b2Vec2& vector_two) {
      return (vector_one.x < vector_two.x);
    }
  };

  /**
   * Sorts a vector by increasing y value
   */
  struct SortByY {
    inline bool operator() (const b2Vec2& vector_one, const b2Vec2& vector_two) {
      return (vector_one.y < vector_two.y);
    }
  };

  /**
   * Sorts tiles by its x values
   */
  struct TileHorizontal {
    inline bool operator() (const Tile* tile_one, const Tile* tile_two ) {
      return (tile_one->drawing_info.img_rectangle_.x1 < tile_two->drawing_info.img_rectangle_.x1);
    }
  };

  /**
   * Sorts tiles by its y values
   */
  struct TileVertical {
    inline bool operator() (const Tile* tile_one, const Tile* tile_two ) {
      return (tile_one->drawing_info.img_rectangle_.y1 < tile_two->drawing_info.img_rectangle_.y1);
    }
  };

}

#endif  // FINALPROJECT_WORLD_UTIL_H
