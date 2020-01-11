#include "stdafx.h"
#include "stdafx.cpp"
int x[17] = { 1,3,6,7,8,11,23,27,61,88,99,100 ,100,101,105};
int C[1000],y = 0;

int test1(int x = 0) {
	if (x < 0 || x>1)
		return 0;
	cout << "x = " << x << endl;
	test1(x + 1); 
	cout << "Recursion level is " << y++ << endl;
}

int repElem_search(int n, int T[]) {
	cout << "Search!" << endl;
	for (int i = 0; i < n; i++) {
		cout << "Search!" << endl;
	if (T[i] == T[i + 1])
		return T[i];
}
	return 0;
}
int repElement_binaryC(int T[], int high, int low = 0) {
	cout << "Search!" << endl;
	if (low > high)
		return 0;
	int mid = (high + low) / 2;
	cout << T[mid - 1] << " " << T[mid] << " " << T[mid + 1] << endl;
	if (T[mid] == T[mid - 1] || T[mid] == T[mid + 1]|| C[T[mid]] > 1|| C[T[mid+1]] > 1 || C[T[mid-1]] > 1) {
		return T[mid];
	}
	else {
		C[T[mid]]++;
		C[T[mid+1]]++;
		C[T[mid - 1]]++;
	}
	repElement_binaryC(T, mid - 1, low);
	repElement_binaryC(T, high, mid + 1);

}
int repElement_binaryS(int T[], int high,int low = 0) {
	cout << "Search!" << endl;
	if (low > high)
		return 0;
	int mid = (high + low) / 2;
	cout << T[mid - 1] << " " << T[mid] << " " << T[mid + 1] << endl;
	if (T[mid] == T[mid - 1] || T[mid] == T[mid + 1]) {
		return T[mid];
	}
	else cout << T[mid - 1] << " " << T[mid] << " " << T[mid + 1] << endl;
	repElement_binaryS(T, mid - 1, low);
	repElement_binaryS(T, high, mid + 1);

}
int hanoiT(int n) {
	if (n > 100)return 0;
	int A[100], B[100], C[100];
	for (int i = 0; i < n; i++)
		A[i] = i+1;
	while (n) {


	}
}

int main() {

	ifstream f("input.in");
	ofstream g("output.out");

	//cout << repElement_binaryS(x, 17,0) << endl << endl;
	//cout << repElement_binaryC(x, 17, 0) << endl << endl;
	//cout << repElem_search(17,x);
	cout<<test1();
	while (1);
	f.close();
	g.close();

	
	return 0;
}