// protocol_fisiere.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "filesCLass.h"
#include "dosar_fisiere.h"
#include <sstream>


#define STRUCT2 "ABCDEFGH000000000000000000000000000000000000000000000000000082.208.143.252004021"


struct fisierIncarcat {

	std::string id;
	std::string nume;
	std::string dimensiune;
	std::string data;
};

std::string trim(const std::string& str, const std::string& whitespace = " \t")
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

bool actualizeazaFisiereIncarcate(const char * buffer) {

	fisierIncarcat fisier;
	std::string padding_id, padding_nume, padding_dimensiune, padding_data;
	std::string temp(buffer);

	// 8 + 64 + 16 + 16
	if (temp.size() > 104 && temp.size() < 128) {

		padding_id = temp.substr(0, 8);
		padding_nume = temp.substr(8, 64);
		padding_dimensiune = temp.substr(72, 16); // 64 + 8
		padding_data = temp.substr(88, 16); 

		fisier.id = trim(padding_id, "0");
		fisier.nume = trim(padding_nume, "0");
		fisier.dimensiune = trim(padding_dimensiune, "0");
		fisier.data = trim(padding_data, "0");

	}
	return 1;
}

std::string convert(char const(&data)[9])
{
	return std::string(data, std::find(data, data + 9, '\0'));
}

std::string gen_random(const int len) {
	char s[9];
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
	return convert(s);
}

std::vector<char> packStringVector(std::vector<std::string> sursa)
{
	std::stringstream ss;
	std::string temp;

	for (auto i = sursa.begin(); i != sursa.end(); i++) {
		ss << *i << '|';
	}
	temp = ss.str();
	std::vector<char> mesaj(temp.begin(), temp.end());
	mesaj.push_back('\0');

	return mesaj;

}

std::vector<std::string> unpackCharVector(std::vector<char> sursa)
{
	std::vector<char> vect;
	std::vector<std::string> fisiere;
	std::string temp(sursa.begin(),sursa.end());
	
	std::stringstream ss(temp);
	char c;

	while (ss >> c)
	{
		vect.push_back(c);

		if (ss.peek() == '|') {
			std::string temp2(vect.begin(),vect.end());
			fisiere.push_back(temp2);
			vect.clear();
			ss.ignore();

		}
	}

	return fisiere;
}



struct hartaFisiere {
	std::string id;
	std::string ip;
	std::string port;
};

void construiesteHartaAdresePeers(std::vector<std::string> buf_adrese) {

	hartaFisiere fisier;
	std::vector<std::string>::iterator iter;
	std::string padding_id, padding_ip, padding_port;
	std::string temp;

	for (iter = buf_adrese.begin(); iter != buf_adrese.end(); iter++) {
		temp = *iter;
		padding_id = temp.substr(0, 8);
		padding_ip = temp.substr(8, 66);
		padding_port = temp.substr(74, temp.length());

		fisier.id = trim(padding_id, "0");
		fisier.ip = trim(padding_ip, "0");
		fisier.port = trim(padding_port, "0");

		std::cout << fisier.id << " || " << fisier.ip << " || " << fisier.port << " || "<< temp.size()<< std::endl;
	}
	

}

int main()
{
	
	//filesClass total;
	//std::vector<std::string> tester;
	//for (int i = 0; i <= 10; i++) {
	//	tester.push_back(gen_random(8));
	//}

	//std::cout << "--------------------------\n";
	//total.actualizeazaListaFisiere({"1234","2345","3456"}, 542);
	//total.actualizeazaListaFisiere({ "1234","2345","3456" }, 543);
	//std::cout << total.numarClientiFisier("1234");
	////total.adaugaFisier("12345678", 4);
	////total.adaugaFisier("12345674", 5);
	//total.printFisier();
	//std::cout << "\n--------------------------";

	dosar_fisiere test1;
	std::vector<std::string> date = test1.getFisiere();
	
	std::vector<char> date3(date[0].begin(), date[0].end());

	actualizeazaFisiereIncarcate(&date3[0]);

	
	//std::vector<std::string> date2;
	//date2.push_back(STRUCT2);
	//construiesteHartaAdresePeers(date2);
	//for (auto i = date2.begin(); i != date2.end(); ++i)
	//	std::cout << *i << '\n';

	while (1);
    return 0;
}

