// Tic-Tac-Toe.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream>
#include <string>
#include "board.h"
using namespace std;

int main()
{	
		
	board Table;
	
	string names[2];
	cout << "First player name: ";
	cin >> names[0];
	cout << "Second player name: ";
	cin >> names[1];
	int curPlayer = 0;

	while (!Table.bIsFinished) {
		Table.print();
		cout << "Player " << names[curPlayer] << " make a move: ";
		int nMove;
		cin >> nMove;
		nMove--;
		if (Table.CheckMove(nMove)) {
			Table.Set(nMove, curPlayer);
			curPlayer ^= 1;
		}
		else {
			cout << "Your move is incorrect!!\n";
			system("pause");
		}
	}
	Table.print();
	cout << "Player " << names[curPlayer ^ 1] << " wins!!!\n";
	system("pause");
	return 0;
}

