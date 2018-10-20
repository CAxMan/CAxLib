#include "stdafx.h"
#include <string>
#include <algorithm>

#define create(className) new(_m)className(_m)
#define createSet(className, sdaiTYPE, n) new(_ma)Set<className*>(_m, sdaiTYPE, n);

void removeLeadingAndTrailingWhiteSpace(std::string& str);
void cleanString(std::string& str);
void correctNameClashWithCPP(std::string& str);


