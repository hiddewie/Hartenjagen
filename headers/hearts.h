#ifndef HEARTS_H
#define HEARTS_H

#include <string>
#include <vector>
#include <iostream>

#define VERBOSE false
#define ERROR false
#define ROUND false

#define HEARTS 0
#define DIAMONDS 1
#define CLUBS 2
#define SPADES 3

#define TWO 0
#define THREE 1
#define FOUR 2
#define FIVE 3
#define SIX 4
#define SEVEN 5
#define EIGHT 6
#define NINE 7
#define TEN 8
#define JACK 9
#define QUEEN 10
#define KING 11
#define ACE 12

#define MAX_POINTS 100

using namespace std;

class Trick;
class Game;
struct PlayedCard;

class Card {
private:
	int card;
public:
	static string ranks[13];
	static string suits[4];

	Card(); // Card()
	Card(int c); // Card(0), Card(15), ...
	Card(int suit, int rank); // Card(HEARTS, 14), Card(2, KING), ...

	/** Returns the characteristic name of the card */
	string getName() const;

	/** Returns the suit of the card (hearts = 0, diamonds = 1, clubs = 2, spades = 3) */
	int getCardSuit() const;

	/** Returns the rank of the card (jack = 11, queen = 12, king = 13, ace = 14) */
	int getCardRank() const;

	/** Returns the number of points a card is worth */
	int getPoints() const;

	/** Returns the integer value of this card */
	operator int() const;

	/** print the cards in this hand*/
	static void printCards(vector<Card> &);
	static void printCards(vector<PlayedCard> &);
	static void printCards(vector<Card> &, vector<int> &);

	static void sortCardsAscending(vector<Card> &);
	static void sortCardsDescending(vector<Card> &);

	bool operator ==(const Card &) const;
	bool operator !=(const Card &) const;
	bool operator <(const Card &) const;
	bool operator <=(const Card &) const;
	bool operator >(const Card &) const;
	bool operator >=(const Card &) const;
};

class IPlayer {
	friend class Game;
public:
	//~IPlayer();//it can't be virtual, and in should not be non-virtual? w/e

	virtual ~IPlayer() {
	}
	;

	vector<Card> hand;
	int points;
	int id;

	IPlayer(int, int);

	void init() {
	}
	;

	/** Returns the amount of points a player has */
	int getPoints() const; // getPoints()

	/** Returns whether a card is in the hand */
	bool isCardInHand(Card) const; // isCardInHand(Card(12)), isCardInHand(Card(CLUBS, KING)), ...
	vector<Card> getCards(std::vector<int>);
	bool isValidMove(const Trick &, Card) const;
	void removeCard(int position);
	void removeCard(Card card);
	void addCard(Card card);

	/** Returns the number of cards with the given suit */
	virtual int numberOfSuitInHand(int) const; // numberOfSuitInHand(HEARTS), numberOfSuitInHand(3), ...

	/** Returns the indices to the cards in the hand that should be passed */
	virtual vector<int> passCards() {
		return vector<int>();
	}
	;

	/** Virtual function that defines the behaviour of evaluation after the cards have passed */
	virtual void evaluateHand() {
	}
	;

	/** Returns the index of the card that should be played based on the cards played so far */
	virtual int decide(const Trick&) {
		return 0;
	}
	;

	/** Virtual function that defines the behaviour of evaluation at the end of a trick */
	virtual void endOfTrick(const Trick&) {
	}
	;

};

class Random : public IPlayer {
private:
	bool heartsBroken;
public:
	Random(int, int);
	/** Returns the indices to the cards in the hand that should be passed */
	vector<int> passCards();

	/** Returns the index of the card that should be played based on the cards played so far */
	int decide(const Trick&);
	void endOfTrick(const Trick&);
};
class SmartRandom : public IPlayer {
private:
	bool heartsBroken;
	vector<Card> sortedHand[4];
	int getLowestRankIndex(bool);
	int getHighestRankIndex(bool);
	int getIndex(Card);
public:
	SmartRandom(int, int);
	/** Returns the indices to the cards in the hand that should be passed */
	vector<int> passCards();
	/** Returns the index of the card that should be played based on the cards played so far */
	int decide(const Trick&);
	void endOfTrick(const Trick&);
};

class Input : public IPlayer {
	/** Returns the indices to the cards in the hand that should be passed */
public:
	Input(int, int);

	vector<int> passCards();

	/** Returns the index of the card that should be played based on the cards played so far */
	int decide(const Trick&);
	void endOfTrick(const Trick&);
};

struct PlayedCard {
	IPlayer* player;
	Card card;
};

class Trick {
public:
	int trickNumber;
	bool heartsBroken;
	Trick(int, bool);

	vector<PlayedCard> playedCards;

	/** Returns the winner of the round */
	IPlayer* getWinner() const;

	/** Returns the points played in this round */
	int getPoints() const;
};

struct PlayerData {
	IPlayer* player;
	int points;
};

class Game {
private:
	int seed;
	int roundIndex;

public:
	static vector<int> passIndices;
	vector<PlayerData> players;

	Game();
	~Game();

	/** Adds a player to the game */
	void addPlayer(IPlayer*);
	/** Clears all the players from the game */
	void clearPlayers();

	/** Plays one round (13 tricks) */
	vector<int> playRound(bool);

	/** Plays a game until the max number of points has been reached */
	vector<int> playGame(vector<int>, bool);

	/** Passes the cards before a round */
	void passCards(bool);

	unsigned long long getRandomNumber();

	/** Shuffle cards and give them to the players */
	void shuffle();

	/** Prints the hand of all the players */
	void printPlayers();
};

class Error {
private:
	int id, info;
	string message;
public:
	Error(int, int);
	Error(string);
	int getID();
	int getInfo();
	string getMessage();
	static string getNumberAsString(int);
};

#endif // HEARTS_H
