//
// Created by nishu on 4/24/2020.
//

#ifndef FINALPROJECT_BAG_H
#define FINALPROJECT_BAG_H

#include <mylibrary/tile.h>

#include <string>

namespace scrabble {

  class Bag {

   private:
    const int kNumBlankTiles = 2;

    /**
     * The vector of 102 tiles in the bag
     */
    std::vector<Tile*> tiles_;

    /**
     * Designating whether the game is over or not
     */
    bool game_over_ = false;

    /**
     * Populates the vector of tiles with the appropriate distribution
     */
    void PopulateTileVector();

   public:
    /**
     * Initializes the vector of tiles
     */
    explicit Bag();

    /**
     * Adds the tiles to the bag
     * @param tiles_to_add a vector of tiles to be added to the bag
     */
    void AddToBag(const std::vector<Tile*>& tiles_to_add);

    /**
     * Draws the last tile from the bag
     * @return the tile
     */
    Tile* Draw();

    /* -------------------- getters and setters -------------------- */
    bool GameIsOver() { return game_over_; };

  };
}

#endif  // FINALPROJECT_BAG_H
