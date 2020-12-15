//
// Created by nishu on 4/19/2020.
//

#include <mylibrary/board.h>
#include <mylibrary/tile_holder.h>
#include <mylibrary/world_util.h>

#include <fstream>
#include <iostream>
#include <utility>

using namespace scrabble;

Board::Board(const int vertical_off, const int horizontal_off) {
  kVerticalOffset = vertical_off;
  kHorizontalOffset = horizontal_off;
  InitializeBoard();
  ProcessScrabbleDictionary();
}

void Board::InitializeBoard() {
  /* adds the horizontal middle line */
  std::vector<Tile*> middle_line;
  middle_line.push_back(new Tile(BOARD_TILE::START));

  for (BOARD_TILE& tile : middle_board_line) {
    middle_line.push_back(new Tile(tile));
    middle_line.insert(middle_line.begin(), new Tile(tile));
  }

  board.push_back(middle_line);

  /* adds everything else in the board, mirrored horizontally and vertically */
  PushToBoard(std::vector<BOARD_TILE>{BOARD_TILE::DOUBLE_LETTER, BOARD_TILE::EMPTY, BOARD_TILE::DOUBLE_LETTER, BOARD_TILE::EMPTY},
              std::vector<int>{1, 3, 1, 2}, 0);

  PushToBoard(std::vector<BOARD_TILE>{BOARD_TILE::EMPTY, BOARD_TILE::TRIPLE_LETTER, BOARD_TILE::EMPTY, BOARD_TILE::TRIPLE_LETTER, BOARD_TILE::EMPTY},
              std::vector<int>{1, 1, 3, 1, 1}, 1);

  PushToBoard(std::vector<BOARD_TILE>{BOARD_TILE::EMPTY, BOARD_TILE::DOUBLE_WORD, BOARD_TILE::EMPTY},
              std::vector<int>{2, 1, 4}, 2);

  PushToBoard(std::vector<BOARD_TILE>{BOARD_TILE::EMPTY, BOARD_TILE::DOUBLE_WORD, BOARD_TILE::EMPTY, BOARD_TILE::DOUBLE_LETTER},
              std::vector<int>{3, 1, 2, 1}, 3);

  PushToBoard(std::vector<BOARD_TILE>{BOARD_TILE::DOUBLE_LETTER, BOARD_TILE::EMPTY, BOARD_TILE::DOUBLE_WORD, BOARD_TILE::EMPTY},
              std::vector<int>{1, 3, 1, 2}, 4);

  PushToBoard(std::vector<BOARD_TILE>{BOARD_TILE::EMPTY, BOARD_TILE::TRIPLE_LETTER, BOARD_TILE::EMPTY, BOARD_TILE::DOUBLE_WORD, BOARD_TILE::EMPTY},
              std::vector<int>{1, 1, 3, 1, 1}, 5);

  PushToBoard(std::vector<BOARD_TILE>{BOARD_TILE::EMPTY, BOARD_TILE::DOUBLE_LETTER, BOARD_TILE::EMPTY, BOARD_TILE::TRIPLE_WORD},
              std::vector<int>{3, 1, 2, 1}, 6);

  AssignTileIndexes();
}

void Board::PushToBoard(std::vector<BOARD_TILE> board_line,
                        std::vector<int> num_tiles, const int middle_index) {
  /* makes sure the arrays or the same size */
  assert(board_line.size() == num_tiles.size());

  std::vector<Tile*> push_board_front;
  std::vector<Tile*> push_board_back;

  push_board_front.push_back(new Tile(middle_board_line[middle_index]));
  push_board_back.push_back(new Tile(middle_board_line[middle_index]));

  /* goes through every tile */
  for (unsigned int board_line_index = 0; board_line_index < board_line.size(); board_line_index++) {
    /* continues while the number of tiles needs to continue */
    for (unsigned int num_tiles_index = 0; num_tiles_index < num_tiles[board_line_index]; num_tiles_index++) {
        /* pushes the tile to the front and end of the list */
        push_board_front.push_back(new Tile(board_line.at(board_line_index)));
        push_board_front.insert(push_board_front.begin(), new Tile(board_line.at(board_line_index)));

      push_board_back.push_back(new Tile(board_line.at(board_line_index)));
      push_board_back.insert(push_board_back.begin(), new Tile(board_line.at(board_line_index)));
    }
  }

  /* adds the completed line to the board */
  board.push_back(push_board_back);
  board.insert(board.begin(), std::vector<Tile*>(push_board_front));
}

