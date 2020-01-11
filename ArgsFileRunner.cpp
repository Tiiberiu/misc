// Executor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <shlwapi.h>
#include <thread>
#include <sstream>
#include <fstream>
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#pragma comment(lib, "shlwapi.lib")
// "NUME_PROGRAM" "FILE PATH"/"FILE NAME IN ENV"  "number of iterations"  "NAME TO SAVE WITH"
//  Executor				program_test.exe              1000				program_text_output.txt
//      arg0				arg1						  arg2					arg3

std::string ExecCmd(const wchar_t* cmd)
{
	std::string strResult;
	HANDLE hPipeRead, hPipeWrite;

	SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
	saAttr.bInheritHandle = TRUE;   //Pipe handles are inherited by child process.
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe to get results from child's stdout.
	if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0))
		return strResult;

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = hPipeWrite;
	si.hStdError = hPipeWrite;
	si.wShowWindow = SW_HIDE;       // Prevents cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.

	PROCESS_INFORMATION pi = { 0 };

	BOOL fSuccess = CreateProcessW(NULL, (LPWSTR)cmd, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!fSuccess)
	{
		std::cout << "CreateProcess failed: " << GetLastError() << std::endl;
		CloseHandle(hPipeWrite);
		CloseHandle(hPipeRead);
		return strResult;
	}

	bool bProcessEnded = false;
	for (; !bProcessEnded;)
	{
		// Give some timeslice (50ms), so we won't waste 100% cpu.
		bProcessEnded = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;

		// Even if process exited - we continue reading, if there is some data available over pipe.
		for (;;)
		{
			char buf[1024];
			DWORD dwRead = 0;
			DWORD dwAvail = 0;

			if (!::PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwAvail, NULL))
				break;

			if (!dwAvail) // no data available, return
				break;

			if (!::ReadFile(hPipeRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
				// error, the child process might ended
				break;

			buf[dwRead] = 0;
			strResult += buf;
		}
	} //for

	CloseHandle(hPipeWrite);
	CloseHandle(hPipeRead);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return strResult;
} //ExecCmd

int checkFileOrDirectory(std::string path) {
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
		{
			//it's a directory
			return 1;
		}
		else if (s.st_mode & S_IFREG)
		{
			//it's a file
			return 2;
		}
		else
		{
			//something else
			return 0;
		}
	}
	else
	{
		//error
		return -1;
	}
}

int checkValidPath(std::string aux, std::string &path) {

	std::ifstream validPath(aux);

	if (validPath) {
#if defined(_WIN32) || defined(WIN32)
		int pos = aux.rfind('\\');
#else
		int pos = aux.rfind('/');
#endif

		path = aux.substr(0, pos + 1);
	}
	else {
		//	std::wcerr << "Path: " << aux.c_str() << " is not valid" << '\n';
		return -1;
	}
	validPath.close();
	return 0;

}

int checkValidProgram(std::string aux, std::string directory_path, std::string &executable_path) {
	//std::string x = ExecCmd(L"vb");

	std::string x = "";
	if (checkFileOrDirectory(aux) == 2) {
		executable_path = aux;
		return 0;
	}
	else if (checkFileOrDirectory(directory_path + aux) == 2) {
		executable_path = directory_path + aux;
		return 0;
	}

	return -1;
}

int checkValidIterationNumber(std::string iter_arg, int &number) {

	if (strspn(iter_arg.c_str(), "0123456789") == iter_arg.size()) {
		std::istringstream ss(iter_arg);
		ss >> number;
		return 0;
	}
	return -1;
}

int checkValidOutputFile(std::string output_file, std::string directory_path, std::string &output_path) {

	std::string extension(PathFindExtensionA(output_file.c_str()));
	std::string temp;

	if (extension == "")
		extension = ".txt";

	//DIR
	if (checkFileOrDirectory(output_file) == 1) {
		output_path = output_file + "EXECUTOR" + extension;
		return 0;
	}

	//CONTAINS DIR
	else if (!checkValidPath(output_file, temp)) {
		if (checkFileOrDirectory(temp) == 1) {

#if defined(_WIN32) || defined(WIN32)
			int pos = output_file.rfind('\\');
#else
			int pos = aux.rfind('/');
#endif
			output_file = output_file.substr(pos, output_file.size());
			output_path = temp + output_file + extension;
			return 0;
		}
		return -1;
	}

	//CONTAINS A NAME
	else if (!output_file.empty()) {
		output_path = directory_path + "EXECUTOR" + extension;
		return 0;
	}


	return -1;
}

int main(int nrArgs, char *args[])
{
	std::string optional_args = "--none--";
	std::string executable_path, output_path, path;

	int iterations;

	if (nrArgs == 5) {
		optional_args = args[4];
	}

	if (nrArgs > 5) {

	USAGE:std::wcerr << "Usage: \n"
		<< "1.  |  " << args[0] << "  |  <--- This program" << '\n'
		<< "2.  |  " << args[1] << "  |  <--- Program to be executed" << '\n'
		<< "3.  |  " << args[2] << "  |  <--- How many times to be executed" << '\n'
		<< "4.  |  " << args[3] << "  |  <--- Where to store the output" << '\n'
		<< "5.  |  " << optional_args.c_str() << "  |  <--- <OPTIONAL> Arguments to run the program" << '\n';

		  return -1;
	}

	//Could be extended to support output path
	if (checkValidPath(args[0], path))
		goto USAGE;

	if (checkValidProgram(args[1], path, executable_path))
		goto USAGE;

	if (checkValidIterationNumber(args[2], iterations))
		goto USAGE;

	if (checkValidOutputFile(args[3], path, output_path))
		goto USAGE;


	std::cout << "path from where Executor started = " << path << '\n';
	std::cout << "executable_path = " << executable_path << '\n';
	std::cout << "iterations = " << iterations << '\n';
	std::cout << "output_path = " << output_path << '\n';
	std::cout << "optional_args = " << optional_args << '\n';

	if (optional_args == "--none--")
		optional_args = "";
	std::string sss = path + executable_path + std::to_string(iterations) + output_path + optional_args;
	std::string cmd(executable_path);
	std::string s_open = "open";

	std::wstring wide_open = std::wstring(s_open.begin(), s_open.end());
	std::wstring wide_s = std::wstring(cmd.begin(), cmd.end());

	//ShellExecuteW(NULL, wide_open.c_str(), wide_s.c_str(), NULL, NULL, SW_SHOW);

	/*HINSTANCE ShellExecuteW(
	HWND    hwnd,
	LPCWSTR lpOperation,
	LPCWSTR lpFile,
	LPCWSTR lpParameters,
	LPCWSTR lpDirectory,
	INT     nShowCmd
	);*/
	std::string output = ExecCmd(wide_s.c_str());
	std::cout << output;
	//std::thread newThread1(foo, 1);
	//newThread1.join();

	//std::thread newThread2(foo, 2);
	//newThread2.join();



	while (1);
	return 0;
}

