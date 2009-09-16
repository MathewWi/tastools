// SSCompress.cpp
// written by: adelikat
// purpose: to use pngout for optimal compression of game screenshots
// It is designed to be a windows application where the user simply needs to drag & drop the screenshot onto Screenshotcompress.exe

#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <Winbase.h>
#include <shellapi.h>

using namespace std;

#define sprintf sprintf_s

int main(int argc, char* argv[])
{
	bool failure = false;			//used for error checking with filestream, if this becomes true, abort program
	string filename = "test";		//The dafult if nothing is specified by the user
	string filenameFull;			//Will store the full path to the file that was compressed

	
	
	string testArray[16];			//Array of filename to test, will be populated based on filename, [0] will be a dummy
	FILE* files[16];				//[0] will be dummy

	//Get base directory
	char* getPath = (char*)malloc(2048*sizeof(char));
	GetModuleFileNameA(NULL, getPath, 2048);

	string baseDirectory = getPath;
	char slash = '\\';
	int x = baseDirectory.find_last_of(slash);
	baseDirectory = baseDirectory.substr(0,x+1);
	
	cout << "Working directory is:\n" << baseDirectory.c_str() << "\n\n"; //Debug

	if (argc > 1)
		filenameFull = argv[1];		//Get a filename if user chose one
	else
	{
		filenameFull = baseDirectory;
		filenameFull.append(filename);	//If filename isn't specified use test.png in the same dictory as the executable
		filenameFull.append(".png");
	}


	cout << "Filename is:\n" << filenameFull.c_str() << "\n\n"; //debug
	////////////////////////////////////////////////////////////////
	// Compress screenshot into 16 files
	////////////////////////////////////////////////////////////////

	//Populate the test array
	char temp[3] = "";
	for (int x = 1; x <= 15; x++)
	{
		testArray[x] = baseDirectory;
		testArray[x].append(filename);	//Add filename
		
		if (x < 10)
			testArray[x].append("0");	//Make 01, 02 etc.

		sprintf(temp, "%d", x);			//Append X
		testArray[x].append(temp);
		testArray[x].append(".png");

		//cout << testArray[x].c_str() << "\n"; //Debug
	}
	
	cout << "Opening pngbat.bat...\n";

	string parameters = "\"";
	parameters.append(filenameFull.c_str());	//Send pngbat.bat the filename
	parameters.append("\"");					//Enclose in quotes
	parameters.append(" ");
	parameters.append(baseDirectory);			//Send the basedirectory as parameter 2 (so it places test01.png in the correct place
	
	cout << "With paramters: " << parameters.c_str();

	SHELLEXECUTEINFOA ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = "pngbat.bat";		
	ShExecInfo.lpParameters = parameters.c_str();
	ShExecInfo.lpDirectory = baseDirectory.c_str();
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;	
	ShellExecuteExA(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	
	////////////////////////////////////////////////////////////////
	//Choose the smallest file
	////////////////////////////////////////////////////////////////
	
	//Open each file and do error checking
	for (int x = 1; x <= 15; x++)
	{
		files[x] = fopen(testArray[x].c_str(), "r");
		if (!files[x])
		{
			failure = true;
			cout << "\n" << testArray[x].c_str() << " file was not found line56";
			break;
		}
	}
	
	if (!failure)
	{
		//Find the lowest filesize
		long lowestSize = 0;
		int lowestNum = 1;
		long tempSize;
		for (int x = 1; x <= 15; x++)
		{
			fseek(files[x], 0, SEEK_END);
			tempSize = ftell(files[x]);
			//cout << "File number " << x << " - " << tempSize << " bytes\n"; //Debug
			if (x == 1)
				lowestSize = tempSize;
			else if (tempSize < lowestSize)
			{
				lowestNum = x;
				lowestSize = tempSize;
			}			
		}

		for (int x = 1; x <= 15; x++)
		{
			fclose(files[x]);
		}
		

		//cout << "\nThe lowest filsize is file number " << lowestNum << "\n" ;	//Debug

		//Delete all but the smallest file
		int fail = 0;
		
		fail = remove(filenameFull.c_str());

		if (fail)
			cout << "Could not remove base file\n";
		
		for (int x = 1; x <= 15; x++)
		{
			if (x != lowestNum)
			{
				fail = remove(testArray[x].c_str());
				//if (!fail)
				//	cout << "Removing file " << x << "\n";
				//else
				//	cout << "Could not remove file " << x << "\n";
			}
		}


		//Rename the lowest to filename
		rename(testArray[lowestNum].c_str(), filenameFull.c_str());
	}
	if (failure)
	{
		cout << "\nCould not open 1 or more files, aborting...";
		cout << "\nPress Enter key to exit";
		cin.ignore(cin.rdbuf()->in_avail()+1);
		return 1;
	}
	//Keep window open until user presses enter
	//cout << "\nPress Enter key to exit";
	//cin.ignore(cin.rdbuf()->in_avail()+1);

	return 0;
}