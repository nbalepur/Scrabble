//
// Created by nishu on 4/27/2020.
//

#ifndef FINALPROJECT_SCRABBLE_DICTIONARY_H
#define FINALPROJECT_SCRABBLE_DICTIONARY_H

#include <iosfwd>
#include <string>
#include <vector>

namespace scrabble {

  class ScrabbleDictionary {

   private:

    static const int kAlphabetSize = 26;

    /* trie info */
    /* help from: https://www.geeksforgeeks.org/print-valid-words-possible-using-characters-array/ */
    /* help from: https://www.geeksforgeeks.org/trie-insert-and-search/ */

    /**
     * Struct to store information about the node of the Trie
     */
    struct TrieNode {
      /* contains the children nodes for the node */
      TrieNode* child_nodes_[kAlphabetSize];
      /* whether or not this node is a leaf */
      bool is_leaf_;
    };

    TrieNode* root_;

    /**
     * Gets a new node for the Trie
     * Initializes all of the new node's children to nullptr
     * @return the new node
     */
    TrieNode* GetNewNode() {
      auto* new_node = new TrieNode;
      new_node->is_leaf_ = false;
      for (auto& child_node : new_node->child_nodes_) {
        child_node = nullptr;
      }
      return new_node;
    }

    /**
     * Inserts a string into the tree
     * @param root_node the root node of the Trie
     * @param word the string (C-Style string)
     */
    void InsertNode(const std::string& word) {
      /* initializes variables */
      TrieNode* current_node = root_;

      for (char word_char : word) {
        /* converts the char into an index that can be used by the array */
        int current_index = word_char - 'A';
        /* correctly sets the child node if there is none */
        if (!current_node->child_nodes_[current_index]) {
          current_node->child_nodes_[current_index] = GetNewNode();
        }
        current_node = current_node->child_nodes_[current_index];
      }
      /* sets the leaf node to true */
      current_node->is_leaf_ = true;
    }

    /**
     * Input operator to convert a file into a Trie
     * @param input the input file
     * @param dictionary the dictionary object for the information to be stored into
     * @return the input
     */
    friend std::istream& operator >> (std::istream& input, ScrabbleDictionary& dictionary);

    /**
     * Helper method to recursively search for the possible word
     * @param current_node the current node of the Trie
     * @param letter_freq vector containing the frequencies of each letter and the blank
     * @param word the word
     * @param possible_words vector of possible words
     * @param delimiter the character that must be in the word
     */
    void SearchPossibleWord(TrieNode* current_node, std::vector<int> letter_freq, const std::string& word, std::vector<std::string>& possible_words, char delimiter);

   public:
    /**
     * Checks if the word is in the Trie
     * @param word the word to be searched for
     * @return true/false if the word exists in the dictionary
     */
    bool FindWord(std::string word);

    /**
     * Recursively gets all the possible words that can be made from a vector of characters
     * @param letter_freq the vector containing the frequencies of each letter and blank tiles
     * @param delimiter the character that must be in the word
     * @return a vector of all the possible words that can be made from the letter frequencies
     */
    std::vector<std::string> GetAllPossibleWords(std::vector<int> letter_freq, char delimiter);
  };
}

#endif  // FINALPROJECT_SCRABBLE_DICTIONARY_H
