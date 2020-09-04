// bnf.cpp
// Gus McCoy
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

const string EndOfValidSyntax = "HALT";
const string PRINTSTATEMENT = "PRINT";
const char	 ASSIGNMENTOPERATOR = '=';
const char ADDITIONOPERATOR = '+';
const char SUBTRACTIONOPERATOR = '-';
const char VARIABLE_Z = 'z';
const char VARIABLE_Y = 'y';
const char VARIABLE_X = 'x';

struct variable
{
	char name;
	int value;
	bool declared;
};

vector<variable> DECLAREDVARIBLES;

bool isValidSyntax(string);
bool isAStatement(string);
bool isAnAssignment(string, bool);
bool isAnExpression(string, bool);
bool isAnTerm(string, bool);
bool isAVariable(char);
bool isADefinedVariable(char);
bool isAnOperator(char);
bool isANumber(string);
bool isAPrintStatement(string);
bool isAnAttemptToPrintUndeclaredVariable(string);
bool isANewVariable(char);
string EvaluateStatement(string);
string DetermineErrorMessage(string);

void main() {
	ifstream fin("bnf.in");

	if (fin.is_open()) {
		
		ofstream fout("bnf.out");

		string line;
		getline(fin, line, '\n');

		while (line != EndOfValidSyntax)
		{
			fout << left << line;
			string message = "";
			if (isValidSyntax(line))
			{
				message = EvaluateStatement(line);
			}
			else
			{
				message = DetermineErrorMessage(line);
			}
			if (message != "")
			{
				fout << right << setw(32 - line.length() + message.length() - 1) << message;
			}
			fout << endl;
			getline(fin, line, '\n');
		}

		fout.close();
		fin.close();
	}
	else {
		cout << "Input file not found." << endl;
	}
}

bool isValidSyntax(string LineToTest)
{
	bool result = false;
	if (isAStatement(LineToTest))
	{
		result = true;
	}

	return result;
}

bool isAStatement(string LineToTest)
{
	bool result = false;

	if (isAPrintStatement(LineToTest))
	{
		result = true;
	}
	else if (isAnAssignment(LineToTest, 0))
	{
		result = true;
	}


	return result;
}

bool isAnAssignment(string LineToTest, bool InvalidTest)
{
	bool result = false;
	stringstream sin(LineToTest);
	string Expression = " ";
	char Variable = ' ', AssignmentOperator = ' ';

	if (sin)
	{
		sin >> Variable;
	}
	if (sin)
	{
		sin >> AssignmentOperator;
	}
	if (sin)
	{
		getline(sin, Expression);
	}
	if (isAVariable(Variable) && AssignmentOperator == ASSIGNMENTOPERATOR && isAnExpression(Expression, InvalidTest))
	{
		if (isANewVariable(Variable) && !InvalidTest)
		{
			DECLAREDVARIBLES.push_back({ Variable, 0, false });
		}
		result = true;
	}

	return result;
}

bool isAnExpression(string LineToTest, bool InvalidTest)
{
	bool result = false;
	stringstream sin(LineToTest);

	char Operator = ' ';
	string Expression = "", Term = "";

	if (sin)
	{
		sin >> Term;
	}
	if (sin)
	{
		sin >> Operator;
	}
	if (sin)
	{
		getline(sin, Expression);
	}

	if (isAnTerm(Term, InvalidTest) && isAnOperator(Operator) && isAnExpression(Expression, InvalidTest))
	{
		result = true;
	}
	else if(isAnTerm(Term, InvalidTest) && Operator == ' ' && Expression == "")
	{
		result = true;
	}
	return result;
}

bool isAnTerm(string CharToTest, bool InvalidTest)
{
	bool result = false;
	stringstream sin(CharToTest);
	char PossibleVariable = ' ';

	sin >> PossibleVariable;

	if (!InvalidTest)
	{
		if (isANumber(CharToTest) || isADefinedVariable(PossibleVariable))
		{
			result = true;
		}
	}
	else
	{
		if(isANumber(CharToTest) || isANewVariable(PossibleVariable) || isADefinedVariable(PossibleVariable))
		{
			result = true;
		}
	}
	return result;
}

bool isAVariable(char CharToTest)
{
	bool result = false;

	if (CharToTest == VARIABLE_X || CharToTest == VARIABLE_Y || CharToTest == VARIABLE_Z)
	{
		result = true;
	}

	return result;
}

bool isADefinedVariable(char CharToTest)
{
	bool result = false;

	for (int i = 0; i < DECLAREDVARIBLES.size(); i++)
	{
		if (DECLAREDVARIBLES[i].name == CharToTest) {
			result = true;
		}
	}
	return result;
}

bool isAnOperator(char CharToTest)
{
	bool result = false;

	if (CharToTest == '+' || CharToTest == '-')
	{
		result = true;
	}

	return result;
}

