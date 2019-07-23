// unRAR-test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <time.h>
#include <conio.h>
#include <string>

#include "Permutations.h"
#include "IniFile.h"

const int  triesPerLine = 13;   // 26 upper 26 lower and 26 other (10 digit + 16 punctuation)

bool       showElapsedTime = false;
bool       showElapsedTimeTotal = false;
long int   startValue = 0;
long int   pastTotalSeconds = 0;
double     triesPerSec;
time_t     secondsDiff;
long long  pwdCount = 1;
int        timePrecisionValue = 2;

// Filename for the INI file
string     FileName                 = "unRAR-test.ini";

// CurrentState Section
string     SectionName_CurrentState = "CurrentState";
string     Key_TrialCount           = "TrialCount";
string     Key_TrialValue           = "TrialValue";

// InitState Section
string     SectionName_InitState    = "InitState";
string     Key_LAlpha               = "Use_Lower_Alpha_Chars";
string     Key_UAlpha               = "Use_Upper_Alpha_Chars";
string     Key_NAlpha               = "Use_Non_Alpha_Chars";
string     Key_Numeric              = "Use_Numeric_Chars";

// Performance Section
string     SectionName_Performance  = "Performance";
string     Key_TrialsPerSec         = "Trials_PerSecond";
string     Key_TotalSeconds         = "TotalSeconds";
string     Key_AvgTrialsPerSec      = "AvgTrials_PerSec";

// 
string     pwdCountStr              = "";

string     elapsedTime();
void       infoLineStart();
void       INI_Update(string pwdIndiciesStr, string pwdStr);

