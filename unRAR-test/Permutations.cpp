#include "Permutations.h"


Permutations::Permutations()
{
	testPwdLen = 0;
	useLAlpha  = false;
	useUAlpha  = false;
	useNAlpha  = false;
	useNumeric = false;
	useNone    = true;

	for (int x = 0; x < MAX_PWD_LEN; x++) {
		testPassword[x] = 0;
		charIndicies[x] = -1;
	}
	testPassword[MAX_PWD_LEN] = 0;
}

Permutations::~Permutations()
{
}

int Permutations::setNext()
{
	if (useNone) return PERMUTE_NO_OPTION;

	int localCharIndicies[MAX_PWD_LEN];
	if (permuteStatus == PERMUTE_SUCCESS) {
		for (int x = 0; x < MAX_PWD_LEN; x++) {
			localCharIndicies[x] = charIndicies[x];
		}
	}
	else {
		return PERMUTE_DONE;
	}

	bool done = false;
	int pwdCharIndex = 0;
	int charLookupIndex;
	int retVal = PERMUTE_SUCCESS;

	while (!done)
	{
		charLookupIndex = localCharIndicies[pwdCharIndex];
		if (charLookupIndex == -1) {
			localCharIndicies[pwdCharIndex] = getLowestIndex();
			charLookupIndex = getLowestIndex();
			done = true;
		}

		if (useLAlpha && !done) {
			if (charLookupIndex >= RANGE_LA_LOWER && charLookupIndex < RANGE_LA_UPPER) {
				localCharIndicies[pwdCharIndex]++;
				done = true;
			}
			else {
				if (charLookupIndex == RANGE_LA_UPPER) {
					if (useUAlpha) {
						localCharIndicies[pwdCharIndex] = RANGE_UA_LOWER;
						done = true;
						continue;
					}
					else if (useNumeric) {
						localCharIndicies[pwdCharIndex] = RANGE_NUM_LOWER;
						done = true;
						continue;
					}
					else if (useNAlpha) {
						localCharIndicies[pwdCharIndex] = RANGE_NA_LOWER;
						done = true;
						continue;
					}
					else {
						pwdCharIndex++;
						if (pwdCharIndex < MAX_PWD_LEN) {
							localCharIndicies[pwdCharIndex - 1] = getLowestIndex();
						}
						else {
							done = true;
							retVal = PERMUTE_DONE;
						}
					}
				}
			}
		}

		if (useUAlpha && !done) {
			if (charLookupIndex >= RANGE_UA_LOWER && charLookupIndex < RANGE_UA_UPPER) {
				localCharIndicies[pwdCharIndex]++;
				done = true;
			}
			else {
				if (charLookupIndex == RANGE_UA_UPPER) {
					if (useNumeric) {
						localCharIndicies[pwdCharIndex] = RANGE_NUM_LOWER;
						done = true;
						continue;
					}
					else if (useNAlpha) {
						localCharIndicies[pwdCharIndex] = RANGE_NA_LOWER;
						done = true;
						continue;
					}
					else {
						pwdCharIndex++;
						if (pwdCharIndex < MAX_PWD_LEN) {
							localCharIndicies[pwdCharIndex - 1] = getLowestIndex();
						}
						else {
							done = true;
							retVal = PERMUTE_DONE;
						}
					}
				}
			}
		}

		if (useNumeric && !done) {
			if (charLookupIndex >= RANGE_NUM_LOWER && charLookupIndex < RANGE_NUM_UPPER) {
				localCharIndicies[pwdCharIndex]++;
				done = true;
			}
			else {
				if (charLookupIndex == RANGE_NUM_UPPER) {
					if (useNAlpha) {
						localCharIndicies[pwdCharIndex] = RANGE_NA_LOWER;
						done = true;
						continue;
					}
					else {
						pwdCharIndex++;
						if (pwdCharIndex < MAX_PWD_LEN) {
							localCharIndicies[pwdCharIndex - 1] = getLowestIndex();
						}
						else {
							done = true;
							retVal = PERMUTE_DONE;
						}
					}
				}
			}
		}

		if (useNAlpha && !done) {
			if (charLookupIndex >= RANGE_NA_LOWER && charLookupIndex < RANGE_NA_UPPER) {
				localCharIndicies[pwdCharIndex]++;
				done = true;
			}
			else {
				if (charLookupIndex == RANGE_NA_UPPER) {
					pwdCharIndex++;
					if (pwdCharIndex < MAX_PWD_LEN) {
						localCharIndicies[pwdCharIndex - 1] = getLowestIndex();
					}
					else {
						done = true;
						retVal = PERMUTE_DONE;
					}
				}
			}
		}
	}

	if (retVal == PERMUTE_SUCCESS) {
		for (int x = 0; x < MAX_PWD_LEN; x++) {
			charIndicies[x] = localCharIndicies[x];
		}

		if (pwdCharIndex + 1 > testPwdLen)
			testPwdLen = pwdCharIndex + 1;
	}
	else {
		permuteStatus = retVal;
	}

	return retVal;
}

string Permutations::get_pwdAsIndicies() {
	string pwdStr = to_string(testPwdLen);
	for (int x = 0; x < testPwdLen; x++) {
		pwdStr += "," + to_string(charIndicies[x]);
	}

	return pwdStr;
}

int Permutations::getLowestIndex()
{
	if (useLAlpha)  return RANGE_LA_LOWER;
	if (useUAlpha)  return RANGE_UA_LOWER;
	if (useNumeric) return RANGE_NUM_LOWER;
	if (useNAlpha)  return RANGE_NA_LOWER;
	return -1;
}

char* Permutations::get_testPassword()
{
	int x, y;
	int charLookupIndex = 0;
	for (y = 0, x = testPwdLen-1; x >= 0; x--, y++) {
		charLookupIndex = charIndicies[x];
		if (charLookupIndex >= RANGE_LA_LOWER && charLookupIndex <= RANGE_LA_UPPER) {
			testPassword[y] = lowerAlpha[charLookupIndex - RANGE_LA_LOWER];
		}
		else if (charLookupIndex >= RANGE_UA_LOWER && charLookupIndex <= RANGE_UA_UPPER) {
			testPassword[y] = upperAlpha[charLookupIndex - RANGE_UA_LOWER];
		}
		else if (charLookupIndex >= RANGE_NUM_LOWER && charLookupIndex <= RANGE_NUM_UPPER) {
			testPassword[y] = numeric[charLookupIndex - RANGE_NUM_LOWER];
		}
		else if (charLookupIndex >= RANGE_NA_LOWER && charLookupIndex <= RANGE_NA_UPPER) {
			testPassword[y] = nonAlpha[charLookupIndex - RANGE_NA_LOWER];
		}
		else
			return nullptr;
	}
	testPassword[testPwdLen] = 0;
	return testPassword;
}

void Permutations::set_useLAlpha(bool newValue)
{
	useLAlpha = newValue;
	useNone = !(useLAlpha || useUAlpha || useNumeric || useNAlpha);
}

void Permutations::set_useUAlpha(bool newValue)
{
	useUAlpha = newValue;
	useNone = !(useLAlpha || useUAlpha || useNumeric || useNAlpha);
}

void Permutations::set_useNAlpha(bool newValue)
{
	useNAlpha = newValue;
	useNone = !(useLAlpha || useUAlpha || useNumeric || useNAlpha);
}

void Permutations::set_useNumeric(bool newValue)
{
	useNumeric = newValue;
	useNone = !(useLAlpha || useUAlpha || useNumeric || useNAlpha);
}
