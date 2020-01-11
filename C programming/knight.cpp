#include "stdafx.h"
#include "stdafx.cpp"

int PathMatrix[100][100],moves = 1;
/*
int x, y;
bool InMatrix(int M[100][100], int x, int y, int n) {
	return(x>-1 && x<n && y>-1 && y<n && M[x][y] == 1);
}

vector<string> printPath(int m[MAX][MAX], int n)
{
	int x_path[4] = { -1,0,1,-1 };
	int y_path[4] = { 0,1,0,-1 };
	string path[4] = { "U","R","D","L" };

	for (int i = 0; i<n; i++) {
		x += x_path[i];
		y += y_path[i];
		if (InMatrix(m, x, y, n)) {
			cout << (path[i]);
			printPath(m, n);

		}



	}
}*/
int DigitCntMax(long long i);
void PrintSpaces(int n);
void MatrixFixed(int M[100][100], int n);
bool MoveOK(int m[100][100],int x, int y, int n) {
	return (x >= 0 && x < n && y >= 0 && y < n && m[x][y] == 0);
}
void KnightTraversal(int M[100][100],int n,int knight_x, int knight_y){
	int xMove[8] = { 2, 1, -1, -2, -2, -1,  1,  2 };
	int yMove[8] = { 1, 2,  2,  1, -1, -2, -2, -1 };
	int next_y, next_x;
	for (int i = 0; i < 8; i++) {
		next_x = knight_x + xMove[i];
		next_y = knight_y + yMove[i];
		if (MoveOK(M,next_x, next_y, n)) {
			M[next_x][next_y] = moves++;
			KnightTraversal(M, n, next_x, next_y);
		}	
	}

}
void MatrixInit(int M[100][100]) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			M[i][j] = 0;
}
void MatrixPrint(int M[100][100]) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++)
			cout << M[i][j] << " ";
		cout << endl;
	}
}
void Knight() {
	int M[100][100], n = 8, x = 0, y = 0;
	MatrixInit(M);
	KnightTraversal(M, n, x, y);
	MatrixFixed(M,n);
}
int DigitCntMax(long long i) {
	return i > 0 ? (int)log10((double)i) + 1 : 1;
}
void PrintSpaces(int n) {
	while (n) {
		cout << " ";
		n--;
	}

}
void MatrixFixed(int M[100][100],int n) {
	int max = 0;
	for (int i = 0; i < n; i++) 
		for (int j = 0; j < n; j++)
			if (DigitCntMax(M[i][j])> max)
				max = DigitCntMax(M[i][j]);


	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			PrintSpaces(max - DigitCntMax(M[i][j]));
			cout << M[i][j] << " ";
		}
		cout << endl;
	}
}

int main() {
	Knight();
	cin.get();
	return 0;
}