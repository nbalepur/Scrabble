//
// Created by nishu on 4/29/2020.
//

#ifndef FINALPROJECT_SCOREBOARD_H
#define FINALPROJECT_SCOREBOARD_H

#include <Box2D/Common/b2Math.h>
#include <cinder/Color.h>
#include <cinder/Font.h>

#include <vector>

namespace scrabble {
  class ScoreBoard {

   private:
    const std::string kScorePrefix = "Player ";
    const std::string kScoreSuffix = " Score: ";

    /** scores of the players **/
    std::vector<int> player_scores_ = std::vector<int>{0, 0};
    /** what number turn it is **/
    int turn_count_ = 0;

    /**
     * Struct to hold the drawing information
     */
    struct TextDrawInfo {

      const int kSpaceBetweenPlayers = 60;
      const std::string kFontName = "Roboto";

      cinder::vec2 player_one_pos_;
      cinder::vec2 player_two_pos_;

      cinder::Font font;
      cinder::Color text_color_;

      std::string player_one_;
      std::string player_two_;

      std::string player_one_name_ = " (You)";
      std::string player_two_name_ = " (AI)";

      explicit TextDrawInfo(const cinder::vec2& top_left_pos, int font_size, const cinder::Color& text_color) {
        player_one_pos_ = top_left_pos;
        player_two_pos_ = cinder::vec2(top_left_pos.x, top_left_pos.y + (float) kSpaceBetweenPlayers);

        font = cinder::Font(kFontName, (float) font_size);
        text_color_ = text_color;
      }
    };

    TextDrawInfo* text_draw_info_;

   public:

    explicit ScoreBoard() = default;

    /**
     * Initializes the drawing info of the scoreboard
     * @param top_left_pos the top left position of the scoreboard
     * @param font_size the font size to be drawn
     * @param text_color the color of the text
     */
    explicit ScoreBoard(const cinder::vec2& top_left_pos, int font_size, const cinder::Color& text_color);

    /**
     * Adds the score obtained in a turn to the appropriate player
     * @param points_scored the points scored in a game
     */
    void UpdateScores(int points_scored);

    /**
     * Subtracts the appropriate values at the end of the game
     * @param player_score the player score to be subtracted
     * @param ai_score the ai score to be subtracted
     * @return vector containing the two scores
     */
    std::vector<int> SubtractEndScores(int player_score, int ai_score);

    TextDrawInfo* GetScoreboardDraw() { return text_draw_info_; };

    bool IsHumanTurn() { return turn_count_ % 2 == 0; };

    /**
     * Resets the scoreboard
     */
    void Reset() { turn_count_ = 0; player_scores_ = std::vector<int>{0, 0}; }
  };
}

#endif  // FINALPROJECT_SCOREBOARD_H
