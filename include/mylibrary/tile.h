//
// Created by nishu on 4/19/2020.
//

#ifndef FINALPROJECT_TILE_H
#define FINALPROJECT_TILE_H

#include <cinder/Rect.h>
#include <cinder/app/AppBase.h>
#include <cinder/gl/Texture.h>
#include <Box2D/Box2D.h>

#include <map>
#include <string>
#include <utility>

namespace scrabble {

/** enum for the type of tile to be associated with multipliers **/
enum class BOARD_TILE {EMPTY, START, DOUBLE_LETTER, DOUBLE_WORD, TRIPLE_LETTER, TRIPLE_WORD};
/** enum for the type of tile **/
enum class TILE_TYPE {LETTER, BOARD};

const static int kTileSize = 100;

/**
 * Struct containing information needed to render an image in a specific shape
 */
struct DrawingInfo {
 public:
  DrawingInfo() = default;

  /** the shape and texture to be rendered **/
  cinder::gl::Texture2dRef img_texture_;
  cinder::Rectf img_rectangle_;

  DrawingInfo(const std::string& set_img_url_, const std::string& img_prefix, const std::string& img_suffix) {
    cinder::ImageSourceRef img = cinder::loadImage(cinder::app::loadAsset(img_prefix + set_img_url_ + img_suffix));
    img_texture_ = cinder::gl::Texture2d::create(img);
  }
};

class Tile {
  protected:
    /**
    * Struct for information of the tile is a board tile
    */
    struct BoardTileInfo {
      /** word information **/
      int letter_multiplier_;
      int word_multiplier_;

      explicit BoardTileInfo(int letter_mult, int word_mult) {
        letter_multiplier_ = letter_mult;
        word_multiplier_ = word_mult;
      }

      BoardTileInfo() = default;

    };

    /**
     * Struct for information if the tile is a letter tile
     */
    struct LetterTileInfo {
      /** letter information **/
      int value_;
      int frequency_;
      char letter_;

      /** holder information **/
      int tile_holder_index_;

      /** drawing information specifically for letters **/
      bool is_selected = false;
      bool is_on_board = false;

      /** for if the tile is blank **/
      bool is_blank = false;

      explicit LetterTileInfo(int set_value, int set_frequency) {
        value_ = set_value;
        frequency_ = set_frequency;
      }

      LetterTileInfo() = default;

    };

   public:
      /** stores the type of tile **/
      TILE_TYPE tile_type_;
      /** constructors to initialize the type of tile **/
      explicit Tile() = default;
      explicit Tile(const BOARD_TILE& board_tile);
      explicit Tile(char letter_tile);

      /** stores the different struct information **/
      BoardTileInfo board_tile_info;
      LetterTileInfo letter_tile_info;
      DrawingInfo drawing_info;

      /**
       * Gives the tile its row and column location, as well as it's offset
       * A rectangle is then generated from this information
       * @param set_row the tile's row location
       * @param set_col the tile's column location
       * @param row_off the tile's row offset pixel length
       * @param col_off the tile's column offset pixel length
       */
      void SetTileLocation(int set_row, int set_col, int row_off, int col_off);

      /**
       * Creates the physics body for a given tile
       * @param x_pixel_loc the x coordinate of the tile
       * @param y_pixel_loc the y coordinate of the tile
       */
      void CreatePhysicsBody(b2World* world, int x_pixel_loc, int y_pixel_loc);
  };
}

#endif  // FINALPROJECT_TILE_H
