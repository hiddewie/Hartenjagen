#include <iostream>
#include <fstream>
#include <stdexcept>
#include <stdio.h> //iemand die streams kan mag dit verwijderen
#include <typeinfo>

// Game
#include "hearts.h"

// Players
#include "KoenHidde.h"

// losses[i] tracks how many times player i has lost a game.
int losses[4], totalLosses[4];
int simulations = 1000, outputInterval = 200;

#include <time.h>

ofstream file;

vector<int> playerClassIDs;

void playGame(int gameID, bool viewRound) {
	try {
		Game* game = new Game();
		vector<int> losers = game->playGame(playerClassIDs, viewRound);
		delete game;

		for (unsigned int i = 0; i < losers.size(); i++) {
			losses[losers.at(i)]++;
			totalLosses[losers.at(i)]++;
		}

		if (gameID % outputInterval == 0) {
			file << "Games #" << gameID - outputInterval << " - #" << gameID << ": " << losses[0] << " " << losses[1] << " " << losses[2] << " "
					<< losses[3] << "\n";
			for (int i = 0; i < 4; i++) {
				losses[i] = 0;
			}
		}
	} catch (Error& error) {
		flush(cout);
		cerr << " --- ERROR --- " << endl;
		cerr << error.getMessage() << endl;
		exit(EXIT_FAILURE);
	} catch (exception& e) {
		cerr << "Error: " << e.what() << "!" << endl;
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]) {
	cout << "Enter a player ID to add a player to the games: " << endl;
	cout << " - Random: \t\t0" << endl;
	cout << " - SmartRandom: \t1" << endl;
	cout << " - KoenHidde: \t\t2" << endl;
	while (playerClassIDs.size() < 4) {
		int id;
		cin >> id;
		if (id >= 0 && id <= 3) {
			playerClassIDs.push_back(id);
		}
		else {
			cout << "Invalid ID (" + Error::getNumberAsString(id) + "), please try again.";
		}
	}

	cout << "Enter the number of simulations to run (enter 1 to view detailed game): >>> ";
	cin >> simulations;

	file.open("output.txt");
	if (simulations != 1) {
		cout << "Enter the interval to output the losses of each player: >>> ";
		cin >> outputInterval;
	}
	file << "Configuration: " << endl;
	cout << "Player configuration: ";
	for (int id = 0; id < 4; id++) {
		switch (playerClassIDs.at(id)) {
			case 0:
				file << "Random ";
				cout << "Random ";
				break;
			case 1:
				file << "SmartRandom ";
				cout << "SmartRandom ";
				break;
			case 2:
				file << "KoenHidde ";
				cout << "KoenHidde ";
				break;
			default:
				break;
		}
	}
	file << "\n";
	cout << endl;
	file << "Number of games: " << simulations << "\n";
	cout << "Number of games: " << simulations << "\n";
	if (simulations != 1) {
		file << "Output interval: each " << outputInterval << " games\n";
		cout << "Output interval: each " << outputInterval << " games\n";
	}

	cout << " --- Starting the games --- " << endl;

	clock_t t1=clock();

	for (int gameID = 1; gameID <= simulations; gameID++) {
		playGame(gameID, simulations == 1);
	}

	cout << " --- All games have ended --- " << endl;

	clock_t t2=clock();
	cout << "Program execution time: "
			<< (t2 - t1) / (double) CLOCKS_PER_SEC << endl;
	file << "Total losses: " << totalLosses[0] << " " << totalLosses[1] << " " << totalLosses[2] << " " << totalLosses[3] << "\n";
	cout << "Total losses: " << totalLosses[0] << " " << totalLosses[1] << " " << totalLosses[2] << " " << totalLosses[3] << endl;

	file.close();
	return 0;
}
