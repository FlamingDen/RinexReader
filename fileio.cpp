#include "fileio.h"

using namespace std;

// File Opener --> Initiates File pointer safely
void FileIO::fileSafeIn(string filename, ifstream& fin) {
    ifstream inputfile(filename);
    if (!inputfile.is_open()) {
        perror("Error while opening file");
    }
    // Check is file can be read
    if (inputfile.bad()) {
        perror("Error while reading file");
    }
    else fin.open(filename);
}

// A function to generate a LOG File for errors in Observation File
void FileIO::logger(string output_filename, string input_filename, ofstream& fout) {
    // Creating NEW LOG file
    fout.open(output_filename); fout.close();
    fout.open(output_filename, ios::app);
    // Preparing a format for LOG file
    fout << "-----------------------------------------------------------------------------------------\n";
    fout << "-----------------------------------------------------------------------------------------\n";
    fout << "THIS IS A LOG FILE FOR THE RINEX FILE READER.\n";
    fout << "THIS FILE CONTAINS RECORDS OF ANY UNEXPECTED BREAKS OR MISSING INFORMATION IN DATA FILES.\n";
    fout << "-----------------------------------------------------------------------------------------\n";
    fout << "-----------------------------------------------------------------------------------------\n";
    fout << "OBSERVATION FILE NAME: " << input_filename << "\n" << "\n";
    fout << std::left
         << std::setw(30) << "EPOCH INFORMATION"
         << std::setw(20) << "PRN"
         << std::setw(20) << "MISSING OBSERVATION" << "\n";
    fout << "-----------------------------------------------------------------------------------------\n";
}

// A function to initialize the updated output datafile
void FileIO::fileSafeOut(string output_filename, ofstream& fout) {
    // Creating NEW LOG file
    fout.open(output_filename); fout.close();
    fout.open(output_filename, ios::app);
}

// Check Rinex File Version
void FileIO::checkRinexVersionType(double &rinex_version, int &rinex_type, std::ifstream &fin)
{
    const std::string sTokenVER = "RINEX VERSION / TYPE";
    std::string line; int nLines = 0;
    while (!fin.eof()) {
        nLines++;
        line.clear();
        getline(fin, line, '\n');
        size_t found_VER = line.find(sTokenVER);
        // RINEX Version
        if (found_VER != std::string::npos) {
            std::istringstream iss(line);
            std::vector<std::string> words{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };
            // Rinex Version
            if ((std::stod(words[0])) >= 3) {
                rinex_version = std::stod(words[0]);
            }
            else if ((std::stod(words[0])) >= 2) {
                rinex_version = std::stod(words[0]);
            }
            else {
                rinex_version = NULL;
            }
            // Rinex Type
            std::string type = words[words.size() - 6].substr(0, 1);
            rinex_type = 0;
            if (type == "G") {
                rinex_type = 1;
            }
            if (type == "R") {
                rinex_type = 3;
            }
            if (type == "E") {
                rinex_type = 2;
            }
            if (type == "C") {
                rinex_type = 4;
            }
            if (type == "M") {
                rinex_type = 5;
            }
            words.clear();

            fin.clear();
            fin.seekg(0,fin.beg);
            break;
        }
        // Fail safe
        if (nLines > 1) {
            // Rinex version usually provided in first line of file
            cout << "RINEX VERSION TYPE NOT FOUND IN FIRST LINE OF FILE! \n";
            break;
        }
    }
}