int main()
{
	bool      exitRequest        = false;
	bool      showAllTrials      = true;
	bool      showThisTrial      = false;
	int       status_setNext;
	int       unrarStatus        = -1;
	int       showThisTrialCount = 0;
	time_t    currentSeconds;
	time_t    startSeconds       = time(NULL);
	time_t    lastCurrentSeconds = startSeconds - 1;

	char      tmpStr[250];
	char *    pwdStr = nullptr;

	Permutations *mutatePassword = new Permutations;
	if (CIniFile::DoesFileExist(FileName)) {
		std::cout << FileName << " - INI File exists" << endl << endl;      // time to read the file

		if (CIniFile::SectionExists(SectionName_InitState, FileName)) {
			std::cout << SectionName_InitState << " - Section exists" << endl;

			string RetValue = "";
			RetValue = CIniFile::GetValue(Key_LAlpha, SectionName_InitState, FileName);
			RetValue[0] = toupper(RetValue[0]);
			if (RetValue.length() == 0 || (RetValue[0] != 'T' && RetValue[0] != 'F')) {
				if (CIniFile::SetValue(Key_LAlpha, "True", SectionName_InitState, FileName)) cout << Key_LAlpha << " was missing - created = True" << endl << endl;
			}
			else {
				if (RetValue[0] == 'T') mutatePassword->set_useLAlpha(true); else mutatePassword->set_useLAlpha(false);
				cout << Key_LAlpha << " read - " << ((RetValue[0] == 'T') ? "true" : "false") << endl;
			}

			RetValue = CIniFile::GetValue(Key_UAlpha, SectionName_InitState, FileName);
			RetValue[0] = toupper(RetValue[0]);
			if (RetValue.length() == 0 || (RetValue[0] != 'T' && RetValue[0] != 'F')) {
				if (CIniFile::SetValue(Key_UAlpha, "True", SectionName_InitState, FileName)) cout << Key_UAlpha << " was missing - created = True" << endl << endl;
			}
			else {
				if (RetValue[0] == 'T') mutatePassword->set_useUAlpha(true); else mutatePassword->set_useUAlpha(false);
				cout << Key_UAlpha << " read - " << ((RetValue[0] == 'T') ? "true" : "false") << endl;
			}

			RetValue = CIniFile::GetValue(Key_NAlpha, SectionName_InitState, FileName);
			RetValue[0] = toupper(RetValue[0]);
			if (RetValue.length() == 0 || (RetValue[0] != 'T' && RetValue[0] != 'F')) {
				if (CIniFile::SetValue(Key_NAlpha, "True", SectionName_InitState, FileName)) cout << Key_NAlpha << " was missing - created = True" << endl << endl;
			}
			else {
				if (RetValue[0] == 'T') mutatePassword->set_useNAlpha(true); else mutatePassword->set_useNAlpha(false);
				cout << Key_NAlpha << " read - " << ((RetValue[0] == 'T') ? "true" : "false") << endl;
			}

			RetValue = CIniFile::GetValue(Key_Numeric, SectionName_InitState, FileName);
			RetValue[0] = toupper(RetValue[0]);
			if (RetValue.length() == 0 || (RetValue[0] != 'T' && RetValue[0] != 'F')) {
				if (CIniFile::SetValue(Key_Numeric, "True", SectionName_InitState, FileName)) cout << Key_Numeric << " was missing - created = True" << endl << endl;
			}
			else {
				if (RetValue[0] == 'T') mutatePassword->set_useNumeric(true); else mutatePassword->set_useNumeric(false);
				cout << Key_Numeric << " read - " << ((RetValue[0] == 'T') ? "true" : "false") << endl;
			}
			cout << endl;
		}
		else {
			std::cout << SectionName_InitState << " - Section does not exist" << endl << endl;
		}

		if (CIniFile::SectionExists(SectionName_CurrentState, FileName)) {
			std::cout << SectionName_CurrentState << " - Section exists" << endl;

			string RetValue = "";
			std::string::size_type sz;      // alias of size_t

			// Read the Trial Count
			RetValue = CIniFile::GetValue(Key_TrialCount, SectionName_CurrentState, FileName);
			if (RetValue.length() == 0 || !isdigit(RetValue[0])) {
				if (CIniFile::SetValue(Key_TrialCount, "0", SectionName_CurrentState, FileName)) cout << Key_TrialCount << " was missing - created = 0" << endl << endl;
			}
			else {
				startValue = stoi(RetValue, &sz);
				if (startValue > 0) {
					std::cout << Key_TrialCount << " read - " << startValue << endl;
				}
				else {
					if (CIniFile::SetValue(Key_TrialCount, "0", SectionName_CurrentState, FileName)) cout << Key_TrialCount << " was missing - created = 0" << endl << endl;
				}
			}

			// Read the Trial Value in the form <index count>,<index 1>,...,<index N>
			if (startValue > 0) {
				RetValue = CIniFile::GetValue(Key_TrialValue, SectionName_CurrentState, FileName);
				if (RetValue.length() == 0) {
					std::cout << Key_TrialValue << " was missing" << endl << endl;
				}
				else {
					std::cout << Key_TrialValue << " read string - " << RetValue << endl;

					std::string::size_type sz;      // alias of size_t
					bool validString = false;
					int from = 0;
					int tmpInt[MAX_PWD_LEN + 1];
					int tmpIntIndex = 0;
					for (std::size_t i = 0; i < RetValue.size() && tmpIntIndex <= MAX_PWD_LEN; ++i) {
						if (RetValue[i] == ',') {
							tmpInt[tmpIntIndex++] = stoi(RetValue.substr(from, i), &sz);
							from = i + 1;
						}
					}
					if (from <= RetValue.size() && tmpIntIndex <= MAX_PWD_LEN) {
						tmpInt[tmpIntIndex++] = stoi(RetValue.substr(from, RetValue.size()), &sz);

						// validate the values read
						validString = true;
						if (tmpInt[0] > 0 && tmpInt[0] <= MAX_PWD_LEN && tmpIntIndex > 1 && tmpIntIndex <= (MAX_PWD_LEN + 1)) {
							for (int i = 1; i <= MAX_PWD_LEN && i < tmpIntIndex; i++) {
								if (tmpInt[i] >= 0 && tmpInt[i] <= RANGE_NA_UPPER) {
									continue;
								}
								validString = false;
							}
						}
						else {
							validString = false;
						}
					}
					if (!validString) {
						cout << "Read of " << Key_TrialValue << "failed - finding key by iteration of " << Key_TrialValue << endl;
					}
					else {
						mutatePassword->set_testPwdLen(tmpInt[0]);
 						cout << Key_TrialValue << " read ";
						int j = 0;
						for (int i = tmpInt[0]; i >= 1; i--) {
							mutatePassword->set_charIndicies(j++, tmpInt[i]);
							cout << tmpInt[i];
							if (i > 1) cout << ", "; else cout << endl;
						}
						pwdStr = mutatePassword->get_plainPassword();
						cout << "plain TrialValue - " << pwdStr << endl;
						pwdCount = startValue;
					}
				}
			}
			cout << endl;
		}
		else {
			std::cout << SectionName_CurrentState << " - Section does not exist" << endl << endl;
		}

		if (CIniFile::SectionExists(SectionName_Performance, FileName)) {
			std::cout << SectionName_Performance << " - Section exists" << endl;

			string RetValue = "";
			std::string::size_type sz;   // alias of size_t
			RetValue = CIniFile::GetValue(Key_TotalSeconds, SectionName_Performance, FileName);
			if (RetValue.length() == 0 || !isdigit(RetValue[0])) {
				if (CIniFile::SetValue(Key_TotalSeconds, "0", SectionName_Performance, FileName)) cout << Key_TotalSeconds << " was missing - created = 0" << endl << endl;
			}
			else {
				pastTotalSeconds = stoi(RetValue, &sz);
				cout << Key_TotalSeconds << " read - " << pastTotalSeconds << endl;
			}
			cout << endl;
		}
		else {
			std::cout << SectionName_Performance << " - Section does not exist" << endl << endl;
		}
	}
	else {
		if (CIniFile::Create(FileName)) {
			std::cout << FileName << " - was successfully created" << endl << endl;
			mutatePassword->set_useLAlpha(true);
			mutatePassword->set_useUAlpha(true);
			mutatePassword->set_useNumeric(true);
			mutatePassword->set_useNAlpha(true);

			startValue = 0;

			if (CIniFile::SetValue(Key_LAlpha,  "True", SectionName_InitState, FileName)) cout << Key_LAlpha  << " was successfully created" << endl << endl;
			if (CIniFile::SetValue(Key_UAlpha,  "True", SectionName_InitState, FileName)) cout << Key_UAlpha  << " was successfully created" << endl << endl;
			if (CIniFile::SetValue(Key_NAlpha,  "True", SectionName_InitState, FileName)) cout << Key_NAlpha  << " was successfully created" << endl << endl;
			if (CIniFile::SetValue(Key_Numeric, "True", SectionName_InitState, FileName)) cout << Key_Numeric << " was successfully created" << endl << endl;

			if (CIniFile::SetValue(Key_TrialCount, "0", SectionName_CurrentState, FileName)) cout << Key_TrialCount << " was successfully created" << endl << endl;
			if (CIniFile::SetValue(Key_TrialValue, "a", SectionName_CurrentState, FileName)) cout << Key_TrialValue << " was successfully created" << endl << endl;

			if (CIniFile::SetValue(Key_TotalSeconds, "0", SectionName_Performance, FileName)) cout << Key_TotalSeconds << " was successfully created" << endl << endl;
		}
		else {
			std::cout << "Failed to create the file - " << FileName << endl << endl;
		}
	}

	cout << "Press 'h' - Help on keystroke actions" << endl << endl;

	while (unrarStatus != 0 && !exitRequest) {
		if ((status_setNext = mutatePassword->setNext()) == 0) {
			if (pwdCount < startValue) {
				pwdCount++;
				continue;
			}
			pwdStr = mutatePassword->get_plainPassword();
			if (pwdStr == nullptr) {
				std::cout << "invalid index in get_testPassword" << std::endl;
			}
			else {
				sprintf_s(tmpStr, sizeof(tmpStr), "\"c:\\Program Files\\WinRAR\\unrar.exe\" E -inul -o+ -P%s C:\\temp\\TMNTS01M6.part01.rar C:\\temp\\result", pwdStr);
				unrarStatus = system(tmpStr);
				if (showAllTrials || showThisTrial) {
					sprintf_s(tmpStr, sizeof(tmpStr), " %*s", max(4, mutatePassword->get_testPwdLen()), pwdStr);
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
					if (currentSeconds != lastCurrentSeconds) {               // make sure at least a second has passed before checking these
						lastCurrentSeconds = currentSeconds;

						if (_kbhit() != 0) {                                  // check for a keyboard char
							char newChar = _getch();
							switch (newChar) {
							case 'h':                                         // display some Key Help
							case 'H':
								cout << endl
									<< "Key Help" << endl
									<< " c, C - Show INI Content" << endl
									<< " h, H - Show KEY help" << endl
									<< " q, Q - Exit this program" << endl
									<< " s, S - Toggle Show all passwords - show every 15 seconds" << endl
									<< " t    - Toggle show time for this session" << endl
									<< " T    - Toggle show total time" << endl
									<< " 0-3  - 0-time in seconds 1-3 precision of time (Min, Hr, Days)" << endl;
								break;
							case 'c':                                         // Display INI content
							case 'C':
								INI_Update(mutatePassword->get_pwdAsIndicies(), mutatePassword->get_pwdAsText());
								cout << endl
									 << "++++++++++++++++++++++++++++++++++++++++" << endl
									 << CIniFile::Content(FileName)
									 << "++++++++++++++++++++++++++++++++++++++++" << endl;
								break;
							case 's':                                         // s toggle show all - show every 15 seconds
							case 'S':
								showAllTrials = !showAllTrials;
								break;
							case 'q':                                         // exit the program
							case 'Q':
								INI_Update(mutatePassword->get_pwdAsIndicies(), mutatePassword->get_pwdAsText());
								cout << endl
									 << "++++++++++++++++++++++++++++++++++++++++" << endl
									 << CIniFile::Content(FileName)
									 << "++++++++++++++++++++++++++++++++++++++++" << endl;
								exitRequest = true;
								break;
							case 't':                                         // toggle show time this run
								showElapsedTime = !showElapsedTime;
								break;
							case 'T':                                         // toggle show time all runs
								showElapsedTimeTotal = !showElapsedTimeTotal;
								break;
							case '0':
								timePrecisionValue = 0;
								break;
							case '1':
								timePrecisionValue = 1;
								break;
							case '2':
								timePrecisionValue = 2;
								break;
							case '3':
								timePrecisionValue = 3;
								break;
							default:
								break;
							}
						}

						if (secondsDiff % 15 == 0 && !showThisTrial && !showAllTrials) {        // show current trials every 15 seconds
							infoLineStart();
							showThisTrial = true;
							showThisTrialCount = 0;
						}

						if (secondsDiff % 600 == 0) {                             // upsate the ini file every 600 seconds
							INI_Update(mutatePassword->get_pwdAsIndicies(), mutatePassword->get_pwdAsText());
							cout << endl
								<< "++++++++++++++++++++++++++++++++++++++++" << endl
								<< CIniFile::Content(FileName)
								<< "++++++++++++++++++++++++++++++++++++++++" << endl;
						}
					}
					if (showAllTrials) {
						if (pwdCount % triesPerLine == 0) {
							infoLineStart();
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
				pwdStr = mutatePassword->get_plainPassword();
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

string elapsedTime()
{
	string retStr = "";
	string tmpStr;
	double timeValue;
	if (showElapsedTime) {
		if (secondsDiff < 60 || timePrecisionValue == 0) {
			retStr = to_string(secondsDiff) + ":S ";
		}
		else if (secondsDiff < (60 *60)) {
			timeValue = (double)secondsDiff / 60;
			tmpStr = to_string(timeValue);
			retStr = tmpStr.substr(0, tmpStr.find(".") + timePrecisionValue + 1) + ":M ";
		}
		else if (secondsDiff < (60 * 60 * 24)) {
			timeValue = (double)secondsDiff / (long)(60 * 60);
			tmpStr = to_string(timeValue);
			retStr = tmpStr.substr(0, tmpStr.find(".") + timePrecisionValue + 1) + ":H ";
		}
		else {
			timeValue = (double)secondsDiff / (long)(60 * 60 * 24);
			tmpStr = to_string(timeValue);
			retStr = tmpStr.substr(0, tmpStr.find(".") + timePrecisionValue + 1) + ":D ";
		}
	}

	if (showElapsedTimeTotal) {
		long totalSecondsDiff = (long)pastTotalSeconds + (long)secondsDiff;
		if (totalSecondsDiff < 60 || timePrecisionValue == 0) {
			retStr = retStr + to_string(totalSecondsDiff) + ":S(t) ";
		}
		else if (totalSecondsDiff < (60 * 60)) {
			timeValue = (double)totalSecondsDiff / 60;
			tmpStr = to_string(timeValue);
			retStr = retStr + tmpStr.substr(0, tmpStr.find(".") + timePrecisionValue + 1) + ":M(t) ";
		}
		else if (totalSecondsDiff < (60 * 60 * 24)) {
			timeValue = (double)totalSecondsDiff / (long)(60 * 60);
			tmpStr = to_string(timeValue);
			retStr = retStr + tmpStr.substr(0, tmpStr.find(".") + timePrecisionValue + 1) + ":H(t) ";
		}
		else {
			timeValue = (double)totalSecondsDiff / (long)(60 * 60 * 24);
			tmpStr = to_string(timeValue);
			retStr = retStr + tmpStr.substr(0, tmpStr.find(".") + timePrecisionValue + 1) + ":D(t) ";
		}
	}
	return retStr;
}

void INI_Update(string pwdIndiciesStr, string pwdStr)
{
	char tmpStr[250];

	pwdCountStr = to_string(pwdCount);
	CIniFile::SetValue(Key_TrialCount, pwdCountStr, SectionName_CurrentState, FileName);

	CIniFile::SetValue(Key_TrialValue, pwdIndiciesStr, SectionName_CurrentState, FileName);
	CIniFile::SetRecordComments("# show plain TrialValue - " + pwdStr + "\n", Key_TrialValue, SectionName_CurrentState, FileName);

	triesPerSec = (double)(pwdCount - startValue) / secondsDiff;
	sprintf_s(tmpStr, sizeof(tmpStr), "%.2f", triesPerSec);
	CIniFile::SetValue(Key_TrialsPerSec, tmpStr, SectionName_Performance, FileName);

	triesPerSec = (double)pwdCount / (pastTotalSeconds + secondsDiff);
	sprintf_s(tmpStr, sizeof(tmpStr), "%.2f", triesPerSec);
	CIniFile::SetValue(Key_AvgTrialsPerSec, tmpStr, SectionName_Performance, FileName);

	CIniFile::SetValue(Key_TotalSeconds, to_string(pastTotalSeconds + secondsDiff), SectionName_Performance, FileName);
}

void infoLineStart(void)
{
	char   tmpStr[250];
	string timeStr = "";

	if (showElapsedTime || showElapsedTimeTotal) {
		timeStr = elapsedTime();
	}
	triesPerSec = (double)(pwdCount - startValue) / secondsDiff;
	sprintf_s(tmpStr, sizeof(tmpStr), "\r\n %s%lld, %5.2f:", timeStr.c_str(), pwdCount, triesPerSec);
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
