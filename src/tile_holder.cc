//
// Created by nishu on 4/25/2020.
//

#include "mylibrary/tile_holder.h"
#include <cinder/audio/audio.h>
#include <mylibrary/world_util.h>
#include <mylibrary/board.h>

#include <random>

namespace scrabble {

  TileHolder::TileHolder(b2World* world, b2Vec2 top_left_pos, Bag* bag) {
    /* initializes the static variable */
    tile_holder_loc = top_left_pos;
    /* initializes drawing information */
    holder_drawing_info_ = DrawingInfo("tile-holder", "images/", ".png");
    SetLocation(top_left_pos);
    /* creates a bag */
    bag_ = bag;
    /* initializes the world */
    world_ = world;
  }

  void TileHolder::DrawTiles(int num_tiles) {
      for (int draw_index = 0; draw_index < num_tiles; draw_index++) {
        /* draws the tile and populates it with the appropriate information */
        Tile* drawn_tile = bag_->Draw();
        CreateTileBody(drawn_tile, draw_index);
        drawn_tile->letter_tile_info.tile_holder_index_ = draw_index;

        /* adds the tiles to the list */
        holder_tiles_.push_back(drawn_tile);
      }
  }

  void TileHolder::SwapTiles(int first_index, int second_index) {
    /* swaps the tiles in the array and resets their indexes */
    std::swap(holder_tiles_[first_index], holder_tiles_[second_index]);
    ResetIndexes();

    /* creates a vector of the old world, and swaps the appropriate indexes */
    std::vector<b2Vec2> old_world;
    for (b2Body* curr_body = world_->GetBodyList(); curr_body != nullptr; curr_body = curr_body->GetNext()) {
      old_world.insert(old_world.begin(), curr_body->GetPosition());
    }
    std::swap(old_world[first_index], old_world[second_index]);

    /* deletes the world to recreate it*/
    delete world_;
    world_ = new b2World(b2Vec2(0, 0));
    /* goes through each tile, and gives it the appropriate location */
    for (int tile_index = 0; tile_index < holder_tiles_.size(); tile_index++) {
      if (!holder_tiles_[tile_index]->letter_tile_info.is_on_board) {
        b2Vec2 location = GetLocationFromIndex(tile_index);
        holder_tiles_[tile_index]->CreatePhysicsBody(world_, location.x, location.y);
      } else {
        b2Vec2 location = WorldUtil::meterToPixel(old_world[tile_index]);
        holder_tiles_[tile_index]->CreatePhysicsBody(world_, location.x, location.y);
      }
    }
  }

  void TileHolder::ExchangeTiles() {
    bag_->AddToBag(holder_tiles_);
    holder_tiles_.clear();
    /* deletes the world to recreate it */
    delete world_;
    world_ = new b2World(b2Vec2(0, 0));
    DrawTiles(kHolderTileCount);
    ResetTiles();
  }

  void TileHolder::ShuffleTiles() {
    std::shuffle(holder_tiles_.begin(), holder_tiles_.end(), std::mt19937(std::random_device()()));
    ResetTiles();
  }

