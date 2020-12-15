//
// Created by nishu on 4/22/2020.
//

#include "mylibrary/game_engine.h"

#include <utility>

namespace scrabble {

  scrabble::GameEngine::GameEngine() {
    /* initializes the physics engine world */
    SetUpWorld();

    //TODO: GET RID OF MAGIC NUMBERS

    /* initializes the scrabble board */
    board_ = new Board(kBoardVertOffset, kBoardHorizOffset);

    /* initializes the bag */
    bag_ = new Bag();

    /* initializes the tile holder */
    b2Vec2 tile_holder_loc = b2Vec2(kTileHolderX, kTileHolderY);
    tile_holder_ = new TileHolder(world_, tile_holder_loc, bag_);
    tile_holder_->DrawTiles(kHolderTileCount);

    /* initializes the AI player */
    ai_ = new AI(bag_, board_);
    ai_->DrawTiles(kHolderTileCount);

    /* initializes the text boxes and adds them to the vector */
    message_text_box_ = new gui::TextBox(b2Vec2(kTextBoxX, kTextBoxY), kTextBoxHeight, kTextBoxWidth);
    message_text_box_->SetText("Welcome to Scrabble!");

    /* initializes the scoreboard */
    score_board_ = new ScoreBoard(cinder::vec2(kTextBoxX, kScoreboardY), kScoreboardFontSize, cinder::Color::white());

    /* initializes the mouse handler */
    mouse_handler_ = new MouseHandler(world_, board_, tile_holder_, message_text_box_);


    /* initializes the buttons and adds them to the vector */
    auto* submit_btn = new gui::Button(b2Vec2(kSubmitBtnX, kSubmitBtnY), kRectButtonHeight, kRectButtonWidth, "play-word", gui::BUTTON_TYPE::PLAY_WORD);
    auto* shuffle_tiles_btn = new gui::Button(b2Vec2(kShuffleBtnX, kShuffleBtnY), kSquareButtonDim, kSquareButtonDim, "shuffle-tiles", gui::BUTTON_TYPE::SHUFFLE_TILES);
    auto* exchange_tiles_btn = new gui::Button(b2Vec2(kExchangeBtnX, kExchangeBtnY), kRectButtonHeight, kRectButtonWidth, "exchange-tiles", gui::BUTTON_TYPE::EXCHANGE_TILES);

    buttons_.push_back(submit_btn);
    buttons_.push_back(shuffle_tiles_btn);
    buttons_.push_back(exchange_tiles_btn);
  }

  void GameEngine::RunGame() {

    /* checks the end state of the game */
    if (bag_->GameIsOver()) {
      if (!message_text_box_->IsFinalScore()) {
        SetUpEndGame();
        return;
      } else if (message_text_box_->IsFinalScore()) {
        EndGame();
      }
    }

    /* the AI plays its turn accordingly */
    else if (!score_board_->IsHumanTurn()) {
      if (!scoreboard_drawn_) {
        message_text_box_->SetPlayer(score_board_->GetScoreboardDraw()->player_one_name_);
        message_text_box_->SetText(board_->GetWordsPlayed(),
                                   board_->GetTurnScore());
        scoreboard_drawn_ = true;
      } else {
        HandlePlayWordClick();
      }
    }

    /* handles all of the mouse information otherwise */
    mouse_handler_->UpdateMovedTiles();

    /* the physics engine takes a step in the world */
    world_->Step(kTimeStep, 0, 0);
  }

  void GameEngine::EndGame() {
    /* the game should only end if the yes/no message is displayed */
    if (!display_yes_no_message_) {
      return;
    }
    /* gets the end code and takes the appropriate action */
    YES_NO_CODE end_code = mouse_handler_->HandleYesNoInput();
    switch (end_code) {
      case YES_NO_CODE::YES:
        Reset();
        break;
      case YES_NO_CODE::NO:
        exit(1);
      case YES_NO_CODE::NONE:
        break;
    }
  }

