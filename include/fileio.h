#pragma once

#ifndef FILEIO_H_
#define FILEIO_H_

#include "pch.h"
class FileIO
{
public:
    // Function Declarations
    void fileSafeIn(std::string filename, std::ifstream &fin);
    void fileSafeOut(std::string filename, std::ofstream &fout);
    void checkRinexVersionType(double &version, std::string &type_file, int &type, std::ifstream &fin);
    void logger(std::string output_filename, std::string input_filename, std::ofstream &fout);
};

#endif /* FILEIO_H_ */
