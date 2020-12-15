//
// Created by nishu on 5/1/2020.
//

#ifndef FINALPROJECT_AI_H
#define FINALPROJECT_AI_H

#include "bag.h"
#include "tile_holder.h"
namespace scrabble {
  class AI {

   private:
    const int kNumLettersAlphabet = 26;
    /** information about the game **/
    Bag* bag_;
    Board* board_;
    /** the tiles the AI has **/
    std::vector<Tile*> tiles_;

    /** struct about the openings for each letter **/
    struct TileOpening {
      /* the letter that may or may not be open */
      char letter_;
      /* information about the availability of the letter */
      int spaces_above_ = 0;
      int spaces_below_ = 0;
      int spaces_right_ = 0;
      int spaces_left_ = 0;
      /* the row and column index of the letter */
      int row_index_;
      int col_index_;

      explicit TileOpening() = default;

      explicit TileOpening(int row, int col, char letter) {
        row_index_ = row;
        col_index_ = col;
        letter_ = letter;
      }
    };

    /**
     * Places the tiles accordingly in the correct spot
     * @param opening the opening associated with the highest-scoring word
     * @param word the highest-scoring word
     */
    TILE_ERROR_CODE PlaceTiles(TileOpening opening, std::string word);

    /**
     * Gets the best possible word (highest points) for a certain TileOpening struct
     * @param openings
     * @param tile_letter_count
     * @return the best possible word represented as a string
     */
    std::string FindBestPossibleWord(TileOpening openings, std::vector<int> tile_letter_count);

    /**
     * Gets the maximum length word from a list of possible words, as well as eliminates the ones impossible by the graph restrictions
     * @param possible_words a list of the possible words
     * @param opening the struct containing information about board restrictions
     * @return the string with the longest length
     */
    std::string GetMaxPointWord(TileOpening opening, const std::vector<std::string>& possible_words);

    /**
     * Finds all the open spaces for words to be played
     * @return a vector containing structs with information about the available spaces
     */
    std::vector<TileOpening> FindTileOpenings();

    /**
     * Reorders the tile to match the given word
     * @param word the scrabble word to be matched
     * @param delim_pos the position of the delimiter
     * @return the reordered tiles to match the word
     */
    std::vector<Tile*> ReorderTiles(std::string word, size_t delim_pos);

    /**
     * Sets the value of the blank tiles (if any are played)
     * @param word the max point word that may contain a blank tile
     */
    void SetBlankTile(std::string& word);

    /**
     * Replaces tiles by drawing from the bag after playing a word
     */
    void ReplaceTiles();

    /**
     * Gets a vector of character count from the tiles
     * @return the vector of character counts
     */
    std::vector<int> GetTileWordCount();


   public:

    struct SortByBaseWordScore {
      /** base letter score values **/
      std::vector<int> letter_scores_ = std::vector<int>({1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10});

      /**
       * Compares the base scores of the words (no multipliers) in descending order
       * @param string_one first string
       * @param string_two second string
       * @return whether or not the first string has a higher score than the second string
       */
      bool operator() (const std::string& string_one, const std::string& string_two) {
        int sum_one = 0;
        int sum_two = 0;

        if (string_one.size() == kHolderTileCount + 1) {
          sum_one += 50;
        }
        if (string_two.size() == kHolderTileCount + 1) {
          sum_two += 50;
        }

        /* gets the scores for each string */
        for (char string_char : string_one) {
          if (string_char < 'a') {
            sum_one += letter_scores_[(string_char - 'A')];
          }
        }
        for (int string_char : string_two) {
          if (string_char < 'a') {
            sum_two += letter_scores_[(string_char - 'A')];
          }
        }

        /* if the scores are equal, the bot will try to use up more letters */
        if (sum_one == sum_two) {
          return string_one.length() > string_two.length();
        }
        /* returns if the first score is greater than the second (descending) */
        return (sum_one > sum_two);
      }
    };

    explicit AI(Bag* bag_, Board* board);

    /**
     * Draws tiles from the bag
     * @param num_tiles the number of tiles to be drawn
     */
    void DrawTiles(int num_tiles);

    /**
     * Exchanges all in the AI's possession
     */
    void ExchangeTiles();

    /**
     * Plays the best possible word
     * @return the error code associated with the played word
     */
    TILE_ERROR_CODE PlayWord();

    /**
     * Gets the value of the AI's tiles to be subtracted at the end of the game
     * @return the sum of the tile values
     */
    int GetTileHolderValue();
  };
}

#endif  // FINALPROJECT_AI_H
