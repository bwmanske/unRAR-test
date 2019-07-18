// unRAR-test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <time.h>
#include <conio.h>
#include <string>

#include "Permutations.h"
#include "IniFile.h"
using namespace std;

const int  triesPerLine = 26;

long int   startValue = 113000; 
double     triesPerSec;
time_t     secondsDiff;
long long  pwdCount = 1;

string     FileName              = "unRAR-test.ini";
string     SectionName_CurrentState = "CurrentState";
string     SectionName_InitState    = "InitState";
string     SectionName_Performance  = "Performance";
string     Key_TrialCount        = "TrialCount";
string     Key_TrialValue        = "TrialValue";
string     Key_LAlpha            = "Use_Lower_Alpha_Chars";
string     Key_UAlpha            = "Use_Upper_Alpha_Chars";
string     Key_NAlpha            = "Use_Non_Alpha_Chars";
string     Key_Numeric           = "Use_Numeric_Chars";
string     Key_TrialsPerSec      = "Trials_PerSecond";
string     pwdCountStr           = "";

void infoLineStart();
void INI_Update(string pwdIndiciesStr);

int main()
{
	bool      exitRequest        = false;
	bool      showAllTrials      = true;
	bool      showThisTrial      = false;
	int       status_setNext;
	int       unrarStatus        = -1;
	int       showThisTrialCount = 0;
	time_t    currentSeconds;
	time_t    lastCurrentSeconds = 0;
	time_t    startSeconds       = time(NULL);

	char      tmpStr[250];
	char *    pwdStr = nullptr;

	Permutations *mutatePassword = new Permutations;
	if (CIniFile::DoesFileExist(FileName)) {
		std::cout << FileName << "INI File exists" << endl << endl;      // time to read the file

		if (CIniFile::SectionExists(SectionName_InitState, FileName)) {
			std::cout << SectionName_InitState << "Section exists" << endl << endl;

			string RetValue = "";
			RetValue = CIniFile::GetValue(Key_LAlpha, SectionName_InitState, FileName);
			RetValue[0] = toupper(RetValue[0]);
			if (RetValue.length() == 0 || (RetValue[0] != 'T' && RetValue[0] != 'F')) {
				if (CIniFile::SetValue(Key_LAlpha, "True", SectionName_InitState, FileName)) std::cout << Key_LAlpha << " was missing - created = True" << endl << endl;
			}
			else {
				if (RetValue[0] == 'T') mutatePassword->set_useLAlpha(true); else mutatePassword->set_useLAlpha(false);
			}

			RetValue = CIniFile::GetValue(Key_UAlpha, SectionName_InitState, FileName);
			RetValue[0] = toupper(RetValue[0]);
			if (RetValue.length() == 0 || (RetValue[0] != 'T' && RetValue[0] != 'F')) {
				if (CIniFile::SetValue(Key_UAlpha, "True", SectionName_InitState, FileName)) std::cout << Key_UAlpha << " was missing - created = True" << endl << endl;
			}
			else {
				if (RetValue[0] == 'T') mutatePassword->set_useUAlpha(true); else mutatePassword->set_useUAlpha(false);
			}

			RetValue = CIniFile::GetValue(Key_NAlpha, SectionName_InitState, FileName);
			RetValue[0] = toupper(RetValue[0]);
			if (RetValue.length() == 0 || (RetValue[0] != 'T' && RetValue[0] != 'F')) {
				if (CIniFile::SetValue(Key_NAlpha, "True", SectionName_InitState, FileName)) std::cout << Key_NAlpha << " was missing - created = True" << endl << endl;
			}
			else {
				if (RetValue[0] == 'T') mutatePassword->set_useNAlpha(true); else mutatePassword->set_useNAlpha(false);
			}

			RetValue = CIniFile::GetValue(Key_Numeric, SectionName_InitState, FileName);
			RetValue[0] = toupper(RetValue[0]);
			if (RetValue.length() == 0 || (RetValue[0] != 'T' && RetValue[0] != 'F')) {
				if (CIniFile::SetValue(Key_Numeric, "True", SectionName_InitState, FileName)) std::cout << Key_Numeric << " was missing - created = True" << endl << endl;
			}
			else {
				if (RetValue[0] == 'T') mutatePassword->set_useNumeric(true); else mutatePassword->set_useNumeric(false);
			}
		}
		else {
			std::cout << SectionName_InitState << "Section does not exist" << endl << endl;
		}

		if (CIniFile::SectionExists(SectionName_CurrentState, FileName)) {
			std::cout << SectionName_CurrentState << "Section exists" << endl << endl;

			string RetValue = "";
			std::string::size_type sz;   // alias of size_t
			RetValue = CIniFile::GetValue(Key_TrialCount, SectionName_CurrentState, FileName);
			if (RetValue.length() == 0 || !isdigit(RetValue[0])) {
				if (CIniFile::SetValue(Key_TrialCount, "0", SectionName_CurrentState, FileName)) std::cout << Key_TrialCount << " was missing - created = 0" << endl << endl;
			}
			else {
				startValue = stoi(RetValue, &sz);
			}
		}
		else {
			std::cout << SectionName_CurrentState << "Section does not exist" << endl << endl;
		}
	}
	else {
		if (CIniFile::Create(FileName)) {
			std::cout << FileName << " was successfully created" << endl << endl;
			mutatePassword->set_useLAlpha(true);
			mutatePassword->set_useUAlpha(true);
			mutatePassword->set_useNumeric(true);
			mutatePassword->set_useNAlpha(true);

			startValue = 0;

			if (CIniFile::SetValue(Key_LAlpha, "True", SectionName_InitState, FileName)) cout << Key_LAlpha << " was successfully created" << endl << endl;
			if (CIniFile::SetValue(Key_UAlpha, "True", SectionName_InitState, FileName)) cout << Key_UAlpha << " was successfully created" << endl << endl;
			if (CIniFile::SetValue(Key_NAlpha, "True", SectionName_InitState, FileName)) cout << Key_NAlpha << " was successfully created" << endl << endl;
			if (CIniFile::SetValue(Key_Numeric, "True", SectionName_InitState, FileName)) cout << Key_Numeric << " was successfully created" << endl << endl;

			if (CIniFile::SetValue(Key_TrialCount, "0", SectionName_CurrentState, FileName)) cout << Key_TrialCount << " was successfully created" << endl << endl;
			if (CIniFile::SetValue(Key_TrialValue, "a", SectionName_CurrentState, FileName)) cout << Key_TrialValue << " was successfully created" << endl << endl;
		}
		else {
			std::cout << "Failed to create the " << FileName << endl << endl;
		}
	}

	while (unrarStatus != 0 && !exitRequest) {
		if ((status_setNext = mutatePassword->setNext()) == 0) {
			if (pwdCount < startValue) {
				pwdCount++;
				continue;
			}
			pwdStr = mutatePassword->get_testPassword();
			if (pwdStr == nullptr) {
				std::cout << "invalid index in get_testPassword" << std::endl;
			}
			else {
				sprintf_s(tmpStr, sizeof(tmpStr), "\"c:\\Program Files\\WinRAR\\unrar.exe\" E -inul -o+ -P%s C:\\temp\\TMNTS01M6.part01.rar C:\\temp\\result", pwdStr);
				unrarStatus = system(tmpStr);
				if (showAllTrials || showThisTrial) {
					sprintf_s(tmpStr, sizeof(tmpStr), " %4s", pwdStr);
					std::cout << tmpStr;
					if (showThisTrial) {
						if (showThisTrialCount < (triesPerLine - 1)) showThisTrialCount++; else showThisTrial = false;
					}
				}
				switch (unrarStatus) {
				case 0:
					std::cout << std::endl << tmpStr << " Success" << std::endl;
					break;
				case 1:
					std::cout << "Warning" << std::endl;
					break;
				case 2:
					std::cout << "Fatal Error" << std::endl;
					break;
				case 3:
					std::cout << "CRC Error" << std::endl;
					break;
				case 4:
					std::cout << "Locked Archive" << std::endl;
					break;
				case 5:
					std::cout << "Write Error" << std::endl;
					break;
				case 6:
					std::cout << "Bad Archive Name" << std::endl;
					break;
				case 7:
					std::cout << "Bad Command Line Option" << std::endl;
					break;
				case 8:
					std::cout << "Memory Error" << std::endl;
					break;
				case 9:
					std::cout << "File Create Error" << std::endl;
					break;
				case 10:
					break;  // normal exit code for failure in this case
				default:
					std::cout << "ERROR: " << unrarStatus << std::endl;
				}

				currentSeconds = time(NULL);
				if ((secondsDiff = (currentSeconds - startSeconds)) > 0) {
					if (showAllTrials) {
						if (pwdCount > 0 && pwdCount % triesPerLine == 0) {
							infoLineStart();
						}
					}
					else {
						if (secondsDiff % 15 == 0 && !showThisTrial) {        // show current trials every 15 seconds
							infoLineStart();
							showThisTrial = true;
							showThisTrialCount = 0;
						}
					}

					if (secondsDiff % 600 == 0) {                             // upsate the ini file every 600 seconds
						INI_Update(mutatePassword->get_pwdAsIndicies());
					}

					if (currentSeconds != lastCurrentSeconds) {               // check for a keyboard char
						lastCurrentSeconds = currentSeconds;
						if (_kbhit() != 0) {
							char newChar = _getch();
							switch (newChar) {
							case 's':                                         // s toggle show all - show every 15 seconds
							case 'S':
								showAllTrials = !showAllTrials;
								break;
							case 'q':                                         // exit the program
							case 'Q':
								INI_Update(mutatePassword->get_pwdAsIndicies());
								exitRequest = true;
								break;
							default:
								break;
							}
						}
					}
				}
			}
		}
		else {
			std::cout << std::endl;
			switch (status_setNext)
			{
			case PERMUTE_DONE:
				std::cout << pwdStr << " All Passwords have been tried" << std::endl;
				break;
			case PERMUTE_NO_OPTION:
				std::cout << "No Password Options set" << std::endl;
				break;
			default:
				break;
			}
			break;
		}
		pwdCount++;
	}
	return 0;
}

void INI_Update(string pwdIndiciesStr)
{
	char tmpStr[250];

	pwdCountStr = to_string(pwdCount);
	CIniFile::SetValue(Key_TrialCount, pwdCountStr, SectionName_CurrentState, FileName);

	CIniFile::SetValue(Key_TrialValue, pwdIndiciesStr, SectionName_CurrentState, FileName);

	sprintf_s(tmpStr, sizeof(tmpStr), "%.2f", triesPerSec);
	CIniFile::SetValue(Key_TrialsPerSec, tmpStr, SectionName_Performance, FileName);
}

void infoLineStart(void)
{
	char tmpStr[250];

	triesPerSec = (double)(pwdCount - startValue) / secondsDiff;
	sprintf_s(tmpStr, sizeof(tmpStr), "\r\n %lld, %5.2f: ", pwdCount, triesPerSec);
	std::cout << tmpStr;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
