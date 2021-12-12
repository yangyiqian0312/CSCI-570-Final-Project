#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>
#include <time.h>
#include <cstdlib>
#include <ctype.h>
#include <iomanip>

//Windows Only h files. Comment these out to run on Linux/Mac
//#include <Windows.h>
//#include <Psapi.h>

using namespace std;

bool validateLength(string finalString, string baseString, int n)
{
	int validLength = pow(2, n) * baseString.length();
	return (finalString.length() == validLength);
}

bool isNumber(string s)
{
	if (isdigit(s[0]))
		return true;
	else return false;
}

const size_t alphabets = 26;

int align(string finalString1, string finalString2, int gap_penalty,
	int mismatchPenalty[alphabets][alphabets], string& alignedString1, string& alignedString2)
{
	int n = finalString1.size();
	int m = finalString2.size();

	vector<vector<int> > A(n + 1, vector<int>(m + 1));

	int i, j;

	for (i = 0; i <= m; ++i)
		A[0][i] = gap_penalty * i;
	for (i = 0; i <= n; ++i)
		A[i][0] = gap_penalty * i;

	for (i = 1; i <= n; ++i)
	{
		for (j = 1; j <= m; ++j)
		{
			char x_i = finalString1[i - 1];
			char y_j = finalString2[j - 1];
			A[i][j] = min(A[i - 1][j - 1] + mismatchPenalty[x_i - 'A'][y_j - 'A'],
				min(A[i - 1][j] + gap_penalty, A[i][j - 1] + gap_penalty));
		}
	}

	i = n;
	j = m;

	while (i >= 1 && j >= 1)
	{
		char x_i = finalString1[i - 1];
		char y_j = finalString2[j - 1];
		if (A[i][j] == A[i - 1][j - 1] + mismatchPenalty[x_i - 'A'][y_j - 'A'])
		{
			alignedString1 = x_i + alignedString1;
			alignedString2 = y_j + alignedString2;
			i--;
			j--;
		}
		else if (A[i][j] == A[i - 1][j] + gap_penalty)
		{
			alignedString1 = x_i + alignedString1;
			alignedString2 = '_' + alignedString2;
			i--;
		}
		else if (A[i][j] == A[i][j - 1] + gap_penalty)
		{
			alignedString1 = '_' + alignedString1;
			alignedString2 = y_j + alignedString2;
			j--;
		}
		else
			cout << "Something Wrong \n";
	}

	while (i >= 1 && j < 1)
	{
		alignedString1 = finalString1[i - 1] + alignedString1;
		alignedString2 = '_' + alignedString2;
		--i;
	}
	while (j >= 1 && i < 1)
	{
		alignedString1 = '_' + alignedString1;
		alignedString2 = finalString2[j - 1] + alignedString2;
		--j;
	}

	return A[n][m];
}

void memEff_sequence_alignment(vector<int>& cost, string a1, string b1, int delta, int mismatchPenalty[alphabets][alphabets]) {
	int m = a1.size();
	int n = b1.size();

	// m * 2 array
	vector<vector<int>> OPT(m + 1, vector<int>(2, 0));

	/*
		Base case:
		OPT[i][0] = i * delta for i = 0 to m
	*/
	for (int i = 0; i <= m; ++i)
		OPT[i][0] = i * delta;

	/*
		Recurrence:
	*/
	for (int j = 1; j <= n; ++j)
	{
		OPT[0][1] = j * delta;
		for (int i = 1; i <= m; ++i)
		{
			char x_i = a1[i - 1];
			char y_j = b1[j - 1];
			OPT[i][1] = min(OPT[i - 1][0] + mismatchPenalty[x_i - 'A'][y_j - 'A'],
				min(OPT[i - 1][1] + delta, OPT[i][0] + delta));
		}
		for (int i = 0; i <= m; ++i)
			OPT[i][0] = OPT[i][1];
	}

	for (int i = 0; i <= m; i++) {
		cost[i] = OPT[i][1];
	}
}

void D_and_C_Alignment(string a1, string b1, int delta, int mismatchPenalty[alphabets][alphabets],
	string& a2, string& b2, int& penalty) {
	int m = a1.size();
	int n = b1.size();
	int nMid = n / 2;
	int mMid = 0;

	if (m <= 2 || n <= 2) penalty += align(a1, b1, delta, mismatchPenalty, a2, b2);
	else {
		string bLowerHalf = b1.substr(0, nMid);
		string bUpperHalf = b1.substr(nMid);
		vector<int> tcost(a1.size() + 1), lcost(a1.size() + 1), rcost(a1.size() + 1);

		string a1Rev = a1;
		string bUpperHalfRev = bUpperHalf;

		reverse(a1Rev.begin(), a1Rev.end());
		reverse(bUpperHalfRev.begin(), bUpperHalfRev.end());

		memEff_sequence_alignment(lcost, a1, bLowerHalf, delta, mismatchPenalty);
		memEff_sequence_alignment(rcost, a1Rev, bUpperHalfRev, delta, mismatchPenalty);

		for (int i = 0; i <= a1.size(); i++) {
			tcost[i] = lcost[i] + rcost[a1.size() - i];
			if (tcost[i] < tcost[mMid]) {
				mMid = i;
			}
		}

		string aLowerHalf = a1.substr(0, mMid);
		string aUpperHalf = a1.substr(mMid);


		D_and_C_Alignment(aUpperHalf, bUpperHalf, delta, mismatchPenalty, a2, b2, penalty);
		D_and_C_Alignment(aLowerHalf, bLowerHalf, delta, mismatchPenalty, a2, b2, penalty);
	}
}

