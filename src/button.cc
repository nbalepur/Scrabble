//
// Created by nishu on 4/26/2020.
//

#include "mylibrary/button.h"

#include <mylibrary/world_util.h>

namespace gui {

  Button::Button(b2Vec2 top_left_pos, int height, int width, const std::string& image_name, BUTTON_TYPE button_type) {
    /* creates the struct needed for the button to be drawn */
    cinder::Rectf rectangle(top_left_pos.x, top_left_pos.y, top_left_pos.x + (float) width, top_left_pos.y + (float) height);
    drawing_info_ = scrabble::DrawingInfo(image_name, kImagePrefix, kImageSuffix);
    drawing_info_.img_rectangle_ = rectangle;

    button_type_ = button_type;
  }


  scrabble::DrawingInfo Button::GetDrawingInfo() {
    return drawing_info_;
  }

  bool Button::IsClicked(const cinder::app::MouseEvent& event) {
    return scrabble::WorldUtil::PointWithinBound((float) event.getX(), (float) event.getY(),
           drawing_info_.img_rectangle_.x1, drawing_info_.img_rectangle_.x2,
           drawing_info_.img_rectangle_.y1, drawing_info_.img_rectangle_.y2);
  }

}