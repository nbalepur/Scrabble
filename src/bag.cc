//
// Created by nishu on 4/24/2020.
//

#include "mylibrary/bag.h"

#include <mylibrary/tile_data.h>

#include <random>

namespace scrabble {

  Bag::Bag() {
    PopulateTileVector();
  }

  void Bag::PopulateTileVector() {
    auto* tile_data = new TileData();
    /* iterates through every a-z tile */
    for (char current_letter = 'a'; current_letter <= 'z'; current_letter++) {
      /* gets the frequency and value of each letter */
      int frequency = tile_data->GetLetterTileInfo(current_letter).frequency_;
      int value = tile_data->GetLetterTileInfo(current_letter).value_;
      /* adds a number of tiles equal to the frequency of the letter */
      for (int freq_count = 0; freq_count < frequency; freq_count++) {
        /* creates a tile and gives it the appropriate information */
        Tile* current_tile = new Tile(current_letter);
        current_tile->letter_tile_info.value_ = value;
        current_tile->letter_tile_info.frequency_ = frequency;
        current_tile->letter_tile_info.letter_ = current_letter;
        /* adds the tile to the list */
        tiles_.push_back(current_tile);
      }
    }
    
    /* adds two blank tiles to the bag */
    for (int blank_tile_index = 0; blank_tile_index < kNumBlankTiles; blank_tile_index++) {
      Tile* blank_tile = new Tile(' ');
      blank_tile->letter_tile_info = tile_data->GetLetterTileInfo(' ');
      blank_tile->letter_tile_info.letter_ = ' ';
      blank_tile->letter_tile_info.is_blank = true;
      tiles_.push_back(blank_tile);
    }

    /* shuffles the list of tiles */
    std::shuffle(tiles_.begin(), tiles_.end(), std::mt19937(std::random_device()()));

    /* deletes tile_data to free memory */
    delete tile_data;
  }

  Tile* Bag::Draw() {
    /* if the bag is empty, it returns nullptr as the game is over */
    if (tiles_.empty()) {
      game_over_ = true;
      return nullptr;
    }
    /* gets the last tile and pops it back */
    Tile* return_tile = tiles_.back();
    tiles_.pop_back();
    return return_tile;
  }

  void Bag::AddToBag(const std::vector<Tile*>& tiles_to_add) {
    for (Tile* tile : tiles_to_add) {
      /* checks for if the tile is blank */
      if (tile->letter_tile_info.is_blank) {
        tile->drawing_info = DrawingInfo("blank", "images/board-tiles/", ".png");
        tile->letter_tile_info.letter_ = ' ';
      }
      tiles_.push_back(tile);
    }
    /* shuffles the bag for more randomness (the exchanged tiles aren't drawn again at the end */
    std::shuffle(tiles_.begin(), tiles_.end(), std::mt19937(std::random_device()()));
  }
}
