//
// Created by nishu on 5/1/2020.
//

#include "mylibrary/ai.h"
namespace scrabble {

  AI::AI(Bag* bag, Board* board) {
    bag_ = bag;
    board_ = board;;
  }

  void AI::DrawTiles(int num_tiles) {
    for (int draw_index = 0; draw_index < num_tiles; draw_index++) {
      Tile* tile_to_add = bag_->Draw();
      if (tile_to_add == nullptr) {
        return;
      }
      tiles_.push_back(tile_to_add);
    }
  }

  void AI::ExchangeTiles() {
    bag_->AddToBag(tiles_);
    tiles_.clear();
    DrawTiles(kHolderTileCount);
  }

  TILE_ERROR_CODE AI::PlayWord() {
    /* declares the appropriate variables to pass as parameters */
    std::vector<TileOpening> tile_openings = FindTileOpenings();
    std::vector<int> letter_count = GetTileWordCount();

    /* if the openings are empty the bot will just exchange it's tiles */
    if (tile_openings.empty()) {
      ExchangeTiles();
      return TILE_ERROR_CODE::EXCHANGE;
    }

    /* declares the map with the custom comparator */
    std::map<std::string, TileOpening, SortByBaseWordScore> map;
    /* adds the best word and opening to the map for each opening */
    for (TileOpening opening : tile_openings) {
      std::string best_word = FindBestPossibleWord(opening, letter_count);
      map.insert(std::make_pair(best_word, opening));
    }
    /* gets the best word and opening from the map (first value) */
    std::string max_point_word = map.begin()->first;
    TileOpening max_point_opening = map.begin()->second;

    return PlaceTiles(max_point_opening, max_point_word);
  }

  TILE_ERROR_CODE AI::PlaceTiles(AI::TileOpening opening, std::string word) {
    /* sets the blank tiles (if there are any played) */
    SetBlankTile(word);

    /* changes the word to lowercase */
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    std::vector<b2Vec2> locations_to_place;

    /* finds all locations of the delimiter, taken from: https://stackoverflow.com/questions/4034750/find-all-a-substrings-occurrences-and-locations */
    size_t delim_pos = word.find(std::string(1, opening.letter_), 0);
    while(delim_pos != std::string::npos) {
      /* gets the next position of the delimiter if it doesn't fit */
      if ((std::max(opening.spaces_above_, opening.spaces_left_) < delim_pos ||
            std::max(opening.spaces_below_, opening.spaces_right_) < word.size() - delim_pos - 1)) {
        delim_pos = word.find(std::string(1, opening.letter_), delim_pos + 1);
      } else {
        /* gets the x and y location for every tile that needs to be placed */
        for (int char_index = 0; char_index < word.size(); char_index++) {
          /* skips if we are at the delimiter position */
          if (char_index == delim_pos) {
            continue;
          }
          /* gets the board x and y locations */
          int board_y = opening.row_index_ + (int)(char_index - delim_pos) * (int)(std::max(opening.spaces_above_, opening.spaces_below_) != 0);
          int board_x = opening.col_index_ + (int)(char_index - delim_pos) * (int)(std::max(opening.spaces_left_, opening.spaces_right_) != 0);

          locations_to_place.emplace_back(board_x, board_y);
        }
        break;
      }
    }

    /* reorders the tiles so they match the order of the word */
    std::vector<Tile*> tiles_to_add = ReorderTiles(word, delim_pos);

    /* replaces the nullptr tiles the AI has before returning a value */
    ReplaceTiles();
    return board_->PlaceTiles(tiles_to_add, locations_to_place);
  }

  std::string AI::FindBestPossibleWord(TileOpening opening, std::vector<int> tile_letter_count) {
    tile_letter_count[opening.letter_ - 'a'] += 1;
    std::vector<std::string> possible_words = board_->GetScrabbleDictionary().GetAllPossibleWords(tile_letter_count, opening.letter_);
    /* sorts the vector by decreasing base score value (not including tile multipliers */
    std::sort(possible_words.begin(), possible_words.end(), SortByBaseWordScore());
    return GetMaxPointWord(opening, possible_words);
  }

