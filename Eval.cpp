// Eval.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include <stdio.h>

using namespace std;
char Look; // variabila lookahead
int Mem[100]; //memoria pt vriabile
int c1, c2, c3, c4, c5, c6,c7;
void GetChar()
{
	cout << "Am intrat in GetChar() " << ++c6 << endl;
	
	cin >> Look;
	

	cout << "Look = " << Look << endl;
}

void Error(char *s)
{
	cout << endl << "Error: " << s << ".";
}

void Expected(char *s)
{
	cout << " Asteptam altceva:";
	Error(s);
	throw('E');
}

void Match(char x)
{
	cout << "Am intrat in Match() " << ++c7 << endl;
	char s[2];
	s[0] = x;
	if (Look == x) GetChar();
	else Expected(s);
}

int IsAddop(char c)
{
	return c == '+' || c == '-';
}

int IsMulop(char c)
{
	return c == '*' || c == '/' || c == '%';
}

int IsVar(char c)
{
	return c >= 'A' && c <= 'Z';
}

int IsConst(char c)
{
	return c >= '0' && c <= '9';
}

int GetVar()
{
	cout << "Am intrat in GetVar() " << ++c4 << endl;
	int Value;
	if (!IsVar(Look)) Expected("Astept o variabila.");
	Value = Mem[((int)Look - 65)];
	GetChar();
	cout << "Am iesit din Factor() " << c4 << endl;
	return Value;
}

int GetConst()
{
	cout << "Am intrat in GetConst() " << ++c5 << endl;
	int Value =0 ;
	if (!IsConst(Look)) Expected(" Astept o constanta. ");
	else {
		while (IsConst(Look)) {
			Value = Value * 10 + (int)Look - (int)'0';
			GetChar();
		}
	}
	cout << "Am iesit din GetConst() " << c5 << endl;
	return Value;
}

int Expression();

int Factor()
{
	cout << "Am intrat in Factor() " << ++c3<<" " <<Look<< endl;
	int Value;
	if (IsVar(Look)) Value = GetVar();
	else if (IsConst(Look)) Value = GetConst();
	else if (Look == '(') { Match('('); Value = Expression(); Match(')'); }
	else Expected("factor asteptat");
	cout << "Am iesit din Factor() " << c3 << endl;
	return Value;
}

int Term()
{
	cout << "Am intrat in Term() " << ++c2 << endl;
	int oldval, newval;
	oldval = Factor();
	while (IsMulop(Look))
		switch (Look)
		{
		case '*': Match('*'); newval = Factor(); oldval = oldval * newval; break;
		case '/': Match('/'); newval = Factor(); oldval = oldval / newval; break;
		case '%': Match('%'); newval = Factor(); oldval = oldval % newval; break;
		}
	cout << "Am iesit din Term() " << c2 << endl;
	return oldval;
}

int Expression()
{
	cout << "Am intrat in Expression() " << ++c1 << endl;
	int oldval, newval;
	oldval = Term();
	while (IsAddop(Look))
		switch (Look)
		{
		case '+': Match('+'); newval = Factor(); oldval = oldval + newval; break;
		case '-': Match('-'); newval = Factor(); oldval = oldval - newval; break;
		}
	cout << "Am iesit din Expression() " << c1 << endl;
	return oldval;
	
}

void MemInit()
{
	for (int i = 0; i<100; i++)
		Mem[i] = 0;
}

void Init()
{
	MemInit();
	GetChar();
}

int main()
{
	try {
		while (1)
		{
			cout << " Tastati expresia aritmetica urmata de un egal: " << endl;
			Init();
			cout << "Valoarea este:" << Expression() << " Apasa Enter !" << endl;
			getchar();
			getchar();
		}
	}
	catch (char s)
	{
	}
	
}