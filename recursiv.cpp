#include "stdafx.h"
#include "stdafx.cpp"

int elim_cifre_imp(int n) {
	if (n == 0) return 0;
	if (n % 2 == 1) return elim_cifre_imp(n / 10);
	else return elim_cifre_imp(n / 10) * 10 + n % 10;
}
int elim_cifre_par(int n) {
	if (n == 0) return 0;
	if (n % 2 == 0) return elim_cifre_par(n / 10);
	else { return elim_cifre_par(n / 10) * 10 + n % 10; }
}
int elim_cifre_x(int n, int x) {
	if (n == 0) return 0;
	if (n % 10 == x) return elim_cifre_x(n / 10, x);
	else return elim_cifre_x(n / 10, x) * 10 + n % 10;
}
int elim_cifre_dif_x(int n, int x) {
	if (n == 0) return 0;
	if (n % 10 != x) return elim_cifre_x(n / 10, x);
	else return elim_cifre_x(n / 10, x) * 10 + n % 10;
}
int suma_cifre(int n) {
	if (n == 0) return 0;
	return n % 10 + suma_cifre(n / 10);
}
int factorial(int n) {
	if (n == 2) return 2;
	else return n * factorial(n - 1);
}
int suma_vector(int v[], int n) {
	if (n == -1) return 0;
	else return v[n] + suma_vector(v, n - 1);
}
void citire_vector(int v[], int n) {
	if (n == 0) {
		cout << "v[0]=";  cin >> v[0];
	}
	else {
		citire_vector(v, n - 1);
		cout << "v[" << n << "]="; cin >> v[n];
	}
}
void afisare_vector(int v[], int n) {
	if (n > 0)
		afisare_vector(v, n - 1);
	cout << v[n] << " ";
}
int suma_cifre_paritate(int n, int p) {
	if (n == 0) return 0;
	if (n % 2 == p)
		return  suma_cifre_paritate(n / 10, p) + n % 10;
	else
		return suma_cifre_paritate(n / 10, p);
}
void inlocuire_vect_2(int v[], int n) {
	if (n > -1) {

		inlocuire_vect_2(v, n - 1);
		v[n] = suma_cifre_paritate(v[n], n % 2);

	}
}
void citire_matrice(int v[][101], int m, int n) {
	if (m > 1)
		citire_matrice(v, m - 1, n);
	else if (n > 1) {
		citire_matrice(v, m, n - 1);
		cout << "v[" << m << "]" << "[" << n << "]=";
		cin >> v[m][n];
	}
}
void numar_cifre_dist(int n, int) {}
int mp(int n) {
	if (n >= 12)
		return n - 1;
	else {
		cout << n<<" ";
		return mp(mp(n + 2));
	}
}
void stea(int i) {
	if (i > 0) {
		cout << "*";
		stea(i - 1);
	}
}
void triunghi(int n,int x) {
	if (x <= (n*2-1)) {
		if (x < n) {
			stea(x);
			cout << endl;
		}
		else {
			stea(n-(x-n));
			cout << endl;
		}
		triunghi(n, x + 1);
	}
}
int verif_putere(int n) {
	if (n == 0)
		return -1;
	else
		return 1 + verif_putere(n / 2);
}
int descompunere(int n) {
	if(n==0)
		return 0;
	cout << int(pow(2, verif_putere(n))) << " ";
	return descompunere(n - int(pow(2, verif_putere(n))));
}
int prim(int n,int i) {

	if (i <= log(n))
		if (n % i == 0)
			return 0;
		else
			return prim(n, i + 1);
	else
		return 1;
}
int main() {
	int n = 3, v[100], pl[100][101], x = 3;

	ifstream f("input.in");
	ofstream g("output.out");

	//triunghi(50,1);
	//descompunere(345);
	cout<<prim(973,2);
	while (1);
	f.close();
	g.close();
	return 0;
}