  std::string AI::GetMaxPointWord(TileOpening opening, const std::vector<std::string>& possible_words) {

    for (const std::string& word : possible_words) {
      std::vector<size_t> delim_positions;
      /* finds all locations of the delimiter, taken from: https://stackoverflow.com/questions/4034750/find-all-a-substrings-occurrences-and-locations */
      size_t pos = word.find(std::string(1, toupper(opening.letter_)), 0);
      while(pos != std::string::npos) {
        delim_positions.push_back(pos);
        pos = word.find(std::string(1, toupper(opening.letter_)), pos + 1);
      }
      /* makes sure that the word fits with the given delimiter and opening */
      for (size_t delim_pos : delim_positions) {
        if (!(std::max(opening.spaces_above_, opening.spaces_left_) < delim_pos ||
              std::max(opening.spaces_below_, opening.spaces_right_) < word.size() - delim_pos - 1)) {
          return word;
        }
      }
    }
    /* returns empty string otherwise */
    return "";
  }

  std::vector<AI::TileOpening> AI::FindTileOpenings() {
    /* declares and initializes vectors needed */
    std::vector<TileOpening> tile_openings_;
    std::vector<std::vector<Tile*>> board_tiles = board_->GetBoard();
    /* goes through each tile on the board */
    for (int row_index = 0; row_index < kBoardSize; row_index++) {
      for (int col_index = 0; col_index < kBoardSize; col_index++) {
        Tile* tile = board_tiles[row_index][col_index];
        /* makes sure the tile we are checking for is a letter */
        if (tile->tile_type_ == TILE_TYPE::BOARD) {
          continue;
        }

        /* initializes the tile opening */
        TileOpening tile_opening(row_index, col_index, tile->letter_tile_info.letter_);

        /* checks if the column is open and takes appropriate actions */
        bool open_col = row_index != 0 && row_index != kBoardSize - 1 &&
                        board_tiles[row_index + 1][col_index]->tile_type_ != TILE_TYPE::LETTER &&
                        board_tiles[row_index - 1][col_index]->tile_type_ != TILE_TYPE::LETTER;

        if (open_col) {
          /* checks below the delimiter */
          int num_spaces_below = 0;
          for (int below_index = row_index + 1; below_index < kBoardSize; below_index++) {
            if ((col_index != 0 && board_tiles[below_index][col_index - 1]->tile_type_ == TILE_TYPE::LETTER) ||
                board_tiles[below_index][col_index]->tile_type_ == TILE_TYPE::LETTER ||
                (col_index != kBoardSize - 1 && board_tiles[below_index][col_index + 1]->tile_type_ == TILE_TYPE::LETTER) ||
                (below_index != kBoardSize - 1 && board_tiles[below_index + 1][col_index]->tile_type_ == TILE_TYPE::LETTER)) {
              break;
            }
            num_spaces_below++;
          }
          /* checks above the delimiter */
          int num_spaces_above = 0;
          for (int above_index = row_index - 1; above_index >= 0; above_index--) {
            if ((col_index != 0 && board_tiles[above_index][col_index - 1]->tile_type_ == TILE_TYPE::LETTER) ||
                board_tiles[above_index][col_index]->tile_type_ == TILE_TYPE::LETTER ||
                (col_index != kBoardSize - 1 && board_tiles[above_index][col_index + 1]->tile_type_ == TILE_TYPE::LETTER) ||
                (above_index != 0 && board_tiles[above_index - 1][col_index]->tile_type_ == TILE_TYPE::LETTER)) {
              break;
            }
            num_spaces_above++;
          }
          /* caps the openings to 7 as to not generate needless words */
          tile_opening.spaces_above_ = num_spaces_above > kHolderTileCount ? kHolderTileCount : num_spaces_above;
          tile_opening.spaces_below_ = num_spaces_below > kHolderTileCount ? kHolderTileCount : num_spaces_below;
        }
        /* checks if the row is open and takes the appropriate actions */
        bool open_row = col_index != kBoardSize - 1 && col_index != 0 &&
                        board_tiles[row_index][col_index + 1]->tile_type_ != TILE_TYPE::LETTER &&
                        board_tiles[row_index][col_index - 1]->tile_type_ != TILE_TYPE::LETTER;

        if (open_row) {
          /* checks to the right of the delimiter */
          int num_spaces_right = 0;
          for (int right_index = col_index + 1; right_index < kBoardSize; right_index++) {
            if ((row_index != 0 && board_tiles[row_index - 1][right_index]->tile_type_ == TILE_TYPE::LETTER) ||
                board_tiles[row_index][right_index]->tile_type_ == TILE_TYPE::LETTER ||
                (row_index != kBoardSize - 1 && board_tiles[row_index + 1][right_index]->tile_type_ == TILE_TYPE::LETTER) ||
                (right_index != kBoardSize - 1 && board_tiles[row_index][right_index + 1]->tile_type_ == TILE_TYPE::LETTER)) {
              break;
            }
            num_spaces_right++;
          }
          /* checks to the left of the delimiter */
          int num_spaces_left = 0;
          for (int left_index = col_index - 1; left_index >= 0; left_index--) {
            if ((row_index != 0 && board_tiles[row_index - 1][left_index]->tile_type_ == TILE_TYPE::LETTER) ||
                board_tiles[row_index][left_index]->tile_type_ == TILE_TYPE::LETTER ||
                (row_index != kBoardSize - 1 && board_tiles[row_index + 1][left_index]->tile_type_ == TILE_TYPE::LETTER) ||
                (left_index != 0 && board_tiles[row_index][left_index - 1]->tile_type_ == TILE_TYPE::LETTER)) {
              break;
            }
            num_spaces_left++;
          }
          tile_opening.spaces_right_ = num_spaces_right > kHolderTileCount ? kHolderTileCount : num_spaces_right;
          tile_opening.spaces_left_ = num_spaces_left > kHolderTileCount ? kHolderTileCount : num_spaces_left;
        }

        /* adds the opening to the list */
        if (tile_opening.spaces_left_ + tile_opening.spaces_right_ + tile_opening.spaces_above_ + tile_opening.spaces_below_ != 0) {
          tile_openings_.push_back(tile_opening);
        }
      }
    }
    return tile_openings_;
  }