bool isANumber(string NumberToTest)
{
	bool result = false;

	try
	{
		if (stoi(NumberToTest))
		{
			result = true;
		}
	}
	catch (invalid_argument error)
	{
		// No nothing
	}
	

	return result;
}

bool isAPrintStatement(string LineToTest)
{
	bool result = false;

	if (LineToTest.size() == 8)
	{
		if (LineToTest.substr(0, 5) == PRINTSTATEMENT && LineToTest[5] == '('
			&& isAVariable(LineToTest[6]) && LineToTest[7] == ')' && !isANewVariable(LineToTest[6]) && isADefinedVariable(LineToTest[6]))
		{
			result = true;
		}
	}
	return result;
}

bool isAnAttemptToPrintUndeclaredVariable(string LineToTest)
{
	bool result = false;

	if (LineToTest.size() == 8)
	{
		if (LineToTest.substr(0, 5) == PRINTSTATEMENT && LineToTest[5] == '('
			&& isAVariable(LineToTest[6]) && LineToTest[7] == ')' && isANewVariable(LineToTest[6]) && !isADefinedVariable(LineToTest[6]))
		{
			result = true;
		}
	}

	return result;
}

bool isANewVariable(char LetterToTest)
{
	bool result = true;

	for (int i = 0; i < DECLAREDVARIBLES.size(); i++)
	{
		if (DECLAREDVARIBLES[i].name == LetterToTest)
		{
			result = false;
			i = DECLAREDVARIBLES.size();
		}
	}

	return result;
}

string EvaluateStatement(string LineToEvaluate)
{
	string result = "";
	if (isAPrintStatement(LineToEvaluate))
	{
		string NumberToOutput = "";
		for (int i = 0; i < DECLAREDVARIBLES.size(); i++)
		{
			if (DECLAREDVARIBLES[i].name == LineToEvaluate[6])
			{
				NumberToOutput = to_string(DECLAREDVARIBLES[i].value);
			}
		}
		result = NumberToOutput;
	}
	else if (isAnAssignment(LineToEvaluate, 0))
	{
		stringstream sin(LineToEvaluate);
		char VariableToBeAssignedTo = ' ', Buffer = ' ';

		if (sin)
		{
			int RunningTotal = 0, VariableIndex = 0, IndexOfAssigningVariable = 0;
			string Term = "";
			char Operator = ' ';

			sin >> VariableToBeAssignedTo >> Buffer >> Term;

			for (int i = 0; i < DECLAREDVARIBLES.size(); i++)
			{
				if (DECLAREDVARIBLES[i].name == VariableToBeAssignedTo)
				{
					IndexOfAssigningVariable = i;
				}
			}

			if (isAVariable(Term[0]))
			{
				for (int i = 0; i < DECLAREDVARIBLES.size(); i++)
				{
					if (DECLAREDVARIBLES[i].name == Term[0])
					{
						VariableIndex = i;
					}
				}
				RunningTotal += DECLAREDVARIBLES[VariableIndex].value;
			}
			else if (isANumber(Term))
			{
				RunningTotal += stoi(Term);
			}

			sin.ignore();

			while (sin)
			{
				sin >> Operator >> Term;

				if (isAVariable(Term[0]))
				{
					int VariableIndex = 0;
					for (int i = 0; i < DECLAREDVARIBLES.size(); i++)
					{
						if (DECLAREDVARIBLES[i].name == Term[0])
						{
							VariableIndex = i;
						}
					}

					if (Operator == ADDITIONOPERATOR)
						RunningTotal += DECLAREDVARIBLES[VariableIndex].value;
					else
						RunningTotal -= DECLAREDVARIBLES[VariableIndex].value;
				}
				else if (isANumber(Term))
				{
					if (Operator == ADDITIONOPERATOR)
						RunningTotal += stoi(Term);
					else
						RunningTotal -= stoi(Term);
				}
				sin.ignore();
			}
			DECLAREDVARIBLES[IndexOfAssigningVariable].value = RunningTotal;
		}
	}
	return result;
}

string DetermineErrorMessage(string LineToTest)
{
	string result = "Bad syntax";

	if (isAnAssignment(LineToTest, 1))
	{
		stringstream sin(LineToTest);
		char TheUndefinedVariable = ' '; char testVariable;

		while (sin)
		{
			sin >> testVariable;
			if (isAVariable(testVariable) && !isADefinedVariable(testVariable))
			{
				TheUndefinedVariable = testVariable;
			}
		}

		string undefvar(1, TheUndefinedVariable);
		if (undefvar != " ")
		{
			result = undefvar + " is undefined";
		}
	}
	else if (isAnAttemptToPrintUndeclaredVariable(LineToTest))
	{
		string undefvar(1, LineToTest[6]);
		result = undefvar + " is undefined";
	}

	return result;
}