void Board::UpdateTileLocation(Tile* new_tile, int row_loc, int col_loc) {
  new_tile->SetTileLocation(row_loc, col_loc, kVerticalOffset, kHorizontalOffset);
  /* adds the tile replaced to the vector of replaced tiles */
  tiles_replaced_.push_back(board[row_loc][col_loc]);
  board[row_loc][col_loc] = new_tile;
}

void Board::AssignTileIndexes() {
  for (int row_index = 0; row_index < kBoardSize; row_index++) {
    for (int col_index = 0; col_index < kBoardSize; col_index++) {
      Tile* current_tile = board[row_index][col_index];
      current_tile->SetTileLocation(row_index, col_index, kVerticalOffset, kHorizontalOffset);
    }
  }
}

TILE_ERROR_CODE Board::PlaceTiles(std::vector<Tile*> tiles_to_add,
                       std::vector<b2Vec2> tile_locations) {
  /* resets the global variables that are updated with every turn */
  tiles_replaced_.clear();
  new_words_.clear();
  turn_score_ = 0;

  for (int tile_index = 0; tile_index < tiles_to_add.size(); tile_index++) {
    /* gets the current tile*/
    Tile* curr_tile = tiles_to_add[tile_index];
    UpdateTileLocation(curr_tile, (int) tile_locations[tile_index].y, (int) tile_locations[tile_index].x);
  }

  /* checks the "graph" of words */
  TILE_ERROR_CODE connectivity_error_code = CheckWordConnectivity(tile_locations);

  if (connectivity_error_code != TILE_ERROR_CODE::NO_ERROR) {
    ReplaceUpdatedTiles();
    return connectivity_error_code;
  } else {
    /* checks the validity of the words */
    new_words_ = FindNewWords(tile_locations);
    TILE_ERROR_CODE word_letter_code = CheckWordValidity(new_words_);
    if (word_letter_code != TILE_ERROR_CODE::NO_ERROR) {
      ReplaceUpdatedTiles();
    } else {
      CalculateTurnScore(tiles_to_add);
      /* adds the new words to the list of played words */
      words_played_.insert(words_played_.end(), new_words_.begin(), new_words_.end());
    }
    return word_letter_code;
  }
}

void Board::CalculateTurnScore(const std::vector<Tile*>& tiles_to_add) {
  /* sets the state of all the tiles to being on the board */
  SetTileBoardState(tiles_to_add);

  /* checks the three cases: one tile played, vertical word, or horizontal word */
  if (tiles_to_add.size() == 1) {
    HandleScoreOneTile(tiles_to_add);
  } else if (is_horizontal_) {
    HandleHorizontalScore(tiles_to_add);
  } else if (is_vertical_) {
    HandleVerticalScore(tiles_to_add);
  }

  /* checks for if all tiles are used */
  if (tiles_to_add.size() == kHolderTileCount) {
    turn_score_ += kAllTilesPoints;
  }
}

