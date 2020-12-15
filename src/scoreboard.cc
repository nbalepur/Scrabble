//
// Created by nishu on 4/29/2020.
//

#include "mylibrary/scoreboard.h"

#include <string>

using namespace scrabble;

ScoreBoard::ScoreBoard(const cinder::vec2& top_left_pos, int font_size,
                                 const cinder::Color& text_color) {
  text_draw_info_ = new TextDrawInfo(top_left_pos, font_size, text_color);
  text_draw_info_->player_one_ = kScorePrefix + std::to_string(1) + text_draw_info_->player_one_name_ + kScoreSuffix + std::to_string(0);
  text_draw_info_->player_two_ = kScorePrefix + std::to_string(2) + text_draw_info_->player_two_name_ + kScoreSuffix + std::to_string(0);
}

void ScoreBoard::UpdateScores(int points_scored) {
  player_scores_[turn_count_ % 2] += points_scored;
  /* changes the name based on whose turn it was */
  if (turn_count_ % 2 == 0) {
    text_draw_info_->player_one_ = kScorePrefix + "1" + text_draw_info_->player_one_name_ + kScoreSuffix + std::to_string(player_scores_[turn_count_ % 2]);
  } else {
    text_draw_info_->player_two_ = kScorePrefix + "2" + text_draw_info_->player_two_name_ + kScoreSuffix + std::to_string(player_scores_[turn_count_ % 2]);
  }

  turn_count_++;

}

std::vector<int> ScoreBoard::SubtractEndScores(int player_score, int ai_score) {
  player_scores_[0] -= player_score;
  player_scores_[1] -= ai_score;
  return player_scores_;
}
