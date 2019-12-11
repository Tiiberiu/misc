#include <iostream>
#include <bitset>
#include <sstream>


const char* hex_char_to_bin(char c)
{
	switch (toupper(c))
	{
	case '0': return "0000";
	case '1': return "0001";
	case '2': return "0010";
	case '3': return "0011";
	case '4': return "0100";
	case '5': return "0101";
	case '6': return "0110";
	case '7': return "0111";
	case '8': return "1000";
	case '9': return "1001";
	case 'A': return "1010";
	case 'B': return "1011";
	case 'C': return "1100";
	case 'D': return "1101";
	case 'E': return "1110";
	case 'F': return "1111";
	}
}

std::string hex_str_to_bin_str(const std::string& hex)
{
	std::string bin;
	for (unsigned i = 0; i != hex.length(); ++i)
		bin += hex_char_to_bin(hex[i]);
	return bin;
}

void convertToBinary(unsigned int n)
{
	if (n / 2 != 0) {
		convertToBinary(n / 2);
	}
	printf("%d", n % 2);
}


#define ARBITRARY 300
#define INT_VALUE 33000

// WEIRD UNEXPECTED BEHAVIOR ON SHORT

int main()
{
	/*  
	If a is smaller than 2^15-1 everything works fine 
	If a is > 2^15-1 && < 2^32-1 the value of k (*k) becomes negative but 
						behavior is normal for addresing
	If a is > than 2^32-1, k becomes positive, but addresing breakes 
	*/

	int a = INT_VALUE;
	short* k = (short*)&a;
	
	std::stringstream sa,sk;
	sa << (&a); std::string a_name = sa.str();
	sk << k; std::string k_name = sk.str();
	
	printf("Address of a is %p (", (void *)&a);
	std::cout << hex_str_to_bin_str(a_name) <<")"<< std::endl;
	printf("Address of k is %p (", (void *)k);
	std::cout << hex_str_to_bin_str(k_name) <<")"<<std::endl;

	std::cout << *k << " = ";
	convertToBinary((unsigned int)*k);
	std::cout << std::endl;

	std::cout << a << " = ";
	convertToBinary(a);
	std::cout << std::endl;

	*k = ARBITRARY;

	printf("Address of a is %p (", (void *)&a);
	std::cout << hex_str_to_bin_str(a_name) << ")" << std::endl;
	printf("Address of k is %p (", (void *)k);
	std::cout << hex_str_to_bin_str(k_name) << ")" << std::endl;

	std::cout << a << " = ";
	convertToBinary(a);
	std::cout << std::endl;

	std::cout << *k << " = ";
	convertToBinary((unsigned int)*k);

	std::string s;
	std::cin>>s;

    return 0;
}