void Board::HandleScoreOneTile(std::vector<Tile*> tiles_to_add) {
  /* gets the row and column of the tile */
  int col_index = ((int) tiles_to_add[0]->drawing_info.img_rectangle_.x1 - (int) board[0][0]->drawing_info.img_rectangle_.x1) / kTileSize;
  int row_index = ((int) tiles_to_add[0]->drawing_info.img_rectangle_.y1 - (int) board[0][0]->drawing_info.img_rectangle_.y1) / kTileSize;

  /* adds the horizontal scores */
  if ((col_index != 0 && board[row_index][col_index - 1]->tile_type_ == TILE_TYPE::LETTER) ||
      (col_index != kBoardSize - 1 && board[row_index][col_index + 1]->tile_type_ == TILE_TYPE::LETTER)) {
    turn_score_ += GetHorizontalWordScore(tiles_replaced_[0], tiles_to_add[0], row_index, col_index);
  }

  /* adds the vertical scores */
  if ((row_index != 0 && board[row_index - 1][col_index]->tile_type_ == TILE_TYPE::LETTER) ||
      (row_index != kBoardSize - 1 && board[row_index + 1][col_index]->tile_type_ == TILE_TYPE::LETTER)) {
    turn_score_ += GetVerticalWordScore(tiles_replaced_[0], tiles_to_add[0], row_index, col_index);
  }
}

void Board::HandleHorizontalScore(std::vector<Tile*> tiles_to_add) {
  /* sorts the vectors appropriately */
  std::sort(tiles_to_add.begin(), tiles_to_add.end(), TileHorizontal());
  std::sort(tiles_replaced_.begin(), tiles_replaced_.end(), TileHorizontal());

  /* declares a horizontal word score and word multiplier to be updated */
  int horizontal_word_mult = 1;
  int horizontal_score = 0;

  for (int tile_index = 0; tile_index < tiles_to_add.size(); tile_index++) {
    /* gets the current tiles */
    Tile* letter_tile = tiles_to_add[tile_index];
    Tile* board_tile = tiles_replaced_[tile_index];
    /* gets the row and column index */
    int col_index = ((int) letter_tile->drawing_info.img_rectangle_.x1 - (int) board[0][0]->drawing_info.img_rectangle_.x1) / kTileSize;
    int row_index = ((int) letter_tile->drawing_info.img_rectangle_.y1 - (int) board[0][0]->drawing_info.img_rectangle_.y1) / kTileSize;

    if ((row_index != 0 && board[row_index - 1][col_index]->tile_type_ == TILE_TYPE::LETTER) || (row_index != kBoardSize - 1 && board[row_index + 1][col_index]->tile_type_ == TILE_TYPE::LETTER)) {
      turn_score_ += GetVerticalWordScore(board_tile, letter_tile, row_index, col_index);
    }

    /* adds the vertical score as there is a vertical word present */
    horizontal_score += board_tile->board_tile_info.letter_multiplier_ * letter_tile->letter_tile_info.value_;
    horizontal_word_mult *= board_tile->board_tile_info.word_multiplier_;
  }

  /* adds connecting tile points */
  float col_max = tiles_to_add[tiles_to_add.size() - 1]->drawing_info.img_rectangle_.x1;
  float col_min = tiles_to_add[0]->drawing_info.img_rectangle_.x1;
  int row_val = (int) tiles_to_add[0]->drawing_info.img_rectangle_.y1;

  if ((int) ((col_max - col_min) / kTileSize) != tiles_to_add.size() - 1) {
    horizontal_score += GetHorizontalMissingTilePoints(tiles_to_add, row_val, (int) col_min, (int) col_max);
  }

  horizontal_score += GetHorizontalConnectingTilePoints(row_val, (int) col_min, (int) col_max);

  turn_score_ += horizontal_score * horizontal_word_mult;
}

