#include <limits.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "time.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef struct score {
	int whites;
	int colored;
} score;

vector<string> read(string file, int size) {
	vector<string> v;
	ifstream myfile(file.c_str());
	string line;
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			int i;
			for (i = 0; i < (int) line.size(); ++i) {
				if (line[i] == ' ')
					break;
			}
			if (i == size)
				v.push_back(line.substr(0, i));
		}
		myfile.close();
	}
	return v;
}

score get_score(string code, string guess) {
	score s;
	s.colored = 0;
	s.whites = 0;
	vector<int> flags(code.size());
	vector<int> used(code.size());
	for (unsigned int i = 0; i < guess.size(); ++i) {
		if (guess[i] == code[i]) {
			s.colored++;
			flags[i] = 1;
			used[i] = 1;
		} else {
			flags[i] = 0;
			used[i] = 0;
		}
	}
	for (unsigned int i = 0; i < guess.size(); i++) {
		if (used[i] == 0) {
			for (unsigned int j = 0; j < guess.size(); j++) {
				if (guess[i] == code[j] && flags[j] == 0) {
					s.whites++;
					flags[j] = 1;
					break;
				}
			}
		}
	}
	return s;
}

int exact(string s1, string s2) {
	if (s1.size() > s2.size())
		return 0;
	for (int i = 0; i < (int) s1.size(); ++i) {
		if (s1[i] != s2[i])
			return 0;
	}
	if (s1.size() == s2.size())
		return 1;
	return 2;
}

int getch(void) {
	struct termios oldattr, newattr;
	int ch;
	tcgetattr( STDIN_FILENO, &oldattr);
	newattr = oldattr;
	newattr.c_lflag &= ~( ICANON | ECHO);
	tcsetattr( STDIN_FILENO, TCSANOW, &newattr);
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldattr);
	cout << toupper(ch);
	return ch;
}
void clear() {
	printf("\033[2J\033[1;1H"); //console cleaner
}

string choose(vector<string> S) {
	string str = "";
	while (1) {
		int matches = 0, printed = 0;
		int r;
		clear();
		cout << "Please choose a word\n";
		cout << str << endl;
		cout << "Matches:" << endl;
		for (int i = 0; i < (int) S.size(); i++) {
			if ((r = exact(str, S[i])) != 0)
				if (printed < 15) {
					cout << S[i] << "\n";
					printed++;
				} else if (printed == 15) {
					cout << "...\n";
					printed++;
				}
			if (r == 1)
				matches++;
		}
		int ch = toupper(getch());
		if (ch >= 'A' && ch <= 'Z')
			str += (char) ch;
		else if (ch == 127 && str.size() > 0)
			str.pop_back();
		else if (ch == 10) {
			if (matches > 0)
				break;
			else {
				clear();
				cout << str << " does not match any word in the storage!\nPress any key to continue\n";
				getch();
			}
		}
	}
	return str;
}

int main() {
	clear();
	time_t begin, end;
	score s;
	int n = 1;
	string code;
	string guess;
	int size;
	while(1)
	{
		cout << "What size do you want to play with?" << endl;
		cin >> size;
		getch();
		clear();
		if(size<2 || size >15)
			cout << "Only words with 2 to 15 letters available"<< endl;
		else break;
	}
	vector<string> S = read("words.txt", size);
	code=choose(S);
	time(&begin);
	clear();
	guess = S[0];
	s = get_score(code, guess);
	while (s.colored != size) {
		cout << "New Guess: " << guess << "\nScore: " << s.colored << " colored," << s.whites << " white" << endl;
		vector<string> temp;
		for (int i = 0; i < (int) S.size(); ++i) {
			score gs;
			gs = get_score(guess, S[i]);
			if (gs.colored == s.colored && gs.whites == s.whites)
				temp.push_back(S[i]);
		}
		S = temp;
		guess = S[0];
		s = get_score(code, guess);
		++n;

	}
	cout << "New Guess: " << guess << "\nScore: " << s.colored << " colored," << s.whites << " white" << endl;
	time(&end);
	cout << "Solved code " << guess << " in " << n << " steps (" << difftime(end, begin) << " seconds)." << endl;
}
