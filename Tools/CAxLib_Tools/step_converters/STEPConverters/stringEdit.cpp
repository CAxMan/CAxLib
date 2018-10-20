#include "stdafx.h"
#include "stringEdit.h"

//CLEAR LEADING AND TRAILING SPACES
void removeLeadingAndTrailingWhiteSpace(std::string& str){
	while (!str.empty() && isspace(*str.begin()))
		str.erase(str.begin());
	while (!str.empty() && isspace(*str.rbegin()))
		str.erase(str.length() - 1);
}
//REMOVE UNWANTED CHARS
void cleanString(std::string& str) {
	char charsToRemove[] = "(,);*";
	for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
		str.erase(std::remove(str.begin(), str.end(), charsToRemove[i]), str.end());
	}

}


//SOME EXPRESS ENTITIES AND TYPES HAVE NAMES THAT CLASHES WITH CPP
//THIS FUNCTION ADDS _ap209 at the end of the name
void correctNameClashWithCPP(std::string& str){
	if (str == "class" || str == "union"){ str = str + "_ap209"; }
}