  std::vector<Tile*> AI::ReorderTiles(std::string word, size_t delim_pos) {
    std::vector<Tile*> copy_vector;
    /* iterates once through each character, and each tile in the holder */
    for (int char_index = 0; char_index < word.size(); char_index++) {
      for (int tile_index = (signed) tiles_.size() - 1; tile_index >= 0; tile_index--) {
        /* the new vector of tiles should not contain the delimiter */
        if (char_index == delim_pos) {
          continue;
        }
        /* if the tile matches it is added in the correct order to the vector */
        if (tiles_[tile_index] != nullptr && tiles_[tile_index]->letter_tile_info.letter_ == word[char_index]) {
          copy_vector.push_back(tiles_[tile_index]);
          tiles_[tile_index] = nullptr;
          break;
        }
      }
    }
    return copy_vector;
  }

  void AI::SetBlankTile(std::string& word) {
    /* iterates through each tile and checks for blank letters */
    for (Tile* tile : tiles_) {
      if (tile->letter_tile_info.letter_ == ' ') {
        for (char& letter : word) {
          /* checks if the letter is lowercase (denoting blank) */
          if (letter >= 'a') {
            /* sets the blank accordingly and makes the char uppercase */
            tile->letter_tile_info.letter_ = letter;
            tile->drawing_info = DrawingInfo(std::string(1, letter),
                                             "images/blanks/", ".png");
            letter = (char) toupper(letter);
          }
        }
      }
    }
  }

  std::vector<int> AI::GetTileWordCount() {
    /* creates and reserves a vector, filling it with zeros */
    std::vector<int> word_letter_count(kNumLettersAlphabet + 1);
    /* gets the letter count for the vector */
    for (Tile* tile : tiles_) {
      if (tile->letter_tile_info.is_blank) {
        word_letter_count[kNumLettersAlphabet] += 1;
        continue;
      }
      word_letter_count[tile->letter_tile_info.letter_ - 'a'] += 1;
    }
    return word_letter_count;
  }

  void AI::ReplaceTiles() {
    for (auto& tile : tiles_) {
      /* if the tile is null, it is replaced */
      if (tile == nullptr) {
        Tile* tile_drawn = bag_->Draw();
        /* if the drawn tile is nullptr, it returns to end the game */
        if (tile_drawn != nullptr) {
          tile = tile_drawn;
        } else {
          return;
        }
      }
    }
  }

  int AI::GetTileHolderValue() {
    int sum = 0;
    /* sums each value of the tile */
    for (Tile* tile : tiles_) {
      if (tile != nullptr) {
        sum += tile->letter_tile_info.value_;
      }
    }
    return sum;
  }
}
