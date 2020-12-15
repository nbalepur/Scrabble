//
// Created by nishu on 4/19/2020.
//

#include "mylibrary/tile.h"

#include <Box2D/Dynamics/b2World.h>
#include <mylibrary/game_engine.h>
#include <mylibrary/tile_data.h>
#include <mylibrary/world_util.h>

using namespace scrabble;

namespace scrabble {

  Tile::Tile(const BOARD_TILE& board_tile) {
    auto* tile_data = new TileData();
    board_tile_info = tile_data->GetBoardTileInfo(board_tile);
    drawing_info = tile_data->GetBoardDrawingInfo(board_tile);
    delete tile_data;

    /* sets the tile type */
    tile_type_ = TILE_TYPE::BOARD;
  }

  Tile::Tile(const char letter_tile) {
    if (letter_tile == ' ') {
      drawing_info = DrawingInfo("blank", "images/board-tiles/", ".png");
    } else {
      drawing_info = DrawingInfo(std::string(1, letter_tile), "images/board-tiles/", ".png");
    }

    /* sets the tile type */
    tile_type_ = TILE_TYPE ::LETTER;
  }

  void Tile::SetTileLocation(int set_row, int set_col, int row_off, int col_off) {
    cinder::Rectf tile_rectangle(row_off + kTileSize * set_col, col_off + kTileSize * set_row,
      row_off + kTileSize * (set_col + 1), col_off + kTileSize * (set_row + 1));
      drawing_info.img_rectangle_ = tile_rectangle;
  }

  void Tile::CreatePhysicsBody(b2World* world, int x_pixel_loc, int y_pixel_loc) {
    /* defining the body */
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;

    /* sets the user data to a pointer of the tile */
    Tile* this_tile = this;
    bodyDef.userData = this_tile;

    b2Vec2 center_pixel(x_pixel_loc, y_pixel_loc);
    b2Vec2 center_meter = WorldUtil::pixelToMeter(center_pixel);

    bodyDef.position.Set(center_meter.x, center_meter.y);

    /* creating the body */
    b2Body* body = world->CreateBody(&bodyDef);

    /* creating the shape/box */
    b2PolygonShape shape;
    b2Vec2 box_size_pixel(kTileSize, kTileSize);
    b2Vec2 box_size_meter = WorldUtil::pixelToMeter(box_size_pixel);

    shape.SetAsBox(box_size_meter.x, box_size_meter.y);

    /* creating the fixture */
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;

    /* using the fixture to attach the shape to the body */
    body->CreateFixture(&fixtureDef);
  }
}
