#include "Map.h"
#include <iostream>
#include <string>
#include <stack>
#include <cctype>
#include <cassert>
using namespace std;

string removeSpaces(string s);
bool isOperator(char c);
bool operandOrParen(char c);

/*
	For a given infix string, the function must return 1 if it's not syntactically valid. If the string is syntactically valid but the operands are not found in
	the given map, the function must return 2. If the infix string to be evaluated results in division by zero, the function must return 3.
*/
int evaluate(string infix, const Map& values, string& postfix, int& result)
{
	infix = removeSpaces(infix);
	if (infix == "")
		return 1;

	stack <char> operatorStack;
	postfix = "";
	int numOpen = 0;
	for (int i = 0; i < infix.size(); i++) {
		switch (infix[i]) {
		case '(':
			if (i - 1 >= 0 &&
				!isOperator(infix[i - 1]) && infix[i - 1] != '(') // If there's a character before, it must either be an operator (+, -, *, /) or the left parenthesis
				return 1;

			if (i + 1 < infix.size() &&
				!isalpha(infix[i + 1]) && infix[i + 1] != '(') // If there's a character after, it must either be an operand or the left parenthesis
				return 1;

			numOpen++; // An open parenthesis has been found
			operatorStack.push(infix[i]); // Open parentheses are just added onto our postfix string
			break;

		case ')':
			if (i - 1 >= 0 &&
				!isalpha(infix[i - 1]) && infix[i - 1] != ')') // If there's a character before, it must either be an operand or the right parenthesis
				return 1;

			if (i + 1 < infix.size() &&
				!isOperator(infix[i + 1]) && infix[i + 1] != ')') // If there's a character after, it must either be an operator (+, -, *, /) or the right parenthesis
				return 1;

			if (numOpen <= 0) // There must be a closing parenthesis
				return 1;
			numOpen--; // A closing parenthesis has been found

			while (!operatorStack.empty() && operatorStack.top() != '(') { // We want to pop off every operator between the two parentheses
				postfix += operatorStack.top();
				operatorStack.pop();
			}
			operatorStack.pop(); // We can't forget to pop the last open parentheses off the stack
			
			break;

		case '+':
		case '-':
		case '*':
		case '/':
			if (i - 1 < 0||
				!operandOrParen(infix[i - 1])) // There must be a character before and it must either be an operand or a parenthesis
				return 1;

			if (i + 1 >= infix.size() ||
				!operandOrParen(infix[i + 1])) // There must be a character after and it must either be an operand or a parenthesis
				return 1;

			while (!operatorStack.empty() && operatorStack.top() != '('
				&& ((operatorStack.top() != '+' && operatorStack.top() != '-') || (infix[i] != '*' && infix[i] != '/'))) {
				postfix += operatorStack.top(); // Multiplication and division have precedence over addition and subtraction, so if * is followed by + we have to pop * first
				operatorStack.pop();
			}
			operatorStack.push(infix[i]); // Now we we add our operator
			break;

		default:
			if (!isalpha(infix[i]) || !islower(infix[i])) // Whatever falls in the default case must be a lower case operand
				return 1;

			if (i - 1 >= 0 &&
				!isOperator(infix[i - 1]) && infix[i - 1] != '(' && infix[i - 1] != ')') // If there's a character before, it must either be an operator or a parenthesis
				return 1;

			if (i + 1 < infix.size() &&
				!isOperator(infix[i + 1]) && infix[i + 1] != '(' && infix[i + 1] != ')') // If there's a character after, it must either be an operator or a parenthesis
				return 1;

			postfix += infix[i]; // Operands are just added onto our postfix string
			break;
		}
	}

	if (numOpen != 0) // There must be a closing parenthesis
		return 1;

	while (!operatorStack.empty()) {
		postfix += operatorStack.top(); // We add whatever's left onto our postfix string
		operatorStack.pop();
	}

	stack <ValueType> operandStack;
	for (int i = 0; i < postfix.size(); i++) {
		if (values.contains(postfix[i])) {
			ValueType v;
			values.get(postfix[i], v);
			operandStack.push(v); // Operands are just pushed onto the stack
		}
		else {
			if (isalpha(postfix[i])) // If a character isn't found in our list of values, we return 2
				return 2;

			ValueType operand2 = operandStack.top(); // If an operator is found, we pop the last two operands and perform the resulting operation
			operandStack.pop();
			ValueType operand1 = operandStack.top();
			operandStack.pop();
			if (postfix[i] == '+')
				operandStack.push(operand1 + operand2);
			else if (postfix[i] == '-')
				operandStack.push(operand1 - operand2);
			else if (postfix[i] == '*')
				operandStack.push(operand1 * operand2);
			else {
				if (operand2 == 0) // If we are to divide by zero, the function returns 3 instead
					return 3;
				operandStack.push(operand1 / operand2);
			}
		}
	}

	result = operandStack.top();
	operandStack.pop();
	return 0;
}