int Board::GetVerticalWordScore(Tile* board_tile, Tile* letter_tile, int row_index, int col_index) {
  /* creates a value for the vertical word score and multiplier */
  int vertical_score = 0;
  int vertical_word_mult = board_tile->board_tile_info.word_multiplier_;

  /* adds the vertical score as there is a vertical word present */
  vertical_score += board_tile->board_tile_info.letter_multiplier_ * letter_tile->letter_tile_info.value_;

  /* traverses above the letter */
  int row_iterator = row_index + 1;
  while (row_iterator != kBoardSize && board[row_iterator][col_index]->tile_type_ == TILE_TYPE::LETTER) {
    vertical_score += board[row_iterator][col_index]->letter_tile_info.value_;
    row_iterator++;
  }

  /* traverses below the letter */
  row_iterator = row_index - 1;
  while (row_iterator != -1 && board[row_iterator][col_index]->tile_type_ == TILE_TYPE::LETTER) {
    vertical_score += board[row_iterator][col_index]->letter_tile_info.value_;
    row_iterator--;
  }

  return (vertical_word_mult * vertical_score);
}

int Board::GetHorizontalMissingTilePoints(const std::vector<Tile*>& tiles_to_add, int row_val, int col_min, int col_max) {
  int missing_tile_points = 0;
  /* adds the points of the tiles that are part of the word, but already on the board */
  for (int col_index = (int) col_min; col_index <= (int) col_max; col_index+=kTileSize) {
    bool part_of_word = false;
    /* checks if the tile is a part of the word */
    for (Tile* tile : tiles_to_add) {
      if ((int) tile->drawing_info.img_rectangle_.x1 == col_index) {
        part_of_word = true;
      }
    }
    /* if it's not, the tile's points are added */
    if (!part_of_word) {
      int row_pos = (row_val - (int) board[0][0]->drawing_info.img_rectangle_.y1) / kTileSize;
      int col_pos = (col_index - (int) board[0][0]->drawing_info.img_rectangle_.x1) / kTileSize;

      missing_tile_points += board[row_pos][col_pos]->letter_tile_info.value_;
    }
  }
  return missing_tile_points;
}

int Board::GetHorizontalConnectingTilePoints(int row_val, int col_min, int col_max) {
  int connecting_tile_points = 0;
  /* gets the max/min column positions of the tile */
  int row_pos = (row_val - (int) board[0][0]->drawing_info.img_rectangle_.y1) / kTileSize;
  int col_max_pos = (col_max - (int) board[0][0]->drawing_info.img_rectangle_.x1) / kTileSize;
  int col_min_pos = (col_min - (int) board[0][0]->drawing_info.img_rectangle_.x1) / kTileSize;

  int col_max_iterator = 1;
  int col_min_iterator = 1;
  /* iterates above and below the tile */
  while ((col_max_pos + col_max_iterator != kBoardSize && board[row_pos][col_max_pos + col_max_iterator]->tile_type_ == TILE_TYPE::LETTER) ||
          (col_min_pos - col_min_iterator != -1 && board[row_pos][col_min_pos - col_min_iterator]->tile_type_ == TILE_TYPE::LETTER)) {
    if (col_max_pos + col_max_iterator != kBoardSize && board[row_pos][col_max_pos + col_max_iterator]->tile_type_ == TILE_TYPE::LETTER) {
      connecting_tile_points += board[row_pos][col_max_pos + col_max_iterator]->letter_tile_info.value_;
      col_max_iterator++;
    } else if (col_min_pos - col_min_iterator != -1) {
      connecting_tile_points += board[row_pos][col_min_pos - col_min_iterator]->letter_tile_info.value_;
      col_min_iterator++;
    }
  }
  return connecting_tile_points;
}