int main(int argc, char* argv[])
{
	string inputFile = "";

	if (argc > 1)
		inputFile = argv[1];
	else
		cout << "No input file provided \n";

	string baseString1, baseString2; // The base string in the input file
	string finalString1, finalString2; // The final strings generated from the base strings

	int j = 0, k = 0;
	int counter = 0;

	fstream myFile;
	ofstream outputFile("output.txt");

	myFile.open(inputFile, ios::in);

	if (myFile.is_open())
	{
		string line;
		string command;
		while (getline(myFile, line))
		{
			istringstream iss(line);
			iss >> command;

			if (counter == 0)
			{
				baseString1 += command;
				finalString1 += command;
				counter++;
			}

			else if (counter == 1)
			{
				if (isNumber(command))
				{
					int indexToAppend = stoi(command);
					indexToAppend++;
					finalString1.insert(indexToAppend, finalString1);
					j++;
				}
				else counter++;
			}

			if (counter == 2)
			{
				baseString2 += command;
				finalString2 += command;
				counter++;
			}

			else if (counter == 3)
			{
				if (isNumber(command))
				{
					int indexToAppend = stoi(command);
					indexToAppend++;
					finalString2.insert(indexToAppend, finalString2);
					k++;

				}
			}

		}
		myFile.close();
	}

	int m, n;
	n = validateLength(finalString1, baseString1, j); // Validate 1st generated string is of length (2^j)*str1.length 
	m = validateLength(finalString2, baseString2, k); // Validate 2nd generated string is of length (2^k)*str2.length

	if (n == -1 || m == -1)
		cout << "Error in string generation \n";

	/*
	The mismatch penalty (alpha) table
	*/
	int mismatchPenalty[alphabets][alphabets];

	for (int i = 0; i < alphabets; ++i)
	{
		for (int j = 0; j < alphabets; ++j)
		{
			if (i == j)
				mismatchPenalty[i][j] = 0;
			else
				mismatchPenalty[i][j] = 1;
		}
	}

	mismatchPenalty['A' - 'A']['C' - 'A'] = 110;
	mismatchPenalty['A' - 'A']['G' - 'A'] = 48;
	mismatchPenalty['A' - 'A']['T' - 'A'] = 94;

	mismatchPenalty['C' - 'A']['A' - 'A'] = 110;
	mismatchPenalty['C' - 'A']['G' - 'A'] = 118;
	mismatchPenalty['C' - 'A']['T' - 'A'] = 48;

	mismatchPenalty['G' - 'A']['A' - 'A'] = 48;
	mismatchPenalty['G' - 'A']['C' - 'A'] = 118;
	mismatchPenalty['G' - 'A']['T' - 'A'] = 110;

	mismatchPenalty['T' - 'A']['A' - 'A'] = 94;
	mismatchPenalty['T' - 'A']['C' - 'A'] = 48;
	mismatchPenalty['T' - 'A']['G' - 'A'] = 110;

	/*
		The gap penalty (delta)
	*/
	int delta = 30;

	/*
		The resulting aligned strings
	*/
	string a2, b2 = "";
	int penalty = 0;

	struct rusage usage;
	long mem_usage;
	clock_t startTime = clock();

	D_and_C_Alignment(finalString1, finalString2, delta, mismatchPenalty, a2, b2, penalty);
	getrusage(RUSAGE_SELF, &usage);
	mem_usage = usage.ru_maxrss;

	clock_t endTime = clock();
	double msTimer = (double)(endTime - startTime) / CLOCKS_PER_SEC;
	float floatMem = (float)mem_usage;
	float floatPen = (float)penalty;

	if (outputFile.is_open()) {
		if (a2.size() <= 50 && b2.size() <= 50) {
			outputFile << a2 << endl;
			outputFile << b2 << endl;
		}
		else {
			string afirst50, alast50 = "";
			string bfirst50, blast50 = "";

			for (int i = 0; i < 50; i++) {
				afirst50 += a2.at(i);
				alast50 += a2.at((a2.size() - 1) - i);
			}

			for (int i = 0; i < 50; i++) {
				bfirst50 += b2.at(i);
				blast50 += b2.at((b2.size() - 1) - i);
			}

			reverse(alast50.begin(), alast50.end());
			reverse(blast50.begin(), blast50.end());

			outputFile << afirst50 << " " << alast50 << endl;
			outputFile << bfirst50 << " " << blast50 << endl;
		}
		outputFile << floatPen << ".0" << endl;
		outputFile << msTimer << endl;	
		if (floor(floatMem) == floatMem) outputFile << floatMem << ".0" << endl;
		else outputFile << floatMem << endl;
		
	}

	return 0;
}