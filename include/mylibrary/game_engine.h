//
// Created by nishu on 4/22/2020.
//

#ifndef FINALPROJECT_GAME_ENGINE_H
#define FINALPROJECT_GAME_ENGINE_H

#include <Box2D/Box2D.h>

#include "ai.h"
#include "bag.h"
#include "board.h"
#include "button.h"
#include "mouse_handler.h"
#include "scoreboard.h"
#include "text_box.h"
#include "tile_holder.h"
#include "world_util.h"

namespace scrabble {

  class GameEngine {

   private:

    /** position constants **/
    const int kBoardVertOffset = 250;
    const int kBoardHorizOffset = 50;

    const int kTileHolderX = 640;
    const int kTileHolderY = 1600;

    const int kTextBoxX = 1825;
    const int kTextBoxY = 50;
    const int kTextBoxHeight = 300;
    const int kTextBoxWidth = 600;

    const int kScoreboardY = 400;
    const int kScoreboardFontSize = 60;

    const int kRectButtonHeight = 50;
    const int kRectButtonWidth = 250;
    const int kSquareButtonDim = 75;

    const int kSubmitBtnX = 735;
    const int kSubmitBtnY = 1800;

    const int kShuffleBtnX = 1400;
    const int kShuffleBtnY = 1635;

    const int kExchangeBtnX = 1015;
    const int kExchangeBtnY = 1800;

    /** physics engine **/
    const float kTimeStep = (1.0f / 60.0f);

    /** information about the state of the game **/
    Board* board_;
    MouseHandler* mouse_handler_;
    b2World* world_;

    /** the human player information **/
    TileHolder* tile_holder_;
    /** the AI player information **/
    AI* ai_;

    /** boolean state information **/
    bool scoreboard_drawn_ = false;
    bool display_yes_no_message_ = false;

    /** gui info **/
    std::vector<gui::Button*> buttons_;
    gui::TextBox* message_text_box_;

    /** scoreboard containing the player scores **/
    ScoreBoard* score_board_;

    /** contains the bag from where the tiles are drawn from **/
    Bag* bag_;

    /**
     * Sets the text box to the correct message
     * Updates the scores, calculating points in other classes
     */
    void HandlePlayWordClick();

    /**
     * Initializes the world
     */
    void SetUpWorld();

    /**
     * Sets up the variables needed to end the game
     */
    void SetUpEndGame();

    /**
     * Code to process the user's input for what to do at the end of the game
     */
    void EndGame();

    /**
     * Resets all variables needed to end the game
     */
    void Reset();

   public:

    /**
     * Sets up the scrabble game
     */
    explicit GameEngine();

    /**
     * Handles most of the physics logic
     */
    void RunGame();

    /**
     * Handles the information when a button is clicked
     * @param event object containing the information of what the mouse did
     */
    void HandleButtonClicks(cinder::app::MouseEvent event);

    /* ---------------------- getters and setters --------------------- */
    std::vector<std::vector<Tile*>> GetBoard() { return board_->GetBoard(); }

    b2World* GetWorld() { return world_; }

    MouseHandler* GetMouseHandler() { return mouse_handler_; }

    DrawingInfo GetTileHolderDraw() { return tile_holder_->GetDrawingInfo(); };

    std::vector<gui::Button*> GetButtons() { return buttons_; }

    gui::TextBox* GetTextBox() { return message_text_box_; };

    ScoreBoard* GetScoreBoard() { return score_board_; };

    void SetScoreboardDrawn(bool is_drawn) { scoreboard_drawn_ = is_drawn; };

    bool IsGameOver() { return bag_->GameIsOver(); };

    bool IsFinalScore() { return message_text_box_->IsFinalScore(); };

    void SetShouldDisplayYesNo(bool set_display) { display_yes_no_message_ = set_display; };
  };

}

#endif  // FINALPROJECT_GAME_ENGINE_H
