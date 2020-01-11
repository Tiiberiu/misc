// Lists.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

using namespace std;

struct nod_lista {

	int info;
	nod_lista * next;

};

nod_lista *inserare_nod(nod_lista *prim, nod_lista *ultim) {

	if (prim == NULL) {
		prim = new nod_lista;
		cout << "PRIM NOD:";
		cin >> prim->info;
		prim->next = NULL;
		ultim = prim;
		return prim;

	}
	else {
		nod_lista *c;
		c = new nod_lista;
		cout << "Info:";
		cin >> c->info;
		c->next = prim;
		prim = c;
		return prim;
	}
	
}
void afisare_lista(nod_lista *prim) {
	nod_lista *c;
	c = prim;
	while (c != NULL) {
		cout << c->info<<" ";
		c = c->next;
		//prim->next = prim;
	}
		
}

nod_lista* stergere_nod(nod_lista *prim, nod_lista *ultim) {

	if (prim != NULL) {
		prim->next = prim->next->next;
	}
	return prim;
}

int main()
{
	int c =2;
	nod_lista *ultim,*prim ,*a;
	ultim = new nod_lista;
	//prim = new nod_lista;
	prim = NULL;
	//prim->next = NULL;
	prim = inserare_nod(prim,ultim);
	prim = inserare_nod(prim, ultim);
	prim = stergere_nod(prim, ultim);
	afisare_lista(prim);


	//cout << prim->info<<endl;
	while (c);
	getchar();
    return 0;

}

