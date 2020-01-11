// Weird GCD.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
using namespace std;
int checkvalue(char i1[99], char i2[99]) { //functie ce verifica care dintre 2 numere MARI 
	int i = 0;
	while (i < sizeof(i1)) {
		if (int(i1[i]) > int(i2[i]))
			return -1;
		else 
			if (int(i1[i]) < int(i2[i]))
				return -2;
		else i++;
	}

	return 0;
}
char substract(char i1[99], char i2[99]){
	char i3[sizeof(i1)], i4[sizeof(i1)];

		for (int i = strlen(i1); i > (strlen(i1) - strlen(i2)); i--)
			i3[i] = i2[i - (strlen(i1) - strlen(i2))];
		for (int i = strlen(i1); i > 0; i--)
			if (i1[i] > i2[i] || i1[i] == i2[i])
				i4[i] = i1[i] - i2[i];
			else if (i1[i - 1] > 0) {
				i1[i - 1] --;
				i4[i] = 10 - i1[i] - i2[i];
			}
			else i4[i] = 0;
			return *i4;

}
int main()
{
	char integer1[99],integer2[99];
	cin >> integer1 >> integer2;
	while (checkvalue(integer1, integer2)) {
		if (checkvalue(integer1, integer2) == -1)
			*integer1 = substract(integer1, integer2);
		else
			*integer2 = substract(integer2, integer1);
	}

	cout << integer2 << endl;
	while (1);
    return 0;
}

