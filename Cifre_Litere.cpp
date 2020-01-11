#include "stdafx.h"
#include "stdafx.cpp"

using namespace std;
//1 234 567

char x[5][20] = { "o mie","zece","o suta "," de mii","un milion" };
char y[10][20] = { "zero","unu","doi","trei","patru","cinci","sase","sapte","opt","noua" };
char z[4][20] = { "zeci"," sute ","mii" };
string prefix,sufix;

void sute();

void zeci(int ok) {
	if (!ok) {
		for (int i = 1; i <= 9; i++) cout << prefix << y[i] << sufix << endl;
		cout << prefix << x[0] << sufix << endl;
		for (int j = 1; j <= 9; j++) {
			if (j == 1) cout << prefix << "un" << "spre" << x[0] << sufix << endl;
			else if (j == 4)cout << prefix << "pai" << "spre" << x[0] << sufix << endl;
			else if (j == 6)cout << prefix << "sai" << "spre" << x[0] << sufix << endl;
			else cout << prefix << y[j] << "spre" << x[0] << sufix << endl;
		}
		for (int l = 2; l <= 9; l++)
			for (int k = 1; k <= 9; k++) {
				if (l == 2)
					cout << prefix << "doua" << z[0] << " si " << y[k] << sufix << endl;
				else if (l == 6)
					cout << prefix << "sai" << z[0] << " si " << y[k] << sufix << endl;
				else cout << prefix << y[l] << z[0] << " si " << y[k] << sufix << endl;
			}

	}
}
void sute(int ok) {
	if (!ok) {
		prefix = x[1];
		zeci(0);
		for (int i = 2; i <= 9; i++) {
			prefix = y[i];
			prefix = prefix + z[1];
			zeci(0);
		}
	}
}
void mii() {
	prefix = "";
	sufix = x[3];
	zeci(0);
	sute(0);
}
void transforma_numere_in_litere(int n) {
	zeci(0);
	sute(0);
	mii();
}
int main() {

	transforma_numere_in_litere(3);
	while (1);
	ofstream g("output.out");
	g.close();
	return 0;
}