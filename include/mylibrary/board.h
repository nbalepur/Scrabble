//
// Created by nishu on 4/19/2020.
//

#ifndef FINALPROJECT_BOARD_H
#define FINALPROJECT_BOARD_H

#include <vector>

#include "scrabble_dictionary.h"
#include "tile.h"

namespace scrabble {

  enum class TILE_ERROR_CODE { NO_ERROR, NOT_CONNECTED, NOT_ON_START, INVALID_WORD, NO_WORD_PLAYED, ALREADY_PLAYED, PLAYED_TWICE, EXCHANGE };

  /**
   * vertical and horizontal offset of the board
   * */
  static int kVerticalOffset;
  static int kHorizontalOffset;

  const int kBoardSize = 15;

  class Board {
   private:
    /** points for using all of the player's tiles **/
    const int kAllTilesPoints = 50;

    /** error string present in vectors with an invalid string **/
    const std::string kErrorString = "ERROR0";

    /** boolean representing whether or not it is the player's first turn **/
    bool first_play_ = true;

    /** vector containing all the words that were played **/
    std::vector<std::string> words_played_;

    /** vector containing all of the tiles that were replaced **/
    std::vector<Tile*> tiles_replaced_;

    /** vector containing the new words that were played **/
    std::vector<std::string> new_words_;

    /** 2D vector containing the state of the board **/
    std::vector<std::vector<Tile*>> board;

    /** the score of that turn **/
    int turn_score_;

    /** booleans representing if it is horizontal or vertical */
    bool is_horizontal_ = false;
    bool is_vertical_ = false;

    /**
     * the middle board line horizontally and vertically
     */
    std::vector<BOARD_TILE> middle_board_line = {BOARD_TILE::EMPTY, BOARD_TILE::EMPTY, BOARD_TILE::EMPTY,
                                                 BOARD_TILE::DOUBLE_LETTER, BOARD_TILE::EMPTY, BOARD_TILE::EMPTY,
                                                 BOARD_TILE::TRIPLE_WORD};


    /**
     * The dictionary containing all the scrabble words
     */
    ScrabbleDictionary dictionary_;

    /**
     * Finds the score for the current round
     * @param tiles_to_add the tiles that were added to the board
     */
    void CalculateTurnScore(const std::vector<Tile*>& tiles_to_add);

    /**
     * Handles scores for if the tile is vertical or horizontal, respectively
     * @param tiles_to_add the tiles that were added to the board
     */
    void HandleHorizontalScore(std::vector<Tile*> tiles_to_add);
    void HandleVerticalScore(std::vector<Tile*> tiles_to_add);

    /**
     * Handles the score if only one tile is played
     * @param tiles_to_add the tiles that were added to the board
     */
    void HandleScoreOneTile(std::vector<Tile*> tiles_to_add);

    /**
     * Handles the potential words for the opposite of is_horizontal/is_vertical
     * @param board_tile the current board tile
     * @param letter_tile the current letter tile
     * @param row_index the index of the tile's row
     * @param col_index the index of the tile's column
     * @return an integer of how many points were added
     */
    int GetVerticalWordScore(Tile* board_tile, Tile* letter_tile, int row_index, int col_index);
    int GetHorizontalWordScore(Tile* board_tile, Tile* letter_tile, int row_index, int col_index);

    /**
     * Adds the points to the word that are from tiles already on the board
     * @param tiles_to_add vector of tiles to be added
     * @param row_val the constant row value
     * @param col_min the minimum column value
     * @param col_max the maximum column value
     * @return how many points need to be added
     */
    int GetHorizontalMissingTilePoints(const std::vector<Tile*>& tiles_to_add, int row_val, int col_min, int col_max);

    /**
     * Adds the points to the word that are from tiles already on the board
     * @param tiles_to_add vector of tiles to be added
     * @param col_val the constant column value
     * @param row_min the minimum row value
     * @param row_max the maximum row value
     * @return how many points need to be added
     */
    int GetVerticalMissingTilePoints(const std::vector<Tile*>& tiles_to_add, int col_val, int row_min, int row_max);

    /**
     * Gets the points if the word played stems from an original word
     * @param row_val the constant value of the row
     * @param col_min the minimum column value
     * @param col_max the maximum column value
     * @return how many points need to be added
     */
    int GetHorizontalConnectingTilePoints(int row_val, int col_min, int col_max);

