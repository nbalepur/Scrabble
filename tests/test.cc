// Copyright (c) 2020 CS126SP20. All rights reserved.

#define CATCH_CONFIG_MAIN

#include <Box2D/Common/b2Math.h>
#include <cinder/Rand.h>
#include <cinder/app/App.h>
#include <cinder/app/AppBase.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/wrapper.h>
#include <mylibrary/bag.h>
#include <mylibrary/board.h>
#include <mylibrary/button.h>
#include <mylibrary/example.h>
#include <mylibrary/game_engine.h>
#include <mylibrary/mouse_handler.h>
#include <mylibrary/scoreboard.h>
#include <mylibrary/text_box.h>
#include <mylibrary/tile.h>
#include <mylibrary/tile_data.h>
#include <mylibrary/tile_holder.h>
#include <mylibrary/world_util.h>
#include <cinder/app/App.h>
#include <cinder/app/RendererGl.h>

#include <catch2/catch.hpp>

using cinder::app::KeyEvent;
using cinder::ImageSourceRef;
using cinder::gl::Texture2dRef;
using cinder::gl::Texture2d;
using cinder::app::loadAsset;

/* ---------------------- World Util Tests ---------------------- */

TEST_CASE("Pixel to meter conversion", "[World-Util]") {
  b2Vec2 pixel_pos = b2Vec2(1900, 300);
  REQUIRE(scrabble::WorldUtil::pixelToMeter(pixel_pos).x == 19.0f);
  REQUIRE(scrabble::WorldUtil::pixelToMeter(pixel_pos).y == 3.0f);
}

TEST_CASE("Meter to pixel conversion", "[World-Util]") {
  b2Vec2 meter_pos = b2Vec2(4, -1);
  REQUIRE(scrabble::WorldUtil::meterToPixel(meter_pos).x == 400.0f);
  REQUIRE(scrabble::WorldUtil::meterToPixel(meter_pos).y == -100.0f);
}

TEST_CASE("Inside of Area Bound", "[World-Util]") {
  REQUIRE(scrabble::WorldUtil::PointWithinBound(10, 10, 0, 100, 0, 100));
}

TEST_CASE("Outside of Area Bound", "[World-Util]") {
  REQUIRE(!scrabble::WorldUtil::PointWithinBound(10, 10, 0, 150, 0, -100));
}

/* ---------------------- Trie creation in Scrabble Dictionary ---------------------- */

TEST_CASE("Random words exist/do not exist in the Trie", "[ScrabbleDictionary]") {
  using namespace scrabble;

  ScrabbleDictionary dictionary;
  std::string string;
  std::ifstream infile(R"(C:\Cinder\my-projects\final-project-nbalepur\data\scrabble-words.txt)");
  std::istream& stream = infile;
  stream >> dictionary;

  SECTION("Words exist", "[Trie][Find]") {
    REQUIRE(dictionary.FindWord("contemplate"));
    REQUIRE(dictionary.FindWord("CHEESE"));
    REQUIRE(dictionary.FindWord("DrAmAtIc"));
    REQUIRE(dictionary.FindWord("overWHELMED"));
    REQUIRE(dictionary.FindWord("aaRDvark"));
  }

  SECTION("Words don't exist", "[Trie][Find]") {
    REQUIRE(!dictionary.FindWord("spoggy"));
    REQUIRE(!dictionary.FindWord("DJANOG"));
    REQUIRE(!dictionary.FindWord("computERScience"));
    REQUIRE(!dictionary.FindWord("lermPED"));
    REQUIRE(!dictionary.FindWord("jayz"));
  }
}

