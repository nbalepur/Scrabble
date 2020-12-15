//
// Created by nishu on 4/22/2020.
//

#include "mylibrary/mouse_handler.h"

namespace scrabble {

  MouseHandler::MouseHandler(b2World* set_world, scrabble::Board* set_board, TileHolder* set_tile_holder, gui::TextBox* text_box) {
    world_ = set_world;
    board_ = set_board;

    tile_holder_ = set_tile_holder;
    holder_location_ = tile_holder_->GetTopLeftLoc();

    message_text_box_ = text_box;

    current_body_ = nullptr;
  }

  void MouseHandler::UpdateMovedTiles() {
    if (blank_tile_placed_) {
      SetValueBlankTile();
    } else if (mouse_down_index_ != -1) {
      /* transforms the body accordingly */
        current_body_->SetTransform(WorldUtil::pixelToMeter(mouse_pos_), 0);
    }
  }

  void MouseHandler::UpdateMouseIndexDown(cinder::app::MouseEvent event) {
    mouse_down_index_ = -1;
    current_body_ = world_->GetBodyList();

    for (int tile_index = 0; tile_index < world_->GetBodyCount(); tile_index++) {
      /* gets the current body position */
      b2Vec2 current_body_pos = WorldUtil::meterToPixel(current_body_->GetPosition());
      /* checks if the body's position is within range of the mouse, sets the index accordingly */
      if (abs((float) event.getX() - current_body_pos.x) <= kTileSize / 2.0f &&
          abs((float) event.getY() - current_body_pos.y) <= kTileSize / 2.0f) {
        /* sets the index and the state of selection of the body */
        mouse_down_index_ = tile_index;
        static_cast<Tile*>(current_body_->GetUserData())->letter_tile_info.is_selected = true;
        return;
      }
      if (current_body_ != nullptr) {
        current_body_ = current_body_->GetNext();
      }
    }
    /* if none of the bodies match, the index is -1, signifying no match */
    mouse_down_index_ = -1;
  }

  void MouseHandler::UpdateMouseIndexUp(const cinder::app::MouseEvent& event) {
    /* resets the mouse index and the state of selection of the body, if applicable */
    if (mouse_down_index_ != -1 ) {
      static_cast<Tile*>(current_body_->GetUserData())
          ->letter_tile_info.is_selected = false;
    }

    std::vector<std::vector<Tile*>> board = board_->GetBoard();

    /* gets the upper left and lower right bounds of the board */
    float top_left_x = board[0][0]->drawing_info.img_rectangle_.x1;
    float top_left_y = board[0][0]->drawing_info.img_rectangle_.y1;

    float bottom_right_x = board[kBoardSize - 1][kBoardSize - 1]->drawing_info.img_rectangle_.x2;
    float bottom_right_y = board[kBoardSize - 1][kBoardSize - 1]->drawing_info.img_rectangle_.y2;

    /* defines booleans to be used in conditionals */
    bool is_on_board = WorldUtil::PointWithinBound((float) event.getX(),
                       (float) event.getY(), top_left_x, bottom_right_x, top_left_y, bottom_right_y);

    bool is_on_holder = WorldUtil::PointWithinBound((float) event.getX(),
                        (float) event.getY(), holder_location_.x, holder_location_.x + kHolderWidth,
                        holder_location_.y, holder_location_.y + kHolderHeight);


    /* swaps the tiles if the tile is on the holder */
    if (is_on_holder && mouse_down_index_ != -1) {
      HandleHolderPlacement();
    }
    /* sends the tile back to its original position if it was not placed on the board  */
    else if (!is_on_board && mouse_down_index_ != -1) {
      HandleInvalidPlacement();
    }
    /* places it on the board otherwise */
    else if (is_on_board && mouse_down_index_ != -1) {
      HandleBoardPlacement(event, (int) top_left_x, (int) top_left_y);
    }
    /* sets the mouse index to -1 */
    mouse_down_index_ = -1;
  }

  void MouseHandler::UpdateMousePosMoved(const cinder::app::MouseEvent& event) {
    mouse_pos_.x = (float)event.getX();
    mouse_pos_.y = (float)event.getY();
  }

  void MouseHandler::HandleHolderPlacement() {
    /* gets the two indexes to swap */
    int moved_index = tile_holder_->GetIndexFromLocation(b2Vec2(mouse_pos_.x, mouse_pos_.y));
    Tile* current_tile = static_cast<Tile*>(current_body_->GetUserData());
    int original_index = current_tile->letter_tile_info.tile_holder_index_;

    /* checks for if the tile is blank */
    if (current_tile->letter_tile_info.is_blank) {
      current_tile->drawing_info = DrawingInfo("blank", "images/board-tiles/", ".png");
      current_tile->letter_tile_info.letter_ = ' ';
    }

    /* sets the original index to no longer being on the board */
    current_tile->letter_tile_info.is_on_board = false;

    /* swaps the tiles */
    tile_holder_->SwapTiles(original_index, moved_index);
  }