void Board::HandleVerticalScore(std::vector<Tile*> tiles_to_add) {
  /* sorts the vectors appropriately */
  std::sort(tiles_to_add.begin(), tiles_to_add.end(), TileVertical());
  std::sort(tiles_replaced_.begin(), tiles_replaced_.end(), TileVertical());

  /* declares a vertical word score and word multiplier to be updated */
  int vertical_word_mult = 1;
  int vertical_score = 0;

  for (int tile_index = 0; tile_index < tiles_to_add.size(); tile_index++) {
    /* gets the current tiles */
    Tile* letter_tile = tiles_to_add[tile_index];
    Tile* board_tile = tiles_replaced_[tile_index];
    /* gets the row and column index */
    int col_index = ((int) letter_tile->drawing_info.img_rectangle_.x1 - (int) board[0][0]->drawing_info.img_rectangle_.x1) / kTileSize;
    int row_index = ((int) letter_tile->drawing_info.img_rectangle_.y1 - (int) board[0][0]->drawing_info.img_rectangle_.y1) / kTileSize;

    if ((col_index != 0 && board[row_index][col_index - 1]->tile_type_ == TILE_TYPE::LETTER) || (col_index != kBoardSize - 1 && board[row_index][col_index + 1]->tile_type_ == TILE_TYPE::LETTER)) {
      turn_score_ += GetHorizontalWordScore(board_tile, letter_tile, row_index, col_index);
    }

    /* adds the vertical score as there is a vertical word present */
    vertical_score += board_tile->board_tile_info.letter_multiplier_ * letter_tile->letter_tile_info.value_;
    vertical_word_mult *= board_tile->board_tile_info.word_multiplier_;
  }

  /* adds connecting tile points */
  float row_max = tiles_to_add[tiles_to_add.size() - 1]->drawing_info.img_rectangle_.y1;
  float row_min = tiles_to_add[0]->drawing_info.img_rectangle_.y1;
  int col_val = (int) tiles_to_add[0]->drawing_info.img_rectangle_.x1;

  if ((int) ((row_max - row_min) / kTileSize) != tiles_to_add.size() - 1) {
    vertical_score += GetVerticalMissingTilePoints(tiles_to_add, col_val, (int) row_min, (int) row_max);
  }

  vertical_score += GetVerticalConnectingTilePoints(col_val, (int) row_min, (int) row_max);

  turn_score_ += vertical_score * vertical_word_mult;
}

int Board::GetHorizontalWordScore(Tile* board_tile, Tile* letter_tile,
                                  int row_index, int col_index) {

  /* creates a value for the horizontal word score and multiplier */
  int horizontal_score = 0;
  int horizontal_word_mult = board_tile->board_tile_info.word_multiplier_;

  /* adds the horizontal score as there is a horizontal word present */
  horizontal_score += board_tile->board_tile_info.letter_multiplier_ * letter_tile->letter_tile_info.value_;

  /* traverses above the letter */
  int col_iterator = col_index + 1;
  while (col_iterator != kBoardSize && board[row_index][col_iterator]->tile_type_ == TILE_TYPE::LETTER) {
    horizontal_score += board[row_index][col_iterator]->letter_tile_info.value_;
    col_iterator++;
  }

  /* traverses below the letter */
  col_iterator = col_index - 1;
  while (col_iterator != -1 && board[row_index][col_iterator]->tile_type_ == TILE_TYPE::LETTER) {
    horizontal_score += board[row_index][col_iterator]->letter_tile_info.value_;
    col_iterator--;
  }

  return horizontal_word_mult * horizontal_score;
}

int Board::GetVerticalMissingTilePoints(const std::vector<Tile*>& tiles_to_add, int col_val, int row_min, int row_max) {
  int missing_tile_points = 0;
  /* adds the points of the tiles that are part of the word, but already on the board */
  for (int row_index = (int) row_min; row_index <= (int) row_max; row_index+=kTileSize) {
    bool part_of_word = false;
    /* checks if the tile is a part of the word */
    for (Tile* tile : tiles_to_add) {
      if ((int) tile->drawing_info.img_rectangle_.y1 == row_index) {
        part_of_word = true;
      }
    }
    /* if it isn't that tile's points are added to the score */
    if (!part_of_word) {
      int row_pos = (row_index - (int) board[0][0]->drawing_info.img_rectangle_.y1) / kTileSize;
      int col_pos = (col_val - (int) board[0][0]->drawing_info.img_rectangle_.x1) / kTileSize;

      missing_tile_points += board[row_pos][col_pos]->letter_tile_info.value_;
    }
  }
  return missing_tile_points;
}

