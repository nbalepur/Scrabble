//
// Created by nishu on 4/26/2020.
//

#ifndef FINALPROJECT_BUTTON_H
#define FINALPROJECT_BUTTON_H

#include <cinder/gl/wrapper.h>

#include "Box2D/Box2D.h"
#include "tile.h"

namespace gui {

enum class BUTTON_TYPE {PLAY_WORD, SHUFFLE_TILES, EXCHANGE_TILES, NONE};

  class Button {

   private:
    /** the constants to render the image **/
    const std::string kImagePrefix = "images/buttons/";
    const std::string kImageSuffix = ".png";

    /** struct containing what to draw **/
    scrabble::DrawingInfo drawing_info_;

    /** enum denoting the type of button **/
    BUTTON_TYPE button_type_;

   public:
    /**
     * Initializes values to be used in drawing
     * @param top_left_pos the top left position of the button
     * @param height height in pixels
     * @param width width in pixels
     * @param image_name the name of the image to rendered
     * @param button_type the type of the button
     */
    explicit Button(b2Vec2 top_left_pos, int height, int width, const std::string& image_name, BUTTON_TYPE button_type);


    /* ------------------ getters and setters ------------------ */

    scrabble::DrawingInfo GetDrawingInfo();

    bool IsClicked(const cinder::app::MouseEvent& event);

    BUTTON_TYPE GetButtonType() { return button_type_; };
  };
}

#endif  // FINALPROJECT_BUTTON_H