  void MouseHandler::HandleInvalidPlacement(){
   /* gets the index through the user data of the body */
   Tile* current_tile = static_cast<Tile*>(current_body_->GetUserData());
   int tile_holder_index = current_tile->letter_tile_info.tile_holder_index_;

    /* checks for if the tile is blank */
    if (current_tile->letter_tile_info.is_blank) {
      current_tile->drawing_info = DrawingInfo("blank", "images/board-tiles/", ".png");
      current_tile->letter_tile_info.letter_ = ' ';
    }

   current_body_->SetTransform(WorldUtil::pixelToMeter(tile_holder_->GetLocationFromIndex(tile_holder_index)),0);
   /* the tile is not on the board */
   current_tile->letter_tile_info.is_on_board = false;
  }

  void MouseHandler::HandleBoardPlacement(const cinder::app::MouseEvent& event, int top_left_x, int top_left_y) {
   /* gets the x and y coordinate of the board */
   int board_x = (event.getX() - (int) top_left_x) / kTileSize;
   int board_y = (event.getY() - (int) top_left_y) / kTileSize;
   /* gets the center of the tile location */
   int center_x = (int) top_left_x + board_x * kTileSize + kTileSize / 2;
   int center_y = (int) top_left_y + board_y * kTileSize + kTileSize / 2;

   /* if there is already a tile in that location, it is an invalid tile placement */
   if (board_->GetBoard()[board_y][board_x]->letter_tile_info.is_on_board) {
     HandleInvalidPlacement();
     return;
   }

   /* if another body is occupying that space, it cannot be put there */
   for (b2Body* curr_body = world_->GetBodyList(); curr_body != nullptr; curr_body = curr_body->GetNext()) {
     if (curr_body->GetPosition() == WorldUtil::pixelToMeter(b2Vec2(center_x, center_y))) {
       HandleInvalidPlacement();
       return;
     }
   }

   /* checks for if the tile is blank */
   if (static_cast<Tile*>(current_body_->GetUserData())->letter_tile_info.is_blank) {
     static_cast<Tile*>(current_body_->GetUserData())->drawing_info = DrawingInfo("blank", "images/board-tiles/", ".png");
     static_cast<Tile*>(current_body_->GetUserData())->letter_tile_info.letter_ = ' ';
     blank_tile_placed_ = true;
   }

   /* in any other scenario, the tile can be placed on the board */
   current_body_->SetTransform(WorldUtil::pixelToMeter(b2Vec2(center_x, center_y)), 0);
   static_cast<Tile*>(current_body_->GetUserData())->letter_tile_info.is_on_board = true;
  }

  void MouseHandler::SetValueBlankTile() {
    if (current_letter_down_ == ' ') {
      message_text_box_->SetText("You played a blank tile! Please type which letter you would like");
    } else if (current_letter_down_ >= 'a' && current_letter_down_ <= 'z') {
      /* gives the appropriate message */
      message_text_box_->SetText(current_letter_down_);

      /* updates the blank tile */
      Tile* blank_tile = static_cast<Tile*>(current_body_->GetUserData());
      blank_tile->letter_tile_info.letter_ = current_letter_down_;
      blank_tile->drawing_info = DrawingInfo(std::string(1, current_letter_down_), "images/blanks/", ".png");

      /* resets appropriate variables */
      current_letter_down_ = ' ';
      blank_tile_placed_ = false;
    } else {
      message_text_box_->SetText("You must type a letter between A and Z");
    }
  }

  YES_NO_CODE MouseHandler::HandleYesNoInput() {
    if (current_letter_down_ == 'n') {
      return YES_NO_CODE::NO;
    } else if (current_letter_down_ == 'y') {
      return YES_NO_CODE::YES;
    }
    return YES_NO_CODE::NONE;
  }

  gui::BUTTON_TYPE MouseHandler::GetButtonTypePressed(const cinder::app::MouseEvent& event, const std::vector<gui::Button*>& buttons) {
    /* iterates through each button */
    for (auto* button : buttons) {
      if (button->IsClicked(event)) {
        return button->GetButtonType();
      }
    }
    /* returns the "none" button type if no button is pressed */
    return gui::BUTTON_TYPE::NONE;
  }
}