int Board::GetVerticalConnectingTilePoints(int col_val, int row_min, int row_max) {
  int connecting_tile_points = 0;
  /* gets the minimum and maximum row position of the word */
  int row_max_pos = (row_max - (int) board[0][0]->drawing_info.img_rectangle_.y1) / kTileSize;
  int row_min_pos = (row_min - (int) board[0][0]->drawing_info.img_rectangle_.y1) / kTileSize;
  int col_pos = (col_val - (int) board[0][0]->drawing_info.img_rectangle_.x1) / kTileSize;

  int row_max_iterator = 1;
  int row_min_iterator = 1;
  /* traverses to the above and below the word */
  while ((row_max_pos + row_max_iterator != kBoardSize && board[row_max_pos + row_max_iterator][col_pos]->tile_type_ == TILE_TYPE::LETTER) ||
         (row_min_pos - row_min_iterator != -1 && board[row_min_pos - row_min_iterator][col_pos]->tile_type_ == TILE_TYPE::LETTER)) {
    if (row_max_pos + row_max_iterator != kBoardSize && board[row_max_pos + row_max_iterator][col_pos]->tile_type_ == TILE_TYPE::LETTER) {
      connecting_tile_points += board[row_max_pos + row_max_iterator][col_pos]->letter_tile_info.value_;
      row_max_iterator++;
    } else if (row_min_pos - row_min_iterator != -1) {
      connecting_tile_points += board[row_min_pos - row_min_iterator][col_pos]->letter_tile_info.value_;
      row_min_iterator++;
    }
  }
  return connecting_tile_points;
}


TILE_ERROR_CODE Board::CheckWordValidity(const std::vector<std::string>& new_words) {
  /* checks if now words were played */
  if (new_words.empty()) {
    return TILE_ERROR_CODE::NO_WORD_PLAYED;
  }

  /* checks for specific edge case for if the same word is played twice */
  if (new_words_[0] == kErrorString) {
    return TILE_ERROR_CODE::PLAYED_TWICE;
  }

  for (const std::string& word : new_words) {
    /* checks if the word has already been played */
    if (std::find(words_played_.begin(), words_played_.end(), word) !=
        words_played_.end()) {
      return TILE_ERROR_CODE::ALREADY_PLAYED;
    }
    /* checks if the word exists in the scrabble dictionary */
    if (!dictionary_.FindWord(word)) {
      return TILE_ERROR_CODE::INVALID_WORD;
    }
  }
  /* sets first_play_ to false, as no rules were violated */
  if (first_play_) {
    first_play_ = false;
  }

  return TILE_ERROR_CODE::NO_ERROR;
}

TILE_ERROR_CODE Board::CheckWordConnectivity(const std::vector<b2Vec2>& tile_locations) {
  /* sets the booleans to be used later in score calculation */
  is_horizontal_ = IsConnectedHorizontally(tile_locations);
  is_vertical_ = IsConnectedVertically(tile_locations);

  /* checks if this is the first word */
  if (first_play_) {
    /* if it is, it must be vertical/horizontal and have a tile on the starting location */
    if (is_horizontal_ || is_vertical_) {
      for (b2Vec2 location : tile_locations) {
        if ((int) location.x == kBoardSize / 2 && (int) location.y == kBoardSize / 2) {
          return TILE_ERROR_CODE::NO_ERROR;
        }
      }
      /* otherwise the tile is not on the start location */
      return TILE_ERROR_CODE::NOT_ON_START;
    }
    /* otherwise the tile isn't connected properly */
    return TILE_ERROR_CODE::NOT_CONNECTED;
  } else {
    /* no error if its connected */
    if (is_horizontal_ || is_vertical_) {
      return TILE_ERROR_CODE::NO_ERROR;
    }
    return TILE_ERROR_CODE::NOT_CONNECTED;
  }
}

