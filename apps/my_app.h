// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_MYAPP_H_
#define FINALPROJECT_APPS_MYAPP_H_

#include <Box2D/Box2D.h>
#include <cinder/app/App.h>
#include <mylibrary/board.h>
#include <mylibrary/game_engine.h>

namespace myapp {

class MyApp : public cinder::app::App {

 private:

  const float kBorderSize = 10;
  scrabble::GameEngine engine_;

  void DrawBoard();
  void DrawBodyTiles();
  void DrawTileHolder();
  void DrawButtons();
  void DrawTextBox();
  void DrawScoreboard();
  void DrawYesNoMessage();

 public:
  MyApp();
  void setup() override;
  void update() override;
  void draw() override;
  void keyDown(cinder::app::KeyEvent) override;
  void mouseDown(cinder::app::MouseEvent) override;
  void mouseUp(cinder::app::MouseEvent) override;
  void mouseMove(cinder::app::MouseEvent) override;
  void mouseDrag(cinder::app::MouseEvent) override;

};

}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
