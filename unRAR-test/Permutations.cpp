#include "Permutations.h"


Permutations::Permutations()
{
	testPwdLen = 0;
	useLAlpha = false;
	useUAlpha = false;
	useNAlpha = false;
	useNumeric = false;
	useNone = true;

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

	bool done = false;
	int pwdCharIndex = 0;
	int charLookupIndex = charIndicies[pwdCharIndex];
	int retVal = PERMUTE_SUCCESS;

	while (!done && pwdCharIndex < MAX_PWD_LEN)
	{
		if (useLAlpha) {
			if (charLookupIndex >= 0 && charLookupIndex < )

		}
		else {

		}

		if (useUAlpha) {

		}
		else {

		}

		if (useNumeric) {

		}
		else {

		}

		if (useNAlpha) {

		}
		else {

		}
	}

	return retVal;
}

char* Permutations::get_testPassword()
{
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