bool Board::IsConnectedVertically(std::vector<b2Vec2> tile_locations) {
  /* sorts the list by the vectors' y values */
  std::sort(tile_locations.begin(), tile_locations.end(), SortByY());
  /* gets the x value that should be consistent */
  const int x_pos = (*tile_locations.begin()).x;

  /* iterates through each location and makes sure their x values are the same */
  for (b2Vec2 location : tile_locations) {
    if ((int) location.x != x_pos) {
      return false;
    }
  }
  /* iterates through the beginning and end y values, and ensures that there exists a straight line of letters */
  if (tile_locations.size() > 1) {
    for (int y_pos = tile_locations[0].y; y_pos < (int)tile_locations[tile_locations.size() - 1].y; y_pos++) {
      if (board[y_pos][x_pos]->tile_type_ != TILE_TYPE::LETTER) {
        return false;
      }
    }
  }

  /* the rest does not apply to the first turn */
  if (first_play_) {
    return true;
  }

  /* iterates through each tile to see if any are connected to the left or right */
  for (b2Vec2 location : tile_locations) {
    if ((x_pos != 0 && board[location.y][x_pos - 1]->tile_type_ == TILE_TYPE::LETTER) ||
        (x_pos != kBoardSize - 1 && board[location.y][x_pos + 1]->tile_type_ == TILE_TYPE::LETTER)) {
      return true;
    }
  }
  /* returns if there is a tile above the first one, below the last one, or if there was a tile that needed to be filled in */
  return (((int) tile_locations[0].y != 0 && board[(int) tile_locations[0].y - 1][x_pos]->tile_type_ == TILE_TYPE::LETTER) ||
          ((int) tile_locations[tile_locations.size() - 1].y != kBoardSize - 1 && board[tile_locations[(int) tile_locations.size() - 1].y + 1][x_pos]->tile_type_ == TILE_TYPE::LETTER) ||
          (int) (tile_locations[tile_locations.size() - 1].y - tile_locations[0].y) != (tile_locations.size() - 1));
}

bool Board::IsConnectedHorizontally(std::vector<b2Vec2> tile_locations) {
  /* sorts the list by the vectors' x values */
  std::sort(tile_locations.begin(), tile_locations.end(), SortByX());
  /* gets the y value that should be consistent */
  const int y_pos = (*tile_locations.begin()).y;

  /* iterates through each location and makes sure their y values are the same */
  if (tile_locations.size() > 1) {
    for (b2Vec2 location : tile_locations) {
      if ((int)location.y != y_pos) {
        return false;
      }
    }
  }

  /* iterates through the beginning and end x values, and ensures that there exists a straight line of letters */
  for (int x_pos = (int) tile_locations[0].x; x_pos < (int) tile_locations[tile_locations.size() - 1].x; x_pos++) {
    if (board[y_pos][x_pos]->tile_type_ != TILE_TYPE::LETTER) {
      return false;
    }
  }

  /* the rest does not apply to the first turn */
  if (first_play_) {
    return true;
  }

  /* iterates through each tile to see if any are connected above or below */
  for (b2Vec2 location : tile_locations) {
    if ((y_pos != 0 && board[y_pos - 1][(int) location.x]->tile_type_ == TILE_TYPE::LETTER) ||
        (y_pos != kBoardSize - 1 && board[y_pos + 1][(int) location.x]->tile_type_ == TILE_TYPE::LETTER)) {
      return true;
    }
  }
  /* returns if there is a tile to the left of the first one, right of the last one, or if there was a tile that needed to be filled in */
  return (((int) tile_locations[0].x != 0 && board[y_pos][(int) (tile_locations[0].x - 1)]->tile_type_ == TILE_TYPE::LETTER) ||
          ((int) tile_locations[tile_locations.size() - 1].x != kBoardSize - 1 && board[y_pos][(int) (tile_locations[tile_locations.size() - 1].x + 1)]->tile_type_ == TILE_TYPE::LETTER) ||
          (int) (tile_locations[(tile_locations.size() - 1)].x - tile_locations[0].x) != tile_locations.size() - 1);
}

