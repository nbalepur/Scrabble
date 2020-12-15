//
// Created by nishu on 4/28/2020.
//

#include "mylibrary/text_box.h"

#include <mylibrary/button.h>

#include "cinder/Text.h"
#include "mylibrary/board.h"

namespace gui {
  TextBox::TextBox(b2Vec2 top_left_pos, int height, int width) {
    img_rectangle_ = cinder::Rectf(top_left_pos.x, top_left_pos.y,  top_left_pos.x + (float) width, top_left_pos.y + (float) height);

    text_box_ = cinder::TextBox();
    text_box_.setSize(cinder::vec2(width, height));
  }

  void TextBox::SetText(scrabble::TILE_ERROR_CODE error_code) {
    switch(error_code) {
      case scrabble::TILE_ERROR_CODE::NO_ERROR:
        break;
      case scrabble::TILE_ERROR_CODE::NOT_CONNECTED:
        message_ = "Your word is not connected to the board properly!";
        break;
      case scrabble::TILE_ERROR_CODE::NOT_ON_START:
        message_ = "Your first word must have a letter on the start tile!";
        break;
      case scrabble::TILE_ERROR_CODE::INVALID_WORD:
        message_ = "You played an invalid word!";
        break;
      case scrabble::TILE_ERROR_CODE::NO_WORD_PLAYED:
        message_ = "You did not play a word!";
        break;
      case scrabble::TILE_ERROR_CODE::ALREADY_PLAYED:
        message_ = "You played a word that was already played!";
        break;
      case scrabble::TILE_ERROR_CODE::PLAYED_TWICE:
        message_ = "You tried to play the same word twice!";
        break;
      case scrabble::TILE_ERROR_CODE::EXCHANGE:
        break;
    }
    text_box_.setText(message_);
  }

  cinder::gl::Texture2dRef TextBox::GetTextBoxTexture() {
    text_box_.setAlignment(cinder::TextBox::CENTER);
    text_box_.setFont(cinder::Font("Roboto", 80));
    text_box_.setColor(cinder::Color::black());
    text_box_.setBackgroundColor(cinder::Color(0.858, 0.870, 0.878));

    return (ci::gl::Texture2d::create(text_box_.render()));
  }

  void TextBox::SetText(std::vector<std::string> words_played, int score) {
    message_ = player_ + " played";
    for (int word_index = 0; word_index < words_played.size(); word_index++) {
      message_ += " '";
      message_ += words_played[word_index];
      message_ += "'";
      if (word_index == words_played.size() - 2) {
        message_ += " and";
      } else if (word_index != words_played.size() - 1) {
        message_ += ", ";
      }
    }

    message_ += " which scored ";
    message_ += std::to_string(score);
    message_ += " points!";

    text_box_.setText(message_);
  }

  void TextBox::SetText(BUTTON_TYPE button_type) {
    if (button_type == BUTTON_TYPE::EXCHANGE_TILES) {
      message_ = "You exchanged your tiles, so you lose a turn!";
      text_box_.setText(message_);
    }
  }

  void TextBox::SetText(char blank_char) {
    std::string blank_str(1, blank_char);
    message_ = "You set the blank tile to '" + blank_str + "'!";
    text_box_.setText(message_);
  }

  void TextBox::SetText(int player_end_score, int ai_end_score) {
    std::string new_message = "There are no more tiles in the bag, so ";
    new_message += player_end_score >= ai_end_score ? kPlayerName : kBotName;
    new_message += player_end_score >= ai_end_score ? " win!" : " wins!";

    message_ = std::move(new_message);
    SetText(message_);

    final_scores_updated_ = true;
  }
}