  TILE_ERROR_CODE TileHolder::HandleTilesOnBoard(Board* board) {
    /* declares appropriate vectors */
    std::vector<b2Body*> bodies_to_destroy;

    std::vector<Tile*> tiles_to_add;
    std::vector<b2Vec2> locations_to_add;

    /* iterates through each body */
    for (b2Body* curr_body = world_->GetBodyList(); curr_body != nullptr; curr_body = curr_body->GetNext()) {
      /* checks if the user data is on the board and takes the appropriate actions */
      Tile* curr_tile = static_cast<Tile*>(curr_body->GetUserData());
      if (curr_tile->letter_tile_info.is_on_board) {
        /* adds the appropriate values to the vectors */
        tiles_to_add.push_back(curr_tile);

        b2Vec2 pixel_pos = WorldUtil::meterToPixel(curr_body->GetPosition());
        /* gets the upper left and lower right bounds of the board */
        float top_left_x = board->GetBoard()[0][0]->drawing_info.img_rectangle_.x1;
        float top_left_y = board->GetBoard()[0][0]->drawing_info.img_rectangle_.y1;

        /* gets the x and y coordinate of the board */
        float board_x = (pixel_pos.x - top_left_x) / kTileSize;
        float board_y = (pixel_pos.y - top_left_y) / kTileSize;

        locations_to_add.emplace_back(board_x, board_y);
        bodies_to_destroy.push_back(curr_body);
      }
    }

    /* handles the edge case where the tiles to add are empty */
    if (tiles_to_add.empty()) {
      ResetTiles();
      return TILE_ERROR_CODE::NO_WORD_PLAYED;
    }

    /* if the error code exists, it will be returned */
    TILE_ERROR_CODE error_code = board->PlaceTiles(tiles_to_add, locations_to_add);
    if (error_code != TILE_ERROR_CODE::NO_ERROR) {
      ResetTiles();
      return error_code;
    }

    /* destroys the bodies that need to be destroyed */
    for (auto& body_index : bodies_to_destroy) {
      world_->DestroyBody(body_index);
    }

    /* replaces all of the null tiles */
    ReplaceTiles(tiles_to_add);
    /* returns the no error code */
    return TILE_ERROR_CODE::NO_ERROR;
  }

  void TileHolder::SetLocation(b2Vec2 top_left) {
    cinder::Rectf img_rectangle(top_left.x, top_left.y, top_left.x + kHolderWidth, top_left.y + kHolderHeight);
    holder_drawing_info_.img_rectangle_ = img_rectangle;
  }

  void TileHolder::CreateTileBody(Tile* drawn_tile, int draw_index) {
    b2Vec2 center_vec = GetLocationFromIndex(draw_index);
    drawn_tile->CreatePhysicsBody(world_, center_vec.x, center_vec.y);
  }

  b2Vec2 TileHolder::GetLocationFromIndex(int draw_index) {
    int center_x = (int) tile_holder_loc.x + draw_index * kTileSize + kTileSize / 2 + kStartPos;
    int center_y = (int) tile_holder_loc.y + kRidgeHeight + kTileSize / 2;

    return {(float) center_x, (float) center_y};
  }

  int TileHolder::GetIndexFromLocation(b2Vec2 location) {
    return (((int)(location.x - tile_holder_loc.x)) / kTileSize);
  }

  void TileHolder::ResetIndexes() {
    for (int tile_index = 0; tile_index < holder_tiles_.size(); tile_index++) {
      holder_tiles_[tile_index]->letter_tile_info.tile_holder_index_ = tile_index;
    }
  }

  void TileHolder::ReplaceTiles(const std::vector<Tile*>& tiles_to_add) {
    for (Tile* tile : tiles_to_add) {
      Tile* drawn_tile = bag_->Draw();
      if (drawn_tile == nullptr) {
        return;
      }
      holder_tiles_[tile->letter_tile_info.tile_holder_index_] = drawn_tile;
      drawn_tile->letter_tile_info.tile_holder_index_ = tile->letter_tile_info.tile_holder_index_;
      CreateTileBody(drawn_tile, tile->letter_tile_info.tile_holder_index_);
    }
  }

  void TileHolder::ResetTiles() {
    ResetIndexes();
    for (b2Body* curr_body = world_->GetBodyList(); curr_body != nullptr; curr_body = curr_body->GetNext()) {
      Tile* tile = static_cast<Tile*>(curr_body->GetUserData());
      tile->letter_tile_info.is_on_board = false;

      /* checks for if the tile is blank */
      if (tile->letter_tile_info.is_blank) {
        tile->drawing_info = DrawingInfo("blank", "images/board-tiles/", ".png");
        tile->letter_tile_info.letter_ = ' ';
      }

      curr_body->SetTransform(WorldUtil::pixelToMeter(GetLocationFromIndex(tile->letter_tile_info.tile_holder_index_)), 0);
    }
  }

  int TileHolder::GetTileHolderValue() {
    int sum = 0;
    for (Tile* tile : holder_tiles_) {
      if (tile != nullptr) {
        sum += tile->letter_tile_info.value_;
      }
    }
    return sum;
  }
}