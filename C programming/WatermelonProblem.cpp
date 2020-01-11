// WatermelonProblem.cpp : Defines the entry point for the console application.
// 2015

#include "stdafx.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
int tip_vector(int n, char m[2])
{

	int i = 0;

	while (n)
	{
		m[i] = '0' + n % 10;
		//cout << m[0] << endl;
		i++;
		n = n / 10;

	}
	return i;
}
int main()
{
	char a[100], x[10], m[2];
	int n, j = 0, z = 0,sum = 0,ok,ii,xx;
	cin >> n;
	while (n)
	{
		cin >> x;
		if (strlen(x) > 10) {
		
			z = 0;
			a[j] = x[0];
			j++;
			int l = tip_vector(strlen(x), m);
			sum =  sum + 2 + l;
			while (l)
			{
				z = l - 1;
				a[j] = m[z];
				z--;
				j++;
				l--;
			}
			a[j] = x[strlen(x) - 1];
			a[++j] = ' ';
			j++;
			n--;
			ok = 0;
		}
		if (strlen(x) < 10) {
		
			ii = 0;
			sum = sum + strlen(x)+1;
			xx = j;
			while(xx < strlen(x))
			{
				a[xx] = x[ii];
				cout << a[xx] << " ";
				xx++;
				ii++;
				cout << xx << endl;
			}
			j = j + strlen(x);
			a[j++] = ' ';
			n--;
			ok = 1;
			//cout << a;
		}
	}
	
	for (int hd = 0; hd < sum; hd++) {
		cout << a[hd];
	}
	while (1);
	return 0;
}

