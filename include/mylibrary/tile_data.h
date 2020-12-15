//
// Created by nishu on 4/21/2020.
//

#ifndef FINALPROJECT_TILE_DATA_H
#define FINALPROJECT_TILE_DATA_H

#include "tile.h"
namespace scrabble {

  class TileData : Tile {
   private:
    /** maps and vectors to associate structs with values **/
    std::map<BOARD_TILE, BoardTileInfo> board_tile_map;
    std::map<BOARD_TILE, DrawingInfo> board_tile_name_map;
    std::vector<LetterTileInfo> letter_tile_vec;
    /** image information to be rendered **/
    const std::string kTilePrefix = "images/board-tiles/";
    const std::string kTileSuffix = ".png";


   public:
    explicit TileData() {
      /** associates a board tile to its struct **/
      board_tile_map = {
          {BOARD_TILE::EMPTY, BoardTileInfo(1, 1)},
          {BOARD_TILE::START, BoardTileInfo(1, 2)},
          {BOARD_TILE::DOUBLE_LETTER, BoardTileInfo(2, 1)},
          {BOARD_TILE::DOUBLE_WORD, BoardTileInfo(1, 2)},
          {BOARD_TILE::TRIPLE_LETTER, BoardTileInfo(3, 1)},
          {BOARD_TILE::TRIPLE_WORD, BoardTileInfo(1, 3)},
      };
      /** associates a board tile to its drawing information **/
      board_tile_name_map = {
          {BOARD_TILE::EMPTY, DrawingInfo("empty", kTilePrefix, kTileSuffix)},
          {BOARD_TILE::START, DrawingInfo("start", kTilePrefix, kTileSuffix)},
          {BOARD_TILE::DOUBLE_LETTER, DrawingInfo("double-letter", kTilePrefix, kTileSuffix)},
          {BOARD_TILE::DOUBLE_WORD, DrawingInfo("double-word", kTilePrefix, kTileSuffix)},
          {BOARD_TILE::TRIPLE_LETTER, DrawingInfo("triple-letter", kTilePrefix, kTileSuffix)},
          {BOARD_TILE::TRIPLE_WORD, DrawingInfo("triple-word", kTilePrefix, kTileSuffix)}
      };
      /** associates a letter with its value and frequency **/
      letter_tile_vec =  {
        LetterTileInfo(1, 9), LetterTileInfo(3, 2),
            LetterTileInfo(3, 2), LetterTileInfo(2, 4),
            LetterTileInfo(1, 12), LetterTileInfo(4, 2),
            LetterTileInfo(2, 3), LetterTileInfo(4, 2),
            LetterTileInfo(1, 9), LetterTileInfo(8, 1),
            LetterTileInfo(5, 1), LetterTileInfo(1, 4),
            LetterTileInfo(3, 2), LetterTileInfo(1, 6),
            LetterTileInfo(1, 8), LetterTileInfo(3, 2),
            LetterTileInfo(10, 1), LetterTileInfo(1, 6),
            LetterTileInfo(1, 4), LetterTileInfo(1, 6),
            LetterTileInfo(1, 4), LetterTileInfo(4, 2),
            LetterTileInfo(4, 2), LetterTileInfo(8, 1),
            LetterTileInfo(4, 2), LetterTileInfo(10, 1),
      };

    }

    /* --------------------- getters and setters --------------------- */

    BoardTileInfo GetBoardTileInfo(BOARD_TILE board_tile) {
        return board_tile_map.find(board_tile)->second;
    }

    DrawingInfo GetBoardDrawingInfo(BOARD_TILE board_tile) {
        return board_tile_name_map.find(board_tile)->second;
    }

    LetterTileInfo GetLetterTileInfo(char letter) {
      if (letter == ' ') {
        return LetterTileInfo(0, 2);
      }
      return letter_tile_vec[letter - 'a'];
    }

  };
}

#endif  // FINALPROJECT_TILE_DATA_H