    /**
     * Gets the points if the word played stems from an original word
     * @param col_val the constant value of the column
     * @param row_min the minimum row value
     * @param row_max the maximum row value
     * @return how many points need to be added
     */
    int GetVerticalConnectingTilePoints(int col_val, int row_min, int row_max);

    /**
     * Updates the board location of a tile
     * @param new_tile the new tile to be put in place
     * @param row_loc the row location of the tile
     * @param col_loc the column location of the tile
     */
    void UpdateTileLocation(Tile* new_tile, int row_loc, int col_loc);

    /**
     * Checks to see if the words are connected properly
     * Also makes sure that the first word is on the start square
     * @param tile_locations the locations of the tiles
     */
    TILE_ERROR_CODE CheckWordConnectivity(const std::vector<b2Vec2>& tile_locations);

    /**
     * Checks if the words that were played are valid
     * Checks for if: the world was already played, a word was played, the word exists
     * @param new_words the words that were played
     */
    TILE_ERROR_CODE CheckWordValidity(const std::vector<std::string>& new_words);

    /**
     * Checks to see if the word is connected horizontally
     * @param tile_locations the locations of the tiles
     * @return true/false for the state of connectivity
     */
    bool IsConnectedHorizontally(std::vector<b2Vec2> tile_locations);

    /**
     * Checks to see if the word is connected vertically
     * @param tile_locations the locations of the tiles
     * @return true/false for the state of connectivity
     */
    bool IsConnectedVertically(std::vector<b2Vec2> tile_locations);

    /**
     * Finds the new words that were played
     * @param new_tile_locations the locations of the placed tiles
     * @return a vector containing the words
     */
    std::vector<std::string> FindNewWords(const std::vector<b2Vec2>& new_tile_locations);

    /**
     * Gets the word of a specific column
     * @param row_index row location of the tile
     * @param col_index column location of the tile
     * @return a string representing the vertical word
     */
    std::string GetColumnWord(const int& row_index, const int& col_index);

    /**
     * Gets the word of a specific row
     * @param row_index row location of the tile
     * @param col_index column location of the tile
     * @return a string representing the horizontal word
     */
    std::string GetRowWord(const int& row_index, const int& col_index);

    /**
     * Sets all the tiles' board states to being on the board
     */
    void SetTileBoardState(const std::vector<Tile*>& tiles);

    /**
     * Initializes the board with the appropriate board tiles
     */
    void InitializeBoard();

    /**
     * Pushes the tiles to the board such that it is symmetrical horizontally and vertically
     * The user must input the sequence of tiles, and the number of tiles to be placed for each tile
     * Each is stored in a vector in the same order
     *
     * @param board_line the vector of board tiles
     * @param num_tiles the vector containing the number to be placed for each board tile
     */
    void PushToBoard(std::vector<BOARD_TILE> board_line, std::vector<int> num_tiles, int middle_index);

    /**
     * Replaces the updated tiles that were originally placed
     */
    void ReplaceUpdatedTiles();

    /**
     * Assigns the indexes to the tiles so the image rectangles can be created
     * This also needs to be done for iteration functionality
     */
    void AssignTileIndexes();

    /**
     * initializes the scrabble dictionary to be used
     */
    void ProcessScrabbleDictionary();

   public:
    /** default constructor to initialize the board */
    explicit Board(int vertical_off, int horizontal_off);

    /**
     * Places a list of tiles on the board with their given body locations (in meters)
     * Each vector is ordered such that the tile at element i corresponds with location at element i
     * @param tiles_to_add the vector of tiles to add to the board
     * @param tile_locations the vector of meter locations of corresponding tiles
     */
    TILE_ERROR_CODE PlaceTiles(std::vector<Tile*> tiles_to_add, std::vector<b2Vec2> tile_locations);

    /* -------------------- getters and setters -------------------- */
    std::vector<std::vector<Tile*>> GetBoard() { return board; };

    std::vector<std::string> GetWordsPlayed() { return new_words_; };

    int GetTurnScore() { return turn_score_; };

    ScrabbleDictionary GetScrabbleDictionary() { return dictionary_; };
  };
}

#endif  // FINALPROJECT_BOARD_H
