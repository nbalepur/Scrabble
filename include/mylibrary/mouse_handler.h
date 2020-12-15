//
// Created by nishu on 4/22/2020.
//

#ifndef FINALPROJECT_MOUSE_HANDLER_H
#define FINALPROJECT_MOUSE_HANDLER_H

#include <Box2D/Box2D.h>
#include <cinder/app/MouseEvent.h>

#include <utility>
#include <vector>

#include "board.h"
#include "button.h"
#include "scoreboard.h"
#include "text_box.h"
#include "tile.h"
#include "tile_holder.h"
#include "world_util.h"

namespace scrabble {

  /**
   * enums for the end of the game
   */
  enum class YES_NO_CODE { YES, NO, NONE };

  /**
   * Handles information involving the mouse
   * This includes: moving the tiles, setting the tiles to the board when moved
   */
  class MouseHandler {

   private:
    /** objects needed for the mouse_handler to updated **/
    b2World* world_;
    b2Body* current_body_;
    Board* board_;
    TileHolder* tile_holder_;

    /** used to set messages to prompt the user **/
    gui::TextBox* message_text_box_;

    /** location of the tile holder **/
    b2Vec2 holder_location_;

    /** information about mouse placement **/
    int mouse_down_index_ = -1;
    b2Vec2 mouse_pos_;

    /** information about the blank tile prompt **/
    char current_letter_down_ = ' ';
    bool blank_tile_placed_ = false;

    /**
     * Handles what to do when the tile is placed on the holder
     * The tile will swap with the tile it is dragged over
     */
    void HandleHolderPlacement();

    /**
     * Handles what to do when the tile is placed in an invalid location
     * The tile will be set back to its original holder location
     */
    void HandleInvalidPlacement();

    /**
     * Handles what to do when the tile is placed on the board
     * The tile will be snapped to the location of the tile
     * @param event the object storing information of what the mouse did
     * @param top_left_x the top left corner x position of the board
     * @param top_left_y the top left corner y position of the board
     */
    void HandleBoardPlacement(const cinder::app::MouseEvent& event, int top_left_x, int top_left_y);

    /**
     * Prompts the player to set the value of the blank tile
     */
    void SetValueBlankTile();

   public:
    explicit MouseHandler(b2World* set_world, Board* set_board, TileHolder* holder, gui::TextBox* text_box);

    /**
     * Moves the tiles if they were dragged
     * @param mouse_down_index the index of the tile the mouse is on
     */
    void UpdateMovedTiles();

    /**
     * Updates the mouse index
     * Checks each tile to see if the cursor location is within the area of a tile
     * Only runs when the mouse is clicked
     * @param event the mouse event storing information of what the mouse did
     */
    void UpdateMouseIndexDown(cinder::app::MouseEvent event);

    /**
     * Sets the mouse_down_index to -1, as no tile is selected
     * Additionally checks to see where the tile was placed
     * If placed outside the board, it goes back to its original position
     * Otherwise, it will place the tile accordingly on the board and free memory
     *
     * @param event object storing information about the mouse
     */
    void UpdateMouseIndexUp(const cinder::app::MouseEvent& event);

    /**
     * Changes the position of the mouse
     * @param event mouse event storing information of what the mouse did
     */
    void UpdateMousePosMoved(const cinder::app::MouseEvent& event);

    /**
    * Handles whether the user typed yes or no for the end of the game
    */
    YES_NO_CODE HandleYesNoInput();

    /**
     * Gets the button that was pressed
     * @param event object containing mouse information (position)
     * @param buttons the list of buttons on the screen
     * @return the mouse code associated with the button
     */
    gui::BUTTON_TYPE GetButtonTypePressed(const cinder::app::MouseEvent& event, const std::vector<gui::Button*>& buttons);

    /* --------------- getters and setters --------------- */

    bool IsBlankTilePlaced() { return blank_tile_placed_; };

    void SetCurrentChar(char set_char) { current_letter_down_ = set_char; };

  };
}

#endif  // FINALPROJECT_MOUSE_HANDLER_H
