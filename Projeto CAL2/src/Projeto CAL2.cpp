#include <iostream>
#include "time.h"
#include <vector>
#include <sstream>
#include <fstream>
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
		while (getline(myfile, line, ' ')) {
			if ((int) line.size() == size)
				v.push_back(line);
			getline(myfile, line);
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

int main() {
	time_t begin, end;
	time(&begin);
	score s;
	int n = 1;
	string code = "GRUDGER";
	string guess;
	int size = code.size();
	vector<string> S = read("words.txt", size);
	guess = S[0];
	s =get_score(code, guess);
	while (s.colored != size) {
		for (int i = 0; i < (int) S.size(); ++i) {
			score gs;
			gs = get_score(guess, S[i]);
			if (gs.colored != s.colored || gs.whites != s.whites) {
				S.erase(S.begin() + i);
				--i;
			}
		}
		guess = S[0];
		s = get_score(code, guess);
		++n;
	}

	time(&end);
	cout << "Solved code " << guess << " in " << n << " steps ("
			<< difftime(end, begin) << " seconds)." << endl;
}