TEST_CASE("Generating all possible words with the Trie", "[ScrabbleDictionary]") {
  using namespace scrabble;

  ScrabbleDictionary dictionary;
  std::string string;
  std::ifstream infile(R"(C:\Cinder\my-projects\final-project-nbalepur\data\scrabble-words.txt)");
  std::istream& stream = infile;
  stream >> dictionary;

  SECTION("Simple three letter example", "[GetAllPossibleWords]") {
    std::vector<int> letter_count({1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0});
    std::vector<std::string> all_words = dictionary.GetAllPossibleWords(letter_count, 'a');

    REQUIRE(all_words[0] == "ACT");
    REQUIRE(all_words[1] == "AT");
    REQUIRE(all_words[2] == "CAT");
    REQUIRE(all_words[3] == "TA");
  }

  SECTION("Simple two letter example with one blank tile", "[GetAllPossibleWords]") {
    std::vector<int> letter_count({0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1});
    std::vector<std::string> all_words = dictionary.GetAllPossibleWords(letter_count, 'j');

    REQUIRE(all_words[0] == "Ja");
    REQUIRE(all_words[1] == "JaW");
    REQUIRE(all_words[2] == "JeW");
    REQUIRE(all_words[3] == "Jo");
    REQUIRE(all_words[4] == "JoW");
  }

  SECTION("Simple one letter example with two blank tiles") {
    std::vector<int> letter_count({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2});
    std::vector<std::string> all_words = dictionary.GetAllPossibleWords(letter_count, 'q');

    REQUIRE(all_words[0] == "Qat");
    REQUIRE(all_words[1] == "Qi");
    REQUIRE(all_words[2] == "Qin");
    REQUIRE(all_words[3] == "Qis");
    REQUIRE(all_words[4] == "Qua");
    REQUIRE(all_words[5] == "suQ");
  }
}

/* ---------------------- Sorting Tests ---------------------- */

TEST_CASE("Sorting algorithm for vectors", "[Sort]") {
  using namespace scrabble;
  SECTION("Sorting vectors by x values", "[SortByX]") {
    std::vector<b2Vec2> vec({b2Vec2(1, 3), b2Vec2(-1, 3), b2Vec2(0, 3)});
    std::vector<b2Vec2> correct_vec({b2Vec2(-1, 3), b2Vec2(0, 3), b2Vec2(1, 3)});
    std::sort(vec.begin(), vec.end(), SortByX());

    REQUIRE(vec == correct_vec);
  }

  SECTION("Sorting vectors by y values", "[SortByY]") {
    std::vector<b2Vec2> vec({b2Vec2(0, -2), b2Vec2(0, -3), b2Vec2(0, 10)});
    std::vector<b2Vec2> correct_vec({b2Vec2(0, -3), b2Vec2(0, -2), b2Vec2(0, 10)});
    std::sort(vec.begin(), vec.end(), SortByY());

    REQUIRE(vec == correct_vec);
  }
}

TEST_CASE("Sorting words by correct point values", "[Sort]") {
  using namespace scrabble;

  SECTION("Sorting words by base point value, same length", "[SortByBaseWordScore]") {
    std::vector<std::string> words({"DOG", "JAW", "EAT"});
    std::vector<std::string> correct_words({"JAW", "DOG", "EAT"});
    std::sort(words.begin(), words.end(), AI::SortByBaseWordScore());

    REQUIRE(words == correct_words);
  }

  SECTION("Sorting words with same base points, different lengths", "[SortByBaseWordScore]") {
    std::vector<std::string> words({"DRAGGED", "QI", "CHAP"});
    std::vector<std::string> correct_words({"DRAGGED", "CHAP", "QI"});
    std::sort(words.begin(), words.end(), AI::SortByBaseWordScore());

    REQUIRE(words == correct_words);
  }

  SECTION("Sorting words when one uses all letters", "[SortByBaseWordScore]") {
    std::vector<std::string> words({"SEAWEEDS", "QUALMS", "ZYGOTE"});
    std::vector<std::string> correct_words({"SEAWEEDS", "ZYGOTE", "QUALMS"});
    std::sort(words.begin(), words.end(), AI::SortByBaseWordScore());

    REQUIRE(words == correct_words);
  }

  SECTION("Sorting words with blank tiles", "[SortByBaseWordScore]") {
    std::vector<std::string> words({"SECTIONs", "QUALM", "QUALm", "QUALms"});
    std::vector<std::string> correct_words({"SECTIONs", "QUALM", "QUALms", "QUALm"});
    std::sort(words.begin(), words.end(), AI::SortByBaseWordScore());

    REQUIRE(words == correct_words);
  }
}

/* ---------------------- Note that all rendering crashes the program, so much of the project was unable to be tested, since the Tile constructor renders an image ---------------------- */

/*TEST_CASE("Board tiles have been deserialized correctly", "[Board-Tile]") {
  cinder::ImageSourceRef img = cinder::loadImage("C:\\Cinder\\my-projects\\final-project-nbalepur\\tests\\assets\\tile-holder.png");
  cinder::gl::Texture2dRef a = cinder::gl::Texture2d::create(img);
} */


