#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <sstream>
using namespace std;

int validateLength(string finalString, string baseString, int n)
{
	int validLength = pow(2,n) * baseString.length(); 
	if (finalString.length() == validLength)
		return validLength;
	else return -1;
}

bool isNumber(string s)
{
	if(isdigit(s[0]))
		return true;
	else return false;
}

const size_t letters = 26;

int alignSequence(string finalString1, string finalString2, int gap_penalty, 
		int mismatchPenalty[letters][letters], string &alignedString1, string &alignedString2)
{
    int n = finalString1.size();
    int m = finalString2.size();

    vector<vector<int> > OPT(n+1, vector<int>(m+1));

    int i, j;

    for (i = 0; i <= m; i++)
        OPT[0][i] = gap_penalty*i;
    for (i = 0; i <= n; i++)
        OPT[i][0] = gap_penalty*i;

    for (i = 1; i <= n; i++)
    {
        for (j = 1; j <= m; j++)
        {
            char x_i = finalString1[i-1];
            char y_j = finalString2[j-1];
            OPT[i][j] = min({OPT[i-1][j-1] + mismatchPenalty[x_i - 'A'][y_j - 'A'], OPT[i-1][j] + gap_penalty,
                          OPT[i][j-1] + gap_penalty});
        }
    }

    alignedString1 = "";
    alignedString2 = "";
    i = n;
    j = m;

	while ( i >= 1 && j >= 1)
    {
        char x_i = finalString1[i-1];
        char y_j = finalString2[j-1];

        if (OPT[i][j] == OPT[i-1][j-1] + mismatchPenalty[x_i - 'A'][y_j - 'A'])
        {
            alignedString1 = x_i + alignedString1;
            alignedString2 = y_j + alignedString2;
            i--;
            j--;
        }
        else if (OPT[i][j] == OPT[i][j-1] + gap_penalty)
        {
            alignedString1 = '_' + alignedString1;
            alignedString2 = y_j + alignedString2;
            j--;
        }
        else if (OPT[i][j] == OPT[i-1][j] + gap_penalty)
        {
            alignedString1 = x_i + alignedString1;
            alignedString2 = '_' + alignedString2;
			i--;        
        }
        else
        	cout << "Something Wrong \n";
    }

    while (i >= 1 && j < 1)
    {
        alignedString1 = finalString1[i-1] + alignedString1;
        alignedString2 = '_' + alignedString2;
        --i;
    }
    while (j >= 1 && i < 1)
    {
        alignedString1 = '_' + alignedString1;
        alignedString2 = finalString2[j-1] + alignedString2;
        --j;
    }
	
    return OPT[n][m];
}


int main(int argc, char *argv[])
{
	string inputFile = "";

	if (argc > 1)
		inputFile = argv[1];
	else
		cout << "No input file provided \n";

	ofstream outputFile("output.txt");

	string baseString1 = "", baseString2 = ""; // The base string in the input file
	string finalString1 = "", finalString2 = ""; // The final strings generated from the base strings
	
	int j = 0, k = 0; 
	int counter = 0;

	fstream myFile;
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
					finalString1.insert(indexToAppend,finalString1);
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
					finalString2.insert(indexToAppend,finalString2);
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

	int gap_penalty = 30;
    int mismatchPenalty[letters][letters];
    
    for (int i = 0; i < letters; i++)
    {
        for (int j = 0; j < letters; j++)
        {
            if (i == j) 
                mismatchPenalty[i][j] = 0;
            else 
                mismatchPenalty[i][j] = 1;
        }
    }

    mismatchPenalty['A'-'A']['C'-'A'] = 110;
    mismatchPenalty['A'-'A']['G'-'A'] = 48;
    mismatchPenalty['A'-'A']['T'-'A'] = 94;

    mismatchPenalty['C'-'A']['A'-'A'] = 110;
    mismatchPenalty['C'-'A']['G'-'A'] = 118;
    mismatchPenalty['C'-'A']['T'-'A'] = 48;

    mismatchPenalty['G'-'A']['A'-'A'] = 48;
    mismatchPenalty['G'-'A']['C'-'A'] = 118;
    mismatchPenalty['G'-'A']['T'-'A'] = 110;
    
    mismatchPenalty['T'-'A']['A'-'A'] = 94;
    mismatchPenalty['T'-'A']['C'-'A'] = 48;
    mismatchPenalty['T'-'A']['G'-'A'] = 110;

    struct rusage usage;
	long mem_usage;
	clock_t startTime = clock();
	
    string alignedString1, alignedString2;
    int penalty = alignSequence(finalString1, finalString2, gap_penalty, mismatchPenalty, alignedString1, alignedString2);

	clock_t endTime = clock();

	getrusage(RUSAGE_SELF, &usage);
	mem_usage = usage.ru_maxrss;
	
	double msTimer = (double)(endTime - startTime) / CLOCKS_PER_SEC;
	float floatMem = (float)mem_usage;
	float floatPen = (float)penalty;

	if (outputFile.is_open()) {
		if (alignedString1.size() <= 50 && alignedString2.size() <= 50) {
			outputFile << alignedString1 << endl;
			outputFile << alignedString2 << endl;
		}
		else {
			string afirst50, alast50 = "";
			string bfirst50, blast50 = "";

			for (int i = 0; i < 50; i++) {
				afirst50 += alignedString1.at(i);
				alast50 += alignedString1.at((alignedString1.size() - 1) - i);
			}

			for (int i = 0; i < 50; i++) {
				bfirst50 += alignedString2.at(i);
				blast50 += alignedString2.at((alignedString2.size() - 1) - i);
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




