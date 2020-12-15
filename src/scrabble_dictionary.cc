//
// Created by nishu on 4/27/2020.
//

#include "mylibrary/scrabble_dictionary.h"

#include <algorithm>
#include <chrono>
#include <iostream>

using namespace scrabble;

std::istream& scrabble::operator>>(std::istream& input,
                                   scrabble::ScrabbleDictionary& dictionary) {
  /* sets the root of the Trie */
  dictionary.root_ = dictionary.GetNewNode();

  /* declares the string and gets the first two lines, as they are not used */
  std::string scrabble_word;
  std::getline(input, scrabble_word);
  std::getline(input, scrabble_word);

  /* iterates through each word while the program has not reached the end of the file */
  do {
    std::getline(input, scrabble_word);
    /* inserts the word into the Trie */
    dictionary.InsertNode(scrabble_word);
  } while (!input.eof());

  return input;
}

bool ScrabbleDictionary::FindWord(std::string word) {
  /* transforms the string to uppercase */
  std::transform(word.begin(), word.end(), word.begin(), ::toupper);
  /* sets the current node to the root of the Trie */
  TrieNode* current_node = root_;
  /* iterates through each character in the word and checks if they match */
  for (char word_char : word) {
    int current_index = word_char - 'A';
    /* if the next node is null and it still doesn't match, the word is not in the Trie */
    if (!current_node->child_nodes_[current_index]) {
      return false;
    }
    /* iterates the node */
    current_node = current_node->child_nodes_[current_index];
  }
  /* the word exists if we are on a leaf node */
  return (current_node != nullptr && current_node->is_leaf_);
}

std::vector<std::string> ScrabbleDictionary::GetAllPossibleWords(std::vector<int> letter_freq, char delimiter) {
  /* declares the list of possible words and sets the current node to the root */
  std::vector<std::string> possible_words;
  TrieNode* current_node = root_;
  /* declares the word to be checked */
  std::string possible_word;
  /* iterates through each letter */
  for (int letter_index = 0; letter_index < kAlphabetSize; letter_index++) {
    /* checks if the frequency of this letter is greater than 0 and if the next node isn't null */
    if (letter_freq[letter_index] > 0 && current_node->child_nodes_[letter_index]) {
      /* adds the character to the possible word and decrements the frequency of that letter */
      possible_word += std::string(1, (char) (letter_index + 'A'));
      letter_freq[letter_index] = letter_freq[letter_index] - 1;
      /* searches for the possible word starting with that letter (recursive helper method) */
      SearchPossibleWord(current_node->child_nodes_[letter_index], letter_freq, possible_word, possible_words, delimiter);
      /* afterwards, the frequency is incremented so it can be reused */
      letter_freq[letter_index] = letter_freq[letter_index] + 1;
      /* resets the possible word string */
      possible_word = "";
    }
    /* searches for the word with the blank tile if it exists */
    if (letter_freq[kAlphabetSize] > 0 && current_node->child_nodes_[letter_index]) {
      possible_word += std::string(1, (char) (letter_index + 'a'));
      letter_freq[kAlphabetSize] = letter_freq[kAlphabetSize] - 1;
      SearchPossibleWord(current_node->child_nodes_[letter_index], letter_freq, possible_word, possible_words, delimiter);
      letter_freq[kAlphabetSize] = letter_freq[kAlphabetSize] + 1;
      possible_word = "";
    }
  }

  return possible_words;
}

void ScrabbleDictionary::SearchPossibleWord(TrieNode* current_node, std::vector<int> letter_freq,
                                            const std::string& word, std::vector<std::string>& possible_words, char delimiter) {
  /* if the current node is a leaf, the word exists so it is pushed back to the vector reference */
  if (current_node->is_leaf_ && word.find(toupper(delimiter)) != std::string::npos) {
    possible_words.push_back(word);
  }
  /* iterates through each letter */
  for (int letter_index = 0; letter_index < kAlphabetSize; letter_index++) {
    /* checks if the letter's frequency is greater than zero and that the Trie continues */
    if (letter_freq[letter_index] > 0 && current_node->child_nodes_[letter_index] != nullptr) {
      /* decrements the frequency of the letter and searches for more words */
      letter_freq[letter_index] = letter_freq[letter_index] - 1;
      SearchPossibleWord(current_node->child_nodes_[letter_index], letter_freq, word + std::string(1, (char)(letter_index + 'A')), possible_words, delimiter);
      /* increments the frequency of the letter as it can be used again */
      letter_freq[letter_index] = letter_freq[letter_index] + 1;
    }
    /* searches for the word with the blank tile if there exists a blank tile */
    if (letter_freq[kAlphabetSize] > 0 && current_node->child_nodes_[letter_index]) {
      letter_freq[kAlphabetSize] = letter_freq[kAlphabetSize] - 1;
      SearchPossibleWord(current_node->child_nodes_[letter_index], letter_freq, word + std::string(1, (char)(letter_index + 'a')), possible_words, delimiter);
      letter_freq[kAlphabetSize] = letter_freq[kAlphabetSize] + 1;
    }
  }
}
