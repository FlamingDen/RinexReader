#pragma once

#include "pch.h"

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

// Functions
std::string replaceChars(std::string str, char ch1, char ch2);
void eraseSubStr(std::string & mainStr, const std::string & toErase);
std::string HHMMSS(double hours, double mins, double secs);

#endif /* STRINGUTILS_H_ */
