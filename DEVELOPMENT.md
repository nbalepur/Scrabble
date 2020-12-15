# Development

---

###**4/18/2020**

#####<u>Completed</u>
- Installed various CinderBlocks and Libraries to test and decided on Box2D
- Figured out general structure of bodies, fixtures and shapes, and added the ability for shapes to "glide" across the screen 
- Additionally, the user mouse input functionality was begun
#####<u>Future Goals</u>
- Fully understand how to use Box2D

---

###**4/19/2020**

#####<u>Completed</u>
- Fixed conversion of pixel coordinates to meter coordinates
- Added functionality for tiles to be dragged around the board
#####<u>Future Goals</u>
- Start object and overall project decomposition

---

###**4/20/2020**

#####<u>Completed</u>
- Worked on project structure, mainly for the board and tile classes
- Implemented functionality of board to be displayed on screen
#####<u>Future Goals</u>
- Begin the tile logic with physics bodies

---

###**4/21/2020**
#####<u>Completed</u>
- Began logic for letter tiles with physics bodies, as well as finalized the scrabble board format
#####<u>Future Goals</u>
- Start mouse movement logic

---

###**4/22/2020**
#####<u>Completed</u>
- Refactored a lot of logic into separate classes, such as one to function as the game engine and one to handle the mouse movement
- Multiple movement of tiles was implemented, and placement board logic was begun
#####<u>Future Goals</u>
- Finalize tile movement

---

###**4/24/2020**
#####<u>Completed</u>
- Refactored code from headers into their respective .cc files
- Tile movement logic was finalized. Bag class storing all of the available tiles was created
#####<u>Future Goals</u>
- Create tile holder class

---

###**4/25/2020**
#####<u>Completed</u>
- Finished most of the logic surrounding the tile holder
- The tile holder instantiates a bag which it uses to draw tiles
#####<u>Future Goals</u>
- Start creating buttons

---

###**4/26/2020**
#####<u>Completed</u>
- Logic surrounding tile placement was finalized
- Tiles can now be swapped within the tile holder
- Button class was created to be used instead of a GUI
- Submit button is correctly processed when clicked
#####<u>Future Goals</u>
- Begin handling logic when the buttons are pressed

---

###**4/27/2020**
#####<u>Completed</u>
- Actions are now correctly handled after the submit button is pressed
- The board class is able to detect whether the word played was valid or not
#####<u>Future Goals</u>
- Detection of invalid/valid moves

---

###**4/28/2020**
#####<u>Completed</u>
- Shuffle button was implemented and various edge cases were handled
- The appropriate actions are taken after the player makes an invalid move
- A text box was created to take the appropriate actions when the player does an invalid move
- Logic to start summing the value of the word(s) played was begun
#####<u>Future Goals</u>
- Finish summation of words

---

###**4/29/2020**
#####<u>Completed</u>
- Code was refactored and decomposed into smaller methods to get the word score
- The logic for the word score was finalized, fixing various bugs
- The game engine now keeps track of and displays how many points each player has
#####<u>Future Goals</u>
- Start creating blank tiles

---

###**4/30/2020**
#####<u>Completed</u>
- The option to exchange tiles was implemented
- Blank tile functionality was finalized, so all of the human gameplay is finished
- Starting writing more test cases, only to realize that the LoadImage method in cinder does not work in the test file
#####<u>Future Goals</u>
- Fix Test Cases
- Start bot implementation

---

###**5/1/2020**
#####<u>Completed</u>
- Refactored ScrabbleDictionary so information is now stored in a Trie, as opposed to a vector
- Creation, search, and word-generating methods were implemented for the Trie, drastically increasing the efficiency of the program
#####<u>Future Goals</u>
- Start working on bot implementation
- Add edge cases for calculating

---

###**5/2/2020**
#####<u>Completed</u>
- Bot implementation was heavily worked on
- The bot is able to play moves with minimal mistakes
- Heavy debugging was performed, fixing issues with the TileOpener struct and delimiter logic
- Known issue with TileConnectivity was started to be addressed
#####<u>Future Goals</u>
- Heavy debugging

---

###**5/3/2020**
#####<u>Completed</u>
- Many different edge cases were tested for and fixed by adding conditions to if statements
#####<u>Future Goals</u>
- Implement blank tiles for the AI

---

###**5/4/2020**
#####<u>Completed</u>
- Blank tile logic was fully implemented into the game
- More errors were fixed revolving around the storage of words in the game
- The end game implementation was begun
#####<u>Future Goals</u>
- Find more bugs to be addressed
- Finish the end game

---

###**5/5/2020**
#####<u>Completed</u>
- More bugs about the board tile placement were fixed
- The ending for the game was finalized, and players now have the option to restart the game when there are no more tiles
- AI logic was redone so it prioritizes using all of its tiles
#####<u>Future Goals</u>
- Add more test cases
- Finish README