bool isOperator(char c) // Returns whether or not a character is an operator
{
	return c == '+' || c == '-' || c == '*' || c == '/';
}

bool operandOrParen(char c) // Returns whether or not a character is either a letter or parenthesis
{
	return isalpha(c) || c == '(' || c == ')';
}

string removeSpaces(string s) {
	if (s == "")
		return "";

	int endIndex = s.size() - 1;
	while (endIndex >= 0 && s[endIndex] == ' ') {
		endIndex--;
	}

	s = s.substr(0, endIndex + 1);  // We are now guaranteed to not find any white spaces at the end of our string.

	for (int i = s.size() - 1; i >= 0; i--) {
		if (s[i] == ' ')
			s = s.substr(0, i) + s.substr(i + 1, s.size() - (i + 1));
	}

	return s;
}

int main()
{
	char vars[] = { 'a', 'b', 'c', 'd', 'e', 'f'};
	int  vals[] = {  1 ,  2 ,  3 ,  4 ,  5 ,  6 };
	Map m;
	for (int k = 0; k < 6; k++)
		m.insert(vars[k], vals[k]);

	string pf = "";
	int answer = 999;

	// Test Cases
	assert(evaluate("", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("E", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("2", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("(", m, pf, answer) == 1 && answer == 999);
	assert(evaluate(")", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("(a+b))", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("(a+b)(c+d)", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("a(", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("()", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("+)", m, pf, answer) == 1 && answer == 999);
	assert(evaluate(")a", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("aa", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("(a+(i-o)", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("-a", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("a*z", m, pf, answer) == 2 &&
		pf == "az*"  &&  answer == 999);
	assert(evaluate("a/(b-b)", m, pf, answer) == 3 &&
		pf == "abb-/"  &&  answer == 999);
	assert(evaluate("((a))", m, pf, answer) == 0 &&
		pf == "a"  &&  answer == 1);
	assert(evaluate("(((a+b)*(c+d)-a)/b)", m, pf, answer) == 0 &&
		pf == "ab+cd+*a-b/"  &&  answer == 10);
	assert(evaluate("a+b*c+d-a/b", m, pf, answer) == 0 &&
		pf == "abc*+d+ab/-"  &&  answer == 11);
	assert(evaluate("a+b*(c+d)*e-f*(b+d)/e", m, pf, answer) == 0 &&
		pf == "abcd+*e*+fbd+*e/-" && answer == 64);
	assert(evaluate("a+b*c+d*e-f*b+d/e", m, pf, answer) == 0 &&
		pf == "abc*+de*+fb*-de/+" && answer == 15);
	assert(evaluate("(a+b)*c/(d+e*e)", m, pf, answer) == 0 &&
		pf == "ab+c*dee*+/" && answer == 0);
	assert(evaluate("a+b*c/d+e*e", m, pf, answer) == 0 &&
		pf == "abc*d/+ee*+" && answer == 27);
	cout << "Passed all tests" << endl;
}
