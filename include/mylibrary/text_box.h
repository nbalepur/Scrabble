//
// Created by nishu on 4/28/2020.
//

#ifndef FINALPROJECT_TEXT_BOX_H
#define FINALPROJECT_TEXT_BOX_H

#include <cinder/Text.h>
#include <mylibrary/button.h>

#include <string>
#include <utility>

#include "board.h"
#include "tile.h"
namespace gui {

  class TextBox {

   private:
    const std::string kPlayerName = "You";
    const std::string kBotName = "AI";

    /**
     * Information to render the text box
     */
    std::string message_;
    cinder::Rectf img_rectangle_;
    cinder::TextBox text_box_;

    /**
     * Player string updated after every turn to denote whose turn it is
     */
    std::string player_;

    /**
     * True/false value for if the score is finalized
     */
    bool final_scores_updated_ = false;

   public:
    explicit TextBox(b2Vec2 top_left_pos, int height, int width);

    /**
     * Updates the text if the player made a mistake
     * @param error_code the enum representation of the error
     */
    void SetText(scrabble::TILE_ERROR_CODE error_code);

    /**
     * Sets the text box to any message
     * @param set_message the new message
     */
    void SetText(std::string set_message) { message_ = std::move(set_message); text_box_.setText(message_); };

    /**
     * Updates the text for the turn
     * @param words_played the vector of words that were played
     * @param score_obtained how many points the player scored that turn
     */
    void SetText(std::vector<std::string> words_played, int score_obtained);

    /**
     * Sets the text depending on what button the player clicked on
     * @param button_type the type of button enum
     */
    void SetText(BUTTON_TYPE button_type);

    /**
     * Sets the text for if the player played a blank tile
     * @param blank_char what the blank tile was changed to
     */
    void SetText(char blank_char);

    /**
     * Updates the text for the end of the game
     * @param player_end_score the player's final score
     * @param ai_end_score the ai's final score
     */
    void SetText(int player_end_score, int ai_end_score);

    /* ------------------------ getters and setters ------------------------ */

    cinder::gl::Texture2dRef GetTextBoxTexture();

    cinder::Rectf GetRectangle() { return img_rectangle_; };

    void SetPlayer(const std::string& set_name) { player_ = set_name.find(kBotName) != std::string::npos ? kBotName : kPlayerName; };

    bool IsFinalScore() { return final_scores_updated_; };

    void Reset() { final_scores_updated_ = false; };
  };

}

#endif  // FINALPROJECT_TEXT_BOX_H
