# Scrabble

Author: Nishant Balepur

---

I am aiming to recreate the classic game of Scrabble. Since playing the game with two humans would require some sort of
web server, I will create a bot to play against the player. I will start by creating my own version of Scrabble, and 
then adding to it based on how much time I have left.

I want to do this project because I feel it combines a lot of subjects that I have been learning throughout the year,
in this class and in other classes. I will have to implement my own object decomposition, algorithms, and possibly data
structures to complete this assignment, all of which I have been working on in different classes. Additionally, I am part
of the ACM club SIGNLL, and if I have enough time to make a simple AI, I will be using some basic elements of natural
language processing, something that I am interested in and want to explore.

To create this game, most of the logic can be completed just by using cinder. However, something that might cause me 
some trouble will be moving the tiles, so I will need to use a Physics engine. The cinder block [Box2d](https://github.com/cinder/Cinder/tree/master/blocks/Box2D)
will let me accomplish this, as I can use the blackbox joints and bodies of the library to move the tiles without completely
knowing how everything works. I will also need a list of words that are valid in the [Scrabble Dictionary](https://boardgames.stackexchange.com/questions/38366/latest-collins-scrabble-words-list-in-text-file)

During the first week, I want to begin the setup of the board and tiles, as well as complete most of the logic to determine
if words are valid, if they are placed correctly, etc. During week 2, I want to finish most of the gameplay and try to fix
any bugs that might be in the program. For the last week, I want to try and implement an AI to play against the player. 
I think that my goals for week 3 might be a stretch, and it's possible that the goals I have for weeks 1 and 2 will extend into week 3