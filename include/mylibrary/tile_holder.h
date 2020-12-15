//
// Created by nishu on 4/25/2020.
//

#ifndef FINALPROJECT_TILE_HOLDER_H
#define FINALPROJECT_TILE_HOLDER_H

#include <Box2D/Box2D.h>
#include <cinder/audio/audio.h>

#include "bag.h"
#include "board.h"
#include "tile.h"

namespace scrabble {
  /** useful dimensions for all classes in the namespace */
  static const int kHolderWidth = 720;
  static const int kHolderHeight = 150;

  /** number of tiles in the holder **/
  static const int kHolderTileCount = 7;

  class TileHolder {
   private:
    /** drawing information (size and img) **/
    DrawingInfo holder_drawing_info_;

    /** dimensions **/
    static const int kRidgeHeight = 25;
    static const int kStartPos = 10;

    /** order of tiles **/
    std::vector<Tile*> holder_tiles_;

    /** top left position **/
    b2Vec2 tile_holder_loc;

    /** bag the tiles will be drawn from **/
    Bag* bag_ = nullptr;

    /** the physics world */
    b2World* world_;

    /**
     * Creates the rectangle position for the tile
     * @param drawn_tile the tile to have a set rectangle
     * @param draw_index the index of the tile in the holder_tiles_ vector
     * @param custom_pos a custom position 2-d vector containing (x, y)
     */
    void CreateTileBody(Tile* drawn_tile, int draw_index);

    /**
     * Replaces the tiles that were added to the board
     * Accomplishes this by drawing from the bag
     */
    void ReplaceTiles(const std::vector<Tile*>& tiles_to_add);

    /**
     * Resets the indexes of the tiles
     */
    void ResetIndexes();

   public:

    explicit TileHolder() = default;

    /**
     * Initializes the drawing info for the object as well as the bag
     */
    explicit TileHolder(b2World* world, b2Vec2 top_left_pos, Bag* bag);

    /**
     * Draws a certain number of tiles to add to the holder
     * @param num_tiles the number of tiles to be added
     * @param optional index if there is a spot the tile needs to go
     */
    void DrawTiles(int num_tiles);

    /**
     * Exchanges all of the tiles in the player's tile holder
     */
    void ExchangeTiles();

    /**
     * Swaps two tiles
     * @param first_index the first tile to be swapped
     * @param second_index the second tile to be swapped
     */
    void SwapTiles(int first_index, int second_index);

    /**
     * Shuffles the tiles in the holder
     */
    void ShuffleTiles();

    /**
     * Gets the points of the tiles in the holder
     * @return int value of the summed points
     */
    int GetTileHolderValue();

    /**
     * Sets the location for the drawing info of the object
     * @param top_left the top left vector to be positioned
     * @param bottom_right the bottom right vector to be positioned
     */
    void SetLocation(b2Vec2 top_left);

    /**
     * Takes an index and returns the location of that index (center of body)
     * @param draw_index the index of the tile
     * @return a vector representing the COM location
     */
    b2Vec2 GetLocationFromIndex(int draw_index);

    /**
     * Takes a location and returns the index associated with it
     * @param location the location to be checked
     * @return the index of the tile contained within said location
     */
    int GetIndexFromLocation(b2Vec2 location);

    /**
     * Resets the tiles back to the holder
     */
    void ResetTiles();

    /**
     * Handles what to do to the tiles on the board
     * It deletes the bodies associated with the tile
     * It also adds the tiles to the correct spot on the board
     * @param board to add the tiles to
     * @return an error code signifying what happened
     */
    TILE_ERROR_CODE HandleTilesOnBoard(Board* board);

    /* ------------------ getters and setters ------------------ */
    DrawingInfo GetDrawingInfo() { return holder_drawing_info_; };

    b2Vec2 GetTopLeftLoc() { return tile_holder_loc; };

  };
}

#endif  // FINALPROJECT_TILE_HOLDER_H