std::vector<std::string> Board::FindNewWords(const std::vector<b2Vec2>& new_tile_locations) {
  std::vector<std::string> new_words;

  /* gets the column and row word for each location */
  for (b2Vec2 location : new_tile_locations) {
    std::string column_word = GetColumnWord(location.y, location.x);
    std::string row_word = GetRowWord(location.y, location.x);

    if (column_word == row_word && column_word.size() != 1) {
      return std::vector<std::string>{kErrorString};
    }

    /* only adds the word if it is already not in the vector */
    if (std::find(new_words.begin(), new_words.end(), column_word) == new_words.end()
        && column_word.size() > 1) {
      new_words.push_back(column_word);
    }
    if (std::find(new_words.begin(), new_words.end(), row_word) == new_words.end()
        && row_word.size() > 1) {
      new_words.push_back(row_word);
    }

  }
  return new_words;
}

std::string Board::GetColumnWord(const int& row_index, const int& col_index) {
  std::string col_word;
  /* pushes back the first letter where it was placed */
  col_word.push_back(board[row_index][col_index]->letter_tile_info.letter_);
  /* checks each letter below the tile */
  for (int current_row = row_index + 1; current_row < kBoardSize; current_row++) {
    if (board[current_row][col_index]->tile_type_ == TILE_TYPE::BOARD) {
      break;
    }
    col_word.push_back(board[current_row][col_index]->letter_tile_info.letter_);
  }
  /* checks each letter above the tile */
  for (int current_row = row_index - 1; current_row >= 0; current_row--) {
    if (board[current_row][col_index]->tile_type_ == TILE_TYPE::BOARD) {
      break;
    }
    col_word.insert(col_word.begin(), board[current_row][col_index]->letter_tile_info.letter_);
  }

  return col_word;
}

std::string Board::GetRowWord(const int& row_index, const int& col_index) {
  std::string row_word;
  /* pushes back the first letter where it was placed */
  row_word.push_back(board[row_index][col_index]->letter_tile_info.letter_);
  /* checks each letter to the right of the tile */
  for (int current_col = col_index + 1; current_col < kBoardSize; current_col++) {
    if (board[row_index][current_col]->tile_type_ == TILE_TYPE::BOARD) {
      break;
    }
    row_word.push_back(board[row_index][current_col]->letter_tile_info.letter_);
  }
  /* checks each letter to the left of the tile */
  for (int current_col = col_index - 1; current_col >= 0; current_col--) {
    if (board[row_index][current_col]->tile_type_ == TILE_TYPE::BOARD) {
      break;
    }
    row_word.insert(row_word.begin(), board[row_index][current_col]->letter_tile_info.letter_);
  }

  return row_word;
}

void Board::ProcessScrabbleDictionary() {
  /* loads the dictionary using the input operator */
  std::string string;
  std::ifstream infile(R"(C:\Cinder\my-projects\final-project-nbalepur\data\scrabble-words.txt)");
  std::istream& stream = infile;

  stream >> dictionary_;
}

void Board::ReplaceUpdatedTiles() {
  /* goes through each tile and replaces the board tiles that were previously removed */
  for (Tile* tile : tiles_replaced_) {
    float top_left_x = tile->drawing_info.img_rectangle_.x1;
    float top_left_y = tile->drawing_info.img_rectangle_.y1;

    float row_loc =
        ((top_left_x - board[0][0]->drawing_info.img_rectangle_.x1) /
         kTileSize);
    float col_loc =
        ((top_left_y - board[0][0]->drawing_info.img_rectangle_.y1) /
         kTileSize);

    board[col_loc][row_loc] = tile;
  }
}

void Board::SetTileBoardState(const std::vector<Tile*>& tiles) {
  /* goes through each tile and sets its state to being on the board */
  for (Tile* tile : tiles) {
    tile->letter_tile_info.is_on_board = true;
  }
}