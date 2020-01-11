// CopyClipboard.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <chrono>
#include <thread>
#include <exception>
#include <iostream>
#include <ostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <windows.h>
using namespace std;

class RaiiClipboard
{
public:
	RaiiClipboard()
	{
		if (!OpenClipboard(nullptr))
			throw runtime_error("Can't open clipboard.");
		// ... or define some custom exception class for clipboard errors.
	}

	~RaiiClipboard()
	{
		CloseClipboard();
	}

	// Ban copy   
private:
	RaiiClipboard(const RaiiClipboard&);
	RaiiClipboard& operator=(const RaiiClipboard&);
};

class RaiiTextGlobalLock
{
public:
	explicit RaiiTextGlobalLock(HANDLE hData)
		: m_hData(hData)
	{
		m_psz = static_cast<const char*>(GlobalLock(m_hData));
		if (!m_psz)
			throw runtime_error("Can't acquire lock on clipboard text.");
	}

	~RaiiTextGlobalLock()
	{
		GlobalUnlock(m_hData);
	}

	const char* Get() const
	{
		return m_psz;
	}

private:
	HANDLE m_hData;
	const char* m_psz;

	// Ban copy
	RaiiTextGlobalLock(const RaiiTextGlobalLock&);
	RaiiTextGlobalLock& operator=(const RaiiTextGlobalLock&);
};

string GetClipboardText()
{
	RaiiClipboard clipboard;

	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == nullptr)
		throw runtime_error("Can't get clipboard text.");

	RaiiTextGlobalLock textGlobalLock(hData);
	string text(textGlobalLock.Get());

	return text;
}

int main()
{
	static const int kExitOk = 0;
	static const int kExitError = 1;
	std::cout << "Output path is: " << "D:\\clipboard.txt \n";
	std::cout << "If error at start, copy something to clipboard and try again \n";

	ofstream clipboard_output("D:\\clipboard.txt");
	std::vector<std::string> clipboard_vector;

	try
	{
		std::string clipboard_s = GetClipboardText();
		while ((clipboard_s != "stop" || clipboard_vector.size() > 100 )) {

			if ((clipboard_s != "" || clipboard_s.find_first_not_of(' ') != std::string::npos )&& !(std::find(clipboard_vector.begin(), clipboard_vector.end(), clipboard_s) != clipboard_vector.end())) {
				clipboard_output << clipboard_s << endl;
				clipboard_vector.push_back(clipboard_s);

			}
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			clipboard_s = GetClipboardText();
		}

		clipboard_output.close();

		return kExitOk;
	}
	catch (const exception& e)
	{
		cerr << "*** ERROR: " << e.what() << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(2200));
		return kExitError;
	}
}
