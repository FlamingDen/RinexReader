#pragma once

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include "pch.h"

// Functions
std::string replaceChars(std::string str, char ch1, char ch2);
void eraseSubStr(std::string &mainStr, const std::string &toErase);
std::string HHMMSS(double hours, double mins, double secs);

#endif /* STRINGUTILS_H_ */
