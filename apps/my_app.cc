// Copyright (c) 2020 [Your Name]. All rights reserved.

#include "my_app.h"

#include <Box2D/Box2D.h>
#include <cinder/app/App.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/wrapper.h>
#include <mylibrary/button.h>
#include <mylibrary/tile.h>
#include <mylibrary/world_util.h>

namespace myapp {

using cinder::app::KeyEvent;
using cinder::Rectf;
using cinder::gl::Texture;

using scrabble::WorldUtil;

MyApp::MyApp() {

}

void MyApp::setup() {

}

void MyApp::update() {
  engine_.RunGame();
}

void MyApp::draw() {

  cinder::gl::clear();

  DrawBoard();
  DrawTileHolder();
  DrawBodyTiles();
  DrawButtons();
  DrawScoreboard();
  DrawTextBox();
  DrawYesNoMessage();


}

void MyApp::DrawBoard() {
  /* draws the board */
  std::vector<std::vector<scrabble::Tile*>> board = engine_.GetBoard();
  for (int row_index = 0; row_index < scrabble::kBoardSize; row_index++) {
    for (int col_index = 0; col_index < scrabble::kBoardSize; col_index++) {
      cinder::gl::draw(board[row_index][col_index]->drawing_info.img_texture_, board[row_index][col_index]->drawing_info.img_rectangle_);
    }
  }
}

void MyApp::DrawTileHolder() {
  /* draws the tile holder */
  cinder::gl::draw(engine_.GetTileHolderDraw().img_texture_, engine_.GetTileHolderDraw().img_rectangle_);
}

void MyApp::DrawBodyTiles() {
  /* gets the world */
  b2World* world = engine_.GetWorld();

  /* initializes variables to use */
  b2Body* curr_body = world->GetBodyList();
  std::vector<b2Body*> bodies;
  int draw_first_index = 0;

  /* finds the index of the tile to be drawn on top */
  for (int body_index = 0; body_index < world->GetBodyCount(); body_index++) {
    bodies.push_back(curr_body);
    if (static_cast<scrabble::Tile*>(curr_body->GetUserData())->letter_tile_info.is_selected) {
      draw_first_index = body_index;
    }
    curr_body = curr_body->GetNext();
  }

  /* draws all the tiles, starting with the one after the draw index */
  for (int body_index = 0; body_index < bodies.size(); body_index++) {
    int modified_index =  ((body_index + draw_first_index + 1)) % bodies.size();
    b2Vec2 pixel_pos = WorldUtil::meterToPixel(bodies[modified_index]->GetPosition());
    auto* letter_tile = static_cast<scrabble::Tile*>(bodies[modified_index]->GetUserData());
    float x = pixel_pos.x;
    float y = pixel_pos.y;

    Rectf img_rect = Rectf(x - (int) (scrabble::kTileSize / 2),  y - (int)(scrabble::kTileSize / 2),
                           x + (int) (scrabble::kTileSize / 2), y + (int) (scrabble::kTileSize / 2));

    cinder::gl::draw(letter_tile->drawing_info.img_texture_, img_rect);
  }
}

void MyApp::DrawButtons() {
  std::vector<gui::Button*> buttons = engine_.GetButtons();
  for (gui::Button* button : buttons) {
    cinder::gl::draw(button->GetDrawingInfo().img_texture_, button->GetDrawingInfo().img_rectangle_);
  }
}

void MyApp::DrawTextBox() {
  cinder::gl::enableAlphaBlending();

  Rectf border = engine_.GetTextBox()->GetRectangle();
  border.x1 = border.x1 - kBorderSize;
  border.x2 = border.x2 + kBorderSize;
  border.y1 = border.y1 - kBorderSize;
  border.y2 = border.y2 + kBorderSize;

  cinder::gl::drawStrokedRect(border);
  cinder::gl::draw(engine_.GetTextBox()->GetTextBoxTexture(), engine_.GetTextBox()->GetRectangle());

  if (engine_.IsGameOver() && engine_.IsFinalScore()) {
    engine_.SetScoreboardDrawn(true);
  }

}

void MyApp::DrawScoreboard() {
  auto draw_info = engine_.GetScoreBoard()->GetScoreboardDraw();

  cinder::gl::drawString(draw_info->player_one_, draw_info->player_one_pos_, draw_info->text_color_, draw_info->font);
  cinder::gl::drawString(draw_info->player_two_, draw_info->player_two_pos_, draw_info->text_color_, draw_info->font);


}

void MyApp::DrawYesNoMessage() {
  if (engine_.IsFinalScore()) {
    cinder::Font font("Roboto", 70);
    cinder::gl::drawStringCentered("Do you want to play again?\n                (Y/N)", cinder::vec2(2125, 625), cinder::Color::white(), font);
    engine_.SetShouldDisplayYesNo(true);
  }
}

void MyApp::mouseDown(cinder::app::MouseEvent event) {
  if (!engine_.GetMouseHandler()->IsBlankTilePlaced() && engine_.GetScoreBoard()->IsHumanTurn()) {
    engine_.GetMouseHandler()->UpdateMouseIndexDown(event);
    engine_.HandleButtonClicks(event);
  }
}

void MyApp::mouseUp(cinder::app::MouseEvent event) {
  if (engine_.GetScoreBoard()->IsHumanTurn()) {
    engine_.GetMouseHandler()->UpdateMouseIndexUp(event);
  }
}

void MyApp::mouseMove(cinder::app::MouseEvent event) {
  if (engine_.GetScoreBoard()->IsHumanTurn()) {
    engine_.GetMouseHandler()->UpdateMousePosMoved(event);
  }
}

void MyApp::mouseDrag(cinder::app::MouseEvent event) {
  mouseMove(event);
}

void MyApp::keyDown(KeyEvent event) {
  if (engine_.GetMouseHandler()->IsBlankTilePlaced() || engine_.IsFinalScore()) {
    engine_.GetMouseHandler()->SetCurrentChar(_tolower(event.getNativeKeyCode()));
  }
}

}  // namespace myapp