  void GameEngine::Reset() {
    /* resets the world for new physics bodies to be generated */
    delete world_;
    world_ = new b2World(b2Vec2(0, 0));

    /* resets the board */
    delete board_;
    board_ = new Board(250, 50);

    /* resets the bag */
    delete bag_;
    bag_ = new Bag();

    /* resets the tile holder */
    delete tile_holder_;
    b2Vec2 tile_holder_loc = b2Vec2(kTileHolderX, kTileHolderY);
    tile_holder_ = new TileHolder(world_, tile_holder_loc, bag_);
    tile_holder_->DrawTiles(kHolderTileCount);

    /* initializes the AI player */
    delete ai_;
    ai_ = new AI(bag_, board_);
    ai_->DrawTiles(kHolderTileCount);

    /* resets the text box message */
    message_text_box_->SetText("Welcome to Scrabble!");
    message_text_box_->SetPlayer("(You)");

    /* resets the scoreboard, text box */
    score_board_->Reset();
    message_text_box_->Reset();

    /* resets the mouse handler */
    delete mouse_handler_;
    mouse_handler_ = new MouseHandler(world_, board_, tile_holder_, message_text_box_);

    /* resets appropriate booleans */
    display_yes_no_message_ = false;
  }

  void GameEngine::SetUpEndGame() {
    std::vector<int> end_score = score_board_->SubtractEndScores(tile_holder_->GetTileHolderValue(), ai_->GetTileHolderValue());
    message_text_box_->SetText(end_score[0], end_score[1]);
  }

  void GameEngine::SetUpWorld() {
    world_ = new b2World(b2Vec2(0, 0));
  }

  void GameEngine::HandleButtonClicks(cinder::app::MouseEvent event) {
    /* switch cases through all the types of buttons */
    switch (mouse_handler_->GetButtonTypePressed(std::move(event), buttons_)) {
      case gui::BUTTON_TYPE::NONE:
        break;
      case gui::BUTTON_TYPE::PLAY_WORD: {
        HandlePlayWordClick();
        break;
      }
      case gui::BUTTON_TYPE::SHUFFLE_TILES: {
        tile_holder_->ShuffleTiles();
        break;
      }
      case gui::BUTTON_TYPE::EXCHANGE_TILES: {
          tile_holder_->ExchangeTiles();
          message_text_box_->SetText(gui::BUTTON_TYPE::EXCHANGE_TILES);
          score_board_->UpdateScores(0);
          break;
      }
    }
  }

  void GameEngine::HandlePlayWordClick() {
    TILE_ERROR_CODE error_code;
    /* checks if the player is human, if so, it does the human actions */
    if (score_board_->IsHumanTurn() && !bag_->GameIsOver()) {
      error_code = tile_holder_->HandleTilesOnBoard(board_);
      scoreboard_drawn_ = false;
    } else {
      error_code = ai_->PlayWord();
      message_text_box_->SetPlayer(score_board_->GetScoreboardDraw()->player_two_name_);
      /* condition for when the bot played a word */
      if (!bag_->GameIsOver() && error_code == TILE_ERROR_CODE::NO_ERROR) {
        /* sleeps so the player can read the scoreboard */
        std::this_thread::sleep_for(std::chrono::seconds(4));
        message_text_box_->SetText(board_->GetWordsPlayed(),
                                   board_->GetTurnScore());
      } else if (!bag_->GameIsOver()) {
        /* condition for when the bot chose to exchange */
        message_text_box_->SetText(gui::BUTTON_TYPE::EXCHANGE_TILES);
        score_board_->UpdateScores(0);
      }
    }
    /* analyzes the error code where appropriate */
    if (!bag_->GameIsOver() && error_code != TILE_ERROR_CODE::NO_ERROR) {
      message_text_box_->SetText(error_code);
    } else if (!bag_->GameIsOver()) {
      score_board_->UpdateScores(board_->GetTurnScore());
    }
  }
}
