#include <iostream>
using namespace std;
#pragma once



class board
{
private:
	//SOME STUF
	char cField[5][5];
	char cPlayers[2];
	pair<int, int> pTo[9];
	bool bCheckTriple(int x, int y, int z) {
		pair<int, int> pCur[3] = { pTo[x], pTo[y], pTo[z] };
		for (int i = 1; i < 3; i++) {
			if (cField[pCur[i].first][pCur[i].second] != cField[pCur[i - 1].first][pCur[i - 1].second] || (cField[pCur[i].first][pCur[i].second] == '.'))
				return 0;
		}
		return 1;
	}
	bool bCheckWinner() {
		bool ok = 0;
		for (int i = 0; i < 9; i+=3) {
			ok |= bCheckTriple(i, i + 1, i + 2);
		}
		for (int i = 0; i < 3; i++) {
			ok |= bCheckTriple(i, i + 3, i + 6);
		}
		ok |= bCheckTriple(0, 4, 8);
		ok |= bCheckTriple(2, 4, 6);
		return ok;
	}

public:
	//SOME STUF
	bool bIsFinished = 0;
	board() {
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				cField[i][j] = '.';
			}
		}
		for (int i = 0; i < 5; i++) {
			cField[1][i] = '-';
			cField[3][i] = '-';
		}
		for (int i = 0; i < 5; i++) {
			cField[i][1] = '|';
			cField[i][3] = '|';		
		}
		pTo[0] = { 0, 0 };
		pTo[1] = { 0, 2 };
		pTo[2] = { 0, 4 };
		pTo[3] = { 2, 0 };
		pTo[4] = { 2, 2 };
		pTo[5] = { 2, 4 };
		pTo[6] = { 4, 0 };
		pTo[7] = { 4, 2 };
		pTo[8] = { 4, 4 };
		cPlayers[0] = 'X';
		cPlayers[1] = 'O';
	}
	bool CheckMove(int nPlayerMove) {
		pair<int, int> pCur = pTo[nPlayerMove];
		if (cField[pCur.first][pCur.second] != '.') return 0;
		return 1;
	}
	void Set(int nPlayerMove, int curPlayer) {
		pair<int, int> pCur = pTo[nPlayerMove];
		cField[pCur.first][pCur.second] = cPlayers[curPlayer];
		if (bCheckWinner()) {
			bIsFinished = 1;
		}
	}	
	void print() {
		system("cls");
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				cout << cField[i][j];
			}
			cout << "\n";
		}
	}
};