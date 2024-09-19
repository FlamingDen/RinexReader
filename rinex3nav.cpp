#include "rinex3nav.h"
#include <regex>

using namespace std;

Rinex3Nav::Rinex3Nav() {}

Rinex3Nav::~Rinex3Nav() {}

// A function to help with organizing GPS header
// Works for the alpha/beta ionospheric constants and time correction
std::vector<double> headerHelperGPS(string line) {
    line = line.substr(5, 55);
    string word;
    vector<double> data;
    for (unsigned i = 0; i < line.length(); i += 12) {
        word = line.substr(i, 12);
        if (word.find_first_not_of(' ') == std::string::npos) { continue; }
        data.push_back(stod(replaceChars(word, 'D', 'e')));
        word.clear();
    }
    return data;
}

// Organizes Epoch Time Information into Vector
vector<double> rinex3EpochTimeOrganizer(string line) {
    vector<double> epochRecord;
    line = line.substr(3, 20);
    // Splitting words in the line
    istringstream iss(line);
    vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
    for (string s : words) {
        epochRecord.push_back(stod(s));
    }
    // Return: Vector containing the Epoch Info
    return epochRecord;
}

// Function to split and organize navigation parameters
vector<std::optional<double>> rinex3NavDataSplitter(string line) {
    // Split line every 19 spaces as allocated for parameters
    vector<std::optional<double>> data;
    //vector<double> data;
    string word;
    for (unsigned i = 0; i < line.length(); i += 19) {
        word = line.substr(i, 19);
        if (word.find_first_not_of(' ') == std::string::npos) {
            data.push_back(std::nullopt);
            continue;
        }
        if (word.find('D') != std::string::npos) {
            data.push_back(stod(replaceChars(word, 'D', 'e')));
            continue;
        }
        if (word.find('E') != std::string::npos) {
            data.push_back(stod(replaceChars(word, 'E', 'e')));
            continue;
        }
        if (word.find('e') != std::string::npos) {
            data.push_back(stod(word));
            continue;
        }
        else {
            continue;
        }
    }
    word.clear();
    return data;
}


//=======================================================================================
template<typename T>
int epochMatcherHelper(double obsTime, std::vector<T> NAV){
    // Initialize time difference variable using arbitrary large number
    double diff = 1000000; int index = 0;
    for (unsigned i = 0; i < NAV.size(); i++) {
        double new_diff = fabs(obsTime - NAV.at(i).gpsTime);
        if (new_diff < diff) {
            diff = new_diff; // update difference
            index = i; // save index
        }
    }
    // Index of most appropriate Nav vector
    return index;
}

// Epoch Time Matcher, returns index of most appropriate data from Navigation vector
int Rinex3Nav::EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGPS> NAV) {
    return epochMatcherHelper(obsTime,NAV);
}

// Epoch Time Matcher, returns index of most appropriate data from Navigation vector
int Rinex3Nav::EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGLO> NAV) {
    return epochMatcherHelper(obsTime,NAV);
}

// Epoch Time Matcher, returns index of most appropriate data from Navigation vector
int Rinex3Nav::EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGAL> NAV) {
    return epochMatcherHelper(obsTime,NAV);
}

// Epoch Time Matcher, returns index of most appropriate data from Navigation vector
int Rinex3Nav::EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataBEI> NAV) {
    return epochMatcherHelper(obsTime,NAV);
}
//=======================================================================================

void Rinex3Nav::readHead(std::ifstream& infile, SatelliteSystem sys){
    //common tokens
    const string sTokenPGM = "PGM / RUN BY / DATE";
    const string sTokenCOM = "COMMENT";
    const string sTokenIONO = "IONOSPHERIC CORR";
    const string sTokenCORR = "TIME SYSTEM CORR";
    const string sTokenLEAP = "LEAP SECONDS";
    const string sTokenEND = "END OF HEADER";

    string line;

    switch (sys) {
    case SatelliteSystem::GPS:{
        //iono
        const string sTokenGPSA = "GPSA";//= GPS alpha0 - alpha3
        const string sTokenGPSB = "GPSB";//GPS beta0 - beta3
        //time sys core
        const string sTokenGPUT = "GPUT";//= GPS - UTC (a0, a1)

        while (!infile.eof()) {
            line.clear();
            // Temporarily store line from input file
            getline(infile, line, '\n');
            // Looking for keywords in Header Part...
            size_t found_PGM = line.find(sTokenPGM);
            size_t found_COM = line.find(sTokenCOM);
            size_t found_IONO = line.find(sTokenIONO);
            size_t found_CORR = line.find(sTokenCORR);
            size_t found_LEAP = line.find(sTokenLEAP);
            size_t found_END = line.find(sTokenEND);

            if(found_PGM != string::npos){
                _headerGPS.pgm = getPMGHeader(line);
                continue;
            }
            // Finding Comments, meaning skip!
            if (found_COM != string::npos){
                continue;
            }
            // Finding Ionophseric Constants as per new format
            if (found_IONO != string::npos) {
                size_t found_GPSA = line.find(sTokenGPSA);
                size_t found_GPSB = line.find(sTokenGPSB);
                if (found_GPSA != string::npos) {
                    _headerGPS.ialpha = headerHelperGPS(line);
                }
                if (found_GPSB != string::npos) {
                    _headerGPS.ibeta = headerHelperGPS(line);
                }
                continue;
            }
            // Finding GPS to UTC Time Correction
            if (found_CORR != string::npos) {
                size_t found_GPUT = line.find(sTokenGPUT);
                if (found_GPUT != string::npos)
                    _headerGPS.GPUT = headerHelperGPS(line);
                continue;
            }
            if (found_LEAP != string::npos) {
                line = line.substr(0, 7);
                _headerGPS.leapSec = stod(line);
                continue;
            }
            // Finding End of Header Info
            if (found_END != string::npos) {
                break;
            }
        }
        break;
    }
    case SatelliteSystem::Galileo:{
        const string sTokenGAL = "GAL";//Galileo ai0 - ai2s
        const string sTokenGAUT = "GAUT";//= GAL - UTC ( a0, a1)

        while (!infile.eof()) {
            line.clear();

            getline(infile, line, '\n');

            size_t found_PGM = line.find(sTokenPGM);
            size_t found_COM = line.find(sTokenCOM);
            size_t found_IONO = line.find(sTokenIONO);
            size_t found_CORR = line.find(sTokenCORR);
            size_t found_LEAP = line.find(sTokenLEAP);
            size_t found_END = line.find(sTokenEND);

            if(found_PGM != string::npos){
                _headerGAL.pgm = getPMGHeader(line);
            }
            // Finding Comments, meaning skip!
            if (found_COM != string::npos) {
                continue;
            }
            // Finding Ionophseric Constants as per new format
            if (found_IONO != string::npos) {
                size_t found_GAL = line.find(sTokenGAL);
                if (found_GAL != string::npos) {
                    _headerGAL.gal = headerHelperGPS(line);
                }
                continue;
            }
            // Finding Time Correction
            if (found_CORR != string::npos) {
                size_t found_GAUT = line.find(sTokenGAUT);
                if (found_GAUT != string::npos)
                    _headerGAL.GAUT = headerHelperGPS(line);
                continue;
            }
            // Finding Leap Second
            if (found_LEAP != string::npos) {
                line = line.substr(0, 7);
                _headerGAL.leapSec = stod(line);
                continue;
            }
            // Finding End of Header Info
            if (found_END != string::npos) {
                break;
            }
        }

        break;
    }
    case SatelliteSystem::Glonass:{
        const string sTokenGLUT = "GLUT";//= GLO - UTC (a0= -TauC, a1=zero)
        const string sTokenGLGP = "GLGP";//= GLO - GPS (a0=TauGPS, a1=zero)

        while (!infile.eof()) {
            line.clear();

            getline(infile, line, '\n');

            size_t found_PGM = line.find(sTokenPGM);
            size_t found_COM = line.find(sTokenCOM);
            size_t found_CORR = line.find(sTokenCORR);
            size_t found_LEAP = line.find(sTokenLEAP);
            size_t found_END = line.find(sTokenEND);

            if(found_PGM != string::npos){
                _headerGLO.pgm = getPMGHeader(line);
            }
            // Finding Comments, meaning skip!
            if (found_COM != string::npos) {
                continue;
            }
            // Finding Time Correction
            if (found_CORR != string::npos) {
                // line = line.substr(0, 60);
                // // Splitting words in the line
                // istringstream iss(line);
                // vector<string> dataS{ istream_iterator<string>{iss}, istream_iterator<string>{} };
                // for (string &s : dataS) { _headerGLO.TimeCorr.push_back(stod(s)); }
                // dataS.clear();
                // continue;
                size_t found_GLUT = line.find(sTokenGLUT);
                if (found_GLUT != string::npos)
                    _headerGLO.GLUT = headerHelperGPS(line);
                continue;
            }
            // Finding Leap Second
            if (found_LEAP != string::npos) {
                line = line.substr(0, 7);
                _headerGLO.leapSec = stod(line);
                continue;
            }
            // Finding End of Header Info
            if (found_END != string::npos) {
                break;
            }
        }
        break;
    }
    case SatelliteSystem::BeiDou:{
        //iono
        const string sTokenBDSA = "BDSA";//BDS alpha0 - alpha3
        const string sTokenBDSB = "BDSB";//= BDS beta0 - beta3
        //time sys cores
        const string sTokenBDUT = "GDUT";// =BDS - UTC (a0=A0UTC, a1=A1UTC )

        while (!infile.eof()) {
            line.clear();

            getline(infile, line, '\n');

            size_t found_PGM = line.find(sTokenPGM);
            size_t found_COM = line.find(sTokenCOM);
            size_t found_IONO = line.find(sTokenIONO);
            size_t found_CORR = line.find(sTokenCORR);
            size_t found_LEAP = line.find(sTokenLEAP);
            size_t found_END = line.find(sTokenEND);

            if(found_PGM != string::npos){
                _headerBEI.pgm = getPMGHeader(line);
            }
            // Finding Comments, meaning skip!
            if (found_COM != string::npos){
                continue;
            }
            // Finding Ionophseric Constants as per new format
            if (found_IONO != string::npos) {
                size_t found_DBSA = line.find(sTokenBDSA);
                size_t found_DBSB = line.find(sTokenBDSB);
                if (found_DBSA != string::npos) {
                    _headerBEI.ialpha = headerHelperGPS(line);
                }
                if (found_DBSB != string::npos) {
                    _headerBEI.ibeta = headerHelperGPS(line);
                }
                continue;
            }
            // Finding GPS to UTC Time Correction
            if (found_CORR != string::npos) {
                size_t found_BDUT = line.find(sTokenBDUT);
                if (found_BDUT != string::npos)
                    _headerBEI.BDUT = headerHelperGPS(line);
                continue;
            }
            if (found_LEAP != string::npos) {
                line = line.substr(0, 7);
                _headerBEI.leapSec = stod(line);
                continue;
            }
            // Finding End of Header Info
            if (found_END != string::npos) {
                break;
            }
        }

        break;
    }
    case SatelliteSystem::Mixed:{
        //iono
        const string sTokenGPSA = "GPSA";//= GPS alpha0 - alpha3
        const string sTokenGPSB = "GPSB";//GPS beta0 - beta3
        //time sys core
        const string sTokenGPUT = "GPUT";//= GPS - UTC (a0, a1)

        //iono
        const string sTokenGAL = "GAL";//Galileo ai0 - ai2s
        //time sys core
        const string sTokenGAUT = "GAUT";//= GAL - UTC ( a0, a1)

        //time sys core
        const string sTokenGLUT = "GLUT";//= GLO - UTC (a0= -TauC, a1=zero)
        const string sTokenGLGP = "GLGP";//= GLO - GPS (a0=TauGPS, a1=zero)

        //iono
        const string sTokenBDSA = "BDSA";//BDS alpha0 - alpha3
        const string sTokenBDSB = "BDSB";//= BDS beta0 - beta3
        //time sys cores
        const string sTokenBDUT = "GDUT";// =BDS - UTC (a0=A0UTC, a1=A1UTC )
        while (!infile.eof()) {
            line.clear();

            getline(infile, line, '\n');

            size_t found_PGM = line.find(sTokenPGM);
            size_t found_COM = line.find(sTokenCOM);
            size_t found_IONO = line.find(sTokenIONO);
            size_t found_CORR = line.find(sTokenCORR);
            size_t found_LEAP = line.find(sTokenLEAP);
            size_t found_END = line.find(sTokenEND);

            if(found_PGM != string::npos){
                _headerBEI.pgm = getPMGHeader(line);
                _headerGPS.pgm = getPMGHeader(line);
                _headerGAL.pgm = getPMGHeader(line);
                _headerGLO.pgm = getPMGHeader(line);
            }
            // Finding Comments, meaning skip!
            if (found_COM != string::npos){
                continue;
            }
            // Finding Ionophseric Constants as per new format
            if (found_IONO != string::npos) {
                size_t found_DBSA = line.find(sTokenBDSA);
                size_t found_DBSB = line.find(sTokenBDSB);
                size_t found_GAL = line.find(sTokenGAL);
                size_t found_GPSA = line.find(sTokenGPSA);
                size_t found_GPSB = line.find(sTokenGPSB);

                if (found_GPSA != string::npos) {
                    _headerGPS.ialpha = headerHelperGPS(line);
                    continue;
                }
                if (found_GPSB != string::npos) {
                    _headerGPS.ibeta = headerHelperGPS(line);
                    continue;
                }
                if (found_GAL != string::npos) {
                    _headerGAL.gal = headerHelperGPS(line);
                    continue;
                }
                if (found_DBSA != string::npos) {
                    _headerBEI.ialpha = headerHelperGPS(line);
                    continue;
                }
                if (found_DBSB != string::npos) {
                    _headerBEI.ibeta = headerHelperGPS(line);
                    continue;
                }
            }
            // Finding GPS to UTC Time Correction
            if (found_CORR != string::npos) {
                size_t found_BDUT = line.find(sTokenBDUT);
                size_t found_GLUT = line.find(sTokenGLUT);
                size_t found_GAUT = line.find(sTokenGAUT);
                size_t found_GPUT = line.find(sTokenGPUT);
                if (found_GPUT != string::npos)
                    _headerGPS.GPUT = headerHelperGPS(line);
                if (found_GAUT != string::npos)
                    _headerGAL.GAUT = headerHelperGPS(line);
                if (found_BDUT != string::npos)
                    _headerBEI.BDUT = headerHelperGPS(line);
                if (found_GLUT != string::npos)
                    _headerGLO.GLUT = headerHelperGPS(line);
                continue;
            }
            if (found_LEAP != string::npos) {
                line = line.substr(0, 7);
                _headerBEI.leapSec = stod(line);
                _headerGPS.leapSec = stod(line);
                _headerGLO.leapSec = stod(line);
                _headerGAL.leapSec = stod(line);
                continue;
            }
            // Finding End of Header Info
            if (found_END != string::npos) {
                break;
            }
        }
        break;
    }
    default: break;
    }
}


std::vector<string> Rinex3Nav::getPMGHeader(std::string line)
{
    line = line.substr(0, 60);
    string word;
    vector<string> data;
    for (unsigned i = 0; i < line.length(); i += 20) {
        word = line.substr(i, 20);
        if (word.find_first_not_of(' ') == std::string::npos)
            continue;
        word = regex_replace(word,regex{R"(^\s+|\s+$)"}, "");
        data.push_back(word);
        word.clear();
    }
    return data;
}

//==================================GPS==================================================
// Navigation Body Organizer for GPS Navigation File
Rinex3Nav::DataGPS epochNavOrganizerGPS(vector<string> block) {
    string sys = block[0].substr(0, 1);
    int prn = stoi(block[0].substr(1, 2));
    vector<double> epochInfo = rinex3EpochTimeOrganizer(block[0]);
    string line = block[0].substr(23, block[0].length());
    for (unsigned int i = 1; i < block.size(); i++) {
        if (block[i].at(0) !=  '-' && block[i].at(0) != ' '){
            line += ' ' + block[i];
            continue;
        }
        line += block[i];
    }
    vector<std::optional<double>> parameters = rinex3NavDataSplitter(line);
    // Storing Values into GPS Data Structure
    Rinex3Nav::DataGPS GPS;
    GPS.PRN = prn;
    GPS.epochInfo = epochInfo;
    GPS.gpsTime = gpsTime(epochInfo);
    GPS.clockBias = parameters.at(0).value();
    GPS.clockDrift = parameters.at(1).value();
    GPS.clockDriftRate = parameters.at(2).value();

    GPS.IODE = parameters.at(3).value();
    GPS.Crs = parameters.at(4).value();
    GPS.Delta_n = parameters.at(5).value();
    GPS.M0 = parameters.at(6).value();

    GPS.Cuc = parameters.at(7).value();
    GPS.Eccentricity = parameters.at(8).value();
    GPS.Cus = parameters.at(9).value();
    GPS.Sqrt_a = parameters.at(10).value();

    GPS.TOE = parameters.at(11).value();
    GPS.Cic = parameters.at(12).value();
    GPS.OMEGA0 = parameters.at(13).value();
    GPS.Cis = parameters.at(14).value();

    GPS.i0 = parameters.at(15).value();
    GPS.Crc = parameters.at(16).value();
    GPS.omega = parameters.at(17).value();
    GPS.OMEGA_DOT = parameters.at(18).value();

    GPS.IDOT = parameters.at(19).value();
    GPS.L2_codes_channel = parameters.at(20).value();
    GPS.GPS_week = parameters.at(21).value();
    GPS.L2_P_data_flag = parameters.at(22).value();

    GPS.svAccuracy = parameters.at(23).value();
    GPS.svHealth = parameters.at(24).value();
    GPS.TGD = parameters.at(25).value();
    GPS.IODC = parameters.at(26).value();

    GPS.transmission_time = parameters.at(27).value();
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if(parameters.size() > 28 && parameters.at(28).has_value())
        GPS.fit_interval = parameters.at(28).value();
    if(parameters.size() > 29 && parameters.at(29).has_value())
        GPS.spare1 = parameters.at(29).value();
    if(parameters.size() > 30 && parameters.at(30).has_value())
        GPS.spare2 = parameters.at(30).value();

    GPS.isAvailable = true;
    return GPS;
}

// Reader for GPS navigation file
void Rinex3Nav::readGPS(std::ifstream& infile) {
    // A vector to hold block of sentences
    vector<string> block;
    // To hold contents of a line from input file
    string line;
    int nlines = 0;

    readHead(infile, SatelliteSystem::GPS);

    // Create GPS Navigation data holder
    map<int, vector<Rinex3Nav::DataGPS>> mapGPS;

    // Reading Navigation Data Body
    while (!infile.eof()) {
        line.clear();
        // Temporarily store line from input file
        getline(infile, line, '\n'); nlines++;
        if (line.find_first_not_of(' ') == std::string::npos) { continue; }
        // Adjust line spaces before adding to block
        if (nlines != 1) {
            line = line.substr(4, line.length());
        }
        block.push_back(line);
        // New block of navigation message
        if (nlines == 8) {
            // Now we must process the block of lines
            DataGPS GPS = epochNavOrganizerGPS(block);
            block.clear(); nlines = 0;
            // Add organized data to data holder
            // Save to Map: if PRN exists in map, then add NavInfo to vector of structs
            // Else add new PRN as key and GPS data structure as Value
            if (mapGPS.find(GPS.PRN) == mapGPS.end()) {
                // not found, therefore insert PRN and corresponding value
                vector<DataGPS> mapNavVector; mapNavVector.push_back(GPS);
                mapGPS.insert(pair<int, vector<DataGPS>>(GPS.PRN, mapNavVector));
            }
            else {
                // found, therefore add to existing PRN
                mapGPS[GPS.PRN].push_back(GPS);
            }
        }
    }
    // Update the attribute of Navigation Object
    _navGPS = mapGPS;
}
//=======================================================================================


//=================================GLONASS===============================================
// Navigation Body Organizer for GLONASS Navigation File
Rinex3Nav::DataGLO epochNavOrganizerGLO(vector<string> block) {
    string sys = block[0].substr(0, 1);
    int prn = stoi(block[0].substr(1, 2));
    vector<double> epochInfo = rinex3EpochTimeOrganizer(block[0]);
    string line = block[0].substr(23, block[0].length());
    for (unsigned int i = 1; i < block.size(); i++) {
        if (block[i].at(0) !=  '-' && block[i].at(0) != ' '){
            line += ' ' + block[i];
            continue;
        }
        line += block[i];
    }
    vector<std::optional<double>> parameters = rinex3NavDataSplitter(line);
    // Storing Values into GPS Data Structure
    Rinex3Nav::DataGLO GLO;
    GLO.PRN = prn;
    GLO.epochInfo = epochInfo;
    GLO.gpsTime = gpsTime(epochInfo);
    GLO.clockBias = parameters.at(0).value();
    GLO.relFreqBias = parameters.at(1).value();
    GLO.messageFrameTime = parameters.at(2).value();

    GLO.satPosX = parameters.at(3).value();
    GLO.satVelX = parameters.at(4).value();
    GLO.satAccX = parameters.at(5).value();
    GLO.satHealth = parameters.at(6).value();

    GLO.satPosY = parameters.at(7).value();
    GLO.satVelY = parameters.at(8).value();
    GLO.satAccY = parameters.at(9).value();
    GLO.freqNum = parameters.at(10).value();

    GLO.satPosZ = parameters.at(11).value();
    GLO.satVelZ = parameters.at(12).value();
    GLO.satAccZ = parameters.at(13).value();
    GLO.infoAge = parameters.at(14).value();

    GLO.isAvailable = true;
    return GLO;
}

// Reader for Glonass navigation file
// Parameters are in ECEF Greenwich coordinate system PZ - 90
void Rinex3Nav::readGLO(std::ifstream& infile) {

    vector<string> block;

    string line;
    int nlines = 0;

    readHead(infile, SatelliteSystem::Glonass);

    // Create GLO Navigation data holder
    map<int, vector<Rinex3Nav::DataGLO>> mapGLO;

    // Reading Navigation Data Body
    while (!infile.eof()) {
        line.clear();
        // Temporarily store line from input file
        getline(infile, line, '\n'); nlines++;
        if (line.find_first_not_of(' ') == std::string::npos) { continue; }
        // Adjust line spaces before adding to block
        if (nlines != 1) {
            line = line.substr(4, line.length());
        }
        block.push_back(line);
        // New block of navigation message
        if (nlines == 4) {
            // Now we must process the block of lines
            Rinex3Nav::DataGLO GLO = epochNavOrganizerGLO(block);
            block.clear(); nlines = 0;
            // Add organized data to data holder
            // Save to Map: if PRN exists in map, then add NavInfo to vector of structs
            // Else add new PRN as key and GLO data structure as Value
            if (mapGLO.find(GLO.PRN) == mapGLO.end()) {
                // not found, therefore insert PRN and corresponding value
                vector<DataGLO> mapNavVector; mapNavVector.push_back(GLO);
                mapGLO.insert(pair<int, vector<DataGLO>>(GLO.PRN, mapNavVector));
            }
            else {
                // found, therefore add to existing PRN
                mapGLO[GLO.PRN].push_back(GLO);
            }
        }
    }
    // Update the attribute of Navigation Object
    _navGLO = mapGLO;
}
//=======================================================================================


//=================================GALILEO===============================================
// Navigation Body Organizer for GAL Navigation File
Rinex3Nav::DataGAL epochNavOrganizerGAL(vector<string> block) {
    string sys = block[0].substr(0, 1);
    int prn = stoi(block[0].substr(1, 2));
    vector<double> epochInfo = rinex3EpochTimeOrganizer(block[0]);
    string line = block[0].substr(23, block[0].length());
    for (unsigned int i = 1; i < block.size(); i++) {
        if (block[i].at(0) !=  '-' && block[i].at(0) != ' '){
            line += ' ' + block[i];
            continue;
        }
        line += block[i];
    }
    vector<std::optional<double>> parameters = rinex3NavDataSplitter(line);
    // Storing Values into GAL Data Structure
    Rinex3Nav::DataGAL GAL;
    GAL.PRN = prn;
    GAL.epochInfo = epochInfo;
    GAL.gpsTime = gpsTime(epochInfo);
    GAL.clockBias = parameters.at(0).value();
    GAL.clockDrift = parameters.at(1).value();
    GAL.clockDriftRate = parameters.at(2).value();

    GAL.IOD = parameters.at(3).value();
    GAL.Crs = parameters.at(4).value();
    GAL.Delta_n = parameters.at(5).value();
    GAL.M0 = parameters.at(6).value();

    GAL.Cuc = parameters.at(7).value();
    GAL.Eccentricity = parameters.at(8).value();
    GAL.Cus = parameters.at(9).value();
    GAL.Sqrt_a = parameters.at(10).value();

    GAL.TOE = parameters.at(11).value();
    GAL.Cic = parameters.at(12).value();
    GAL.OMEGA0 = parameters.at(13).value();
    GAL.Cis = parameters.at(14).value();

    GAL.i0 = parameters.at(15).value();
    GAL.Crc = parameters.at(16).value();
    GAL.omega = parameters.at(17).value();
    GAL.OMEGA_DOT = parameters.at(18).value();

    GAL.IDOT = parameters.at(19).value();
    GAL.Data_sources = parameters.at(20).value();
    GAL.GAL_week = parameters.at(21).value();
    if(parameters.at(22).has_value())
        GAL.spare1 = parameters.at(22).value();

    GAL.SISA = parameters.at(23).value();
    GAL.svHealth = parameters.at(24).value();
    GAL.BGD_E5a = parameters.at(25).value();
    GAL.BGD_E5b = parameters.at(26).value();

    GAL.transmission_time = parameters.at(27).value();
    if(parameters.size() > 28 && parameters.at(28).has_value())
        GAL.spare2 = parameters.at(28).value();
    if(parameters.size() > 29 && parameters.at(29).has_value())
        GAL.spare3 = parameters.at(29).value();
    if(parameters.size() > 30 && parameters.at(30).has_value())
        GAL.spare4 = parameters.at(30).value();

    GAL.isAvailable = true;
    return GAL;
}

// Reader for Galileo navigation file
void Rinex3Nav::readGAL(std::ifstream& infile) {
    // A vector to hold block of sentences
    vector<string> block;
    // To hold contents of a line from input file
    string line;
    int nlines = 0;

    readHead(infile, SatelliteSystem::Galileo);


    // Create GAL Navigation data holder
    map<int, vector<Rinex3Nav::DataGAL>> mapGAL;

    // Reading Navigation Data Body
    while (!infile.eof()) {
        line.clear();
        // Temporarily store line from input file
        getline(infile, line, '\n'); nlines++;
        if (line.find_first_not_of(' ') == std::string::npos) { continue; }
        // Adjust line spaces before adding to block
        if (nlines != 1) {
            line = line.substr(4, line.length());
        }
        block.push_back(line);
        // New block of navigation message
        if (nlines == 8) {
            // Now we must process the block of lines
            Rinex3Nav::DataGAL GAL = epochNavOrganizerGAL(block);
            block.clear(); nlines = 0;
            // Add organized data to data holder
            // Save to Map: if PRN exists in map, then add NavInfo to vector of structs
            // Else add new PRN as key and GAL data structure as Value
            if (mapGAL.find(GAL.PRN) == mapGAL.end()) {
                // not found, therefore insert PRN and corresponding value
                vector<DataGAL> mapNavVector; mapNavVector.push_back(GAL);
                mapGAL.insert(pair<int, vector<DataGAL>>(GAL.PRN, mapNavVector));
            }
            else {
                // found, therefore add to existing PRN
                mapGAL[GAL.PRN].push_back(GAL);
            }
        }
    }
    _navGAL = mapGAL;
}
//=======================================================================================


//=================================BeiDou================================================
Rinex3Nav::DataBEI epochNavOrganizerBEI(vector<string> block) {
    string sys = block[0].substr(0, 1);
    int prn = stoi(block[0].substr(1, 2));
    vector<double> epochInfo = rinex3EpochTimeOrganizer(block[0]);
    string line = block[0].substr(23, block[0].length());
    for (unsigned int i = 1; i < block.size(); i++) {
        if (block[i].at(0) !=  '-' && block[i].at(0) != ' '){
            line += ' ' + block[i];
            continue;
        }
        line += block[i];
    }
    vector<std::optional<double>> parameters = rinex3NavDataSplitter(line);
    // Storing Values into BEI Data Structure
    Rinex3Nav::DataBEI BEI;
    BEI.PRN = prn;
    BEI.epochInfo = epochInfo;
    BEI.gpsTime = gpsTime(epochInfo);
    BEI.clockBias = parameters.at(0).value();
    BEI.clockDrift = parameters.at(1).value();
    BEI.clockDriftRate = parameters.at(2).value();

    BEI.AODE = parameters.at(3).value();
    BEI.Crs = parameters.at(4).value();
    BEI.Delta_n = parameters.at(5).value();
    BEI.M0 = parameters.at(6).value();

    BEI.Cuc = parameters.at(7).value();
    BEI.Eccentricity = parameters.at(8).value();
    BEI.Cus = parameters.at(9).value();
    BEI.Sqrt_a = parameters.at(10).value();

    BEI.TOE = parameters.at(11).value();
    BEI.Cic = parameters.at(12).value();
    BEI.OMEGA0 = parameters.at(13).value();
    BEI.Cis = parameters.at(14).value();

    BEI.i0 = parameters.at(15).value();
    BEI.Crc = parameters.at(16).value();
    BEI.omega = parameters.at(17).value();
    BEI.OMEGA_DOT = parameters.at(18).value();

    BEI.IDOT = parameters.at(19).value();
    if(parameters.at(20).has_value())
        BEI.spare1 = parameters.at(20).value();
    BEI.BDT_week = parameters.at(21).value();
    if(parameters.at(22).has_value())
        BEI.spare2 = parameters.at(22).value();

    BEI.SVaccuracy = parameters.at(23).value();
    BEI.SatH1 = parameters.at(24).value();
    BEI.TGD1 = parameters.at(25).value();
    BEI.TGD2 = parameters.at(26).value();

    BEI.transmission_time = parameters.at(27).value();
    BEI.AODC = parameters.at(28).value();
    if(parameters.size() > 29 && parameters.at(29).has_value())
        BEI.spare3 = parameters.at(29).value();
    if(parameters.size() > 30 && parameters.at(30).has_value())
        BEI.spare4 = parameters.at(30).value();


    BEI.isAvailable = true;
    return BEI;
}

void Rinex3Nav::readBEI(std::ifstream& infile)
{
    // A vector to hold block of sentences
    vector<string> block;
    // To hold contents of a line from input file
    string line;
    int nlines = 0;

    readHead(infile, SatelliteSystem::BeiDou);

    // Create BEI Navigation data holder
    map<int, vector<Rinex3Nav::DataBEI>> mapBEI;

    // Reading Navigation Data Body
    while (!infile.eof()) {
        line.clear();
        // Temporarily store line from input file
        getline(infile, line, '\n'); nlines++;
        if (line.find_first_not_of(' ') == std::string::npos) { continue; }
        // Adjust line spaces before adding to block
        if (nlines != 1) {
            line = line.substr(4, line.length());
        }
        block.push_back(line);
        // New block of navigation message
        if (nlines == 8) {
            // Now we must process the block of lines
            Rinex3Nav::DataBEI BEI = epochNavOrganizerBEI(block);
            block.clear(); nlines = 0;
            // Add organized data to data holder
            // Save to Map: if PRN exists in map, then add NavInfo to vector of structs
            // Else add new PRN as key and BEI data structure as Value
            if (mapBEI.find(BEI.PRN) == mapBEI.end()) {
                // not found, therefore insert PRN and corresponding value
                vector<DataBEI> mapNavVector;
                mapNavVector.push_back(BEI);
                mapBEI.insert(pair<int, vector<DataBEI>>(BEI.PRN, mapNavVector));
            }
            else {
                // found, therefore add to existing PRN
                mapBEI[BEI.PRN].push_back(BEI);
            }
        }
    }
    _navBEI = mapBEI;
}
//=======================================================================================


// Reader for GPS navigation file
void Rinex3Nav::readMixed(std::ifstream& infile) {
    vector<string> block;

    string line;
    int nlines = 0;

    readHead(infile, SatelliteSystem::Mixed);

    // Create GPS Navigation data holder
    map<int, vector<Rinex3Nav::DataGPS>> mapGPS;
    map<int, vector<Rinex3Nav::DataGLO>> mapGLO;
    map<int, vector<Rinex3Nav::DataGAL>> mapGAL;
    map<int, vector<Rinex3Nav::DataBEI>> mapBEI;


    // Reading Navigation Data Body
    while (!(infile >> std::ws).eof()) {
        // *** Deal with end of file error
        if (infile.fail()) { break; }
        // ***

        // Temporarily store line from input file
        line.clear();
        getline(infile, line, '\n');

        // Constellation identifier
        string ID = line.substr(0, 1);

        // GPS
        if(ID.find('G') != std::string::npos) {
            nlines = 0;
            while ((!(infile >> std::ws).eof()) || (nlines <= 8)) {
                // *** Deal with end of file error
                if (infile.fail()) { break; }
                // ***

                if (line.find_first_not_of(' ') == string::npos) { continue; }
                block.push_back(line); nlines++;

                // New block of navigation message
                if (nlines == 8) {
                    // Now we must process the block of lines
                    DataGPS GPS = epochNavOrganizerGPS(block);
                    block.clear(); line.clear();
                    // Add organized data to data holder
                    // Save to Map: if PRN exists in map, then add NavInfo to vector of structs
                    // Else add new PRN as key and GPS data structure as Value
                    if (mapGPS.find(GPS.PRN) == mapGPS.end()) {
                        // not found, therefore insert PRN and corresponding value
                        vector<DataGPS> mapNavVector; mapNavVector.push_back(GPS);
                        mapGPS.insert(pair<int, vector<DataGPS>>(GPS.PRN, mapNavVector));
                    }
                    else {
                        // found, therefore add to existing PRN
                        mapGPS[GPS.PRN].push_back(GPS);
                    }
                    break;
                }
                if (nlines < 8) {
                    line.clear();
                    getline(infile, line, '\n');
                }
            }
        }

        // GALILEO
        if (ID.find('E') != std::string::npos) {
            nlines = 0;
            while ((!(infile >> std::ws).eof()) || (nlines <= 8)) {
                // *** Deal with end of file error
                if (infile.fail()) { break; }
                // ***

                if (line.find_first_not_of(' ') == string::npos) { continue; }
                block.push_back(line); nlines++;

                // New block of navigation message
                if (nlines == 8) {
                    // Now we must process the block of lines
                    Rinex3Nav::DataGAL GAL = epochNavOrganizerGAL(block);
                    block.clear(); line.clear();
                    // Add organized data to data holder
                    // Save to Map: if PRN exists in map, then add NavInfo to vector of structs
                    // Else add new PRN as key and GAL data structure as Value
                    if (mapGAL.find(GAL.PRN) == mapGAL.end()) {
                        // not found, therefore insert PRN and corresponding value
                        vector<DataGAL> mapNavVector; mapNavVector.push_back(GAL);
                        mapGAL.insert(pair<int, vector<DataGAL>>(GAL.PRN, mapNavVector));
                    }
                    else {
                        // found, therefore add to existing PRN
                        mapGAL[GAL.PRN].push_back(GAL);
                    }
                    break;
                }
                if (nlines < 8) {
                    line.clear();
                    getline(infile, line, '\n');
                }
            }
        }

        // GLONASS
        if (ID.find('R') != std::string::npos) {
            nlines = 0;
            while ((!(infile >> std::ws).eof()) || (nlines <= 4)) {
                // *** Deal with end of file error
                if (infile.fail()) { break; }
                // ***

                if (line.find_first_not_of(' ') == string::npos) { continue; }
                block.push_back(line); nlines++;

                // New block of navigation message
                if (nlines == 4) {
                    // Now we must process the block of lines
                    Rinex3Nav::DataGLO GLO = epochNavOrganizerGLO(block);
                    block.clear(); line.clear();
                    // Add organized data to data holder
                    // Save to Map: if PRN exists in map, then add NavInfo to vector of structs
                    // Else add new PRN as key and GLO data structure as Value
                    if (mapGLO.find(GLO.PRN) == mapGLO.end()) {
                        // not found, therefore insert PRN and corresponding value
                        vector<DataGLO> mapNavVector; mapNavVector.push_back(GLO);
                        mapGLO.insert(pair<int, vector<DataGLO>>(GLO.PRN, mapNavVector));
                    }
                    else {
                        // found, therefore add to existing PRN
                        mapGLO[GLO.PRN].push_back(GLO);
                    }
                    break;
                }
                if (nlines < 4) {
                    line.clear();
                    getline(infile, line, '\n');
                }
            }
        }

        // BEIDO
        if (ID.find('C') != std::string::npos) {
            nlines = 0;
            while ((!(infile >> std::ws).eof()) || (nlines <= 8)) {
                // *** Deal with end of file error
                if (infile.fail()) { break; }
                // ***

                if (line.find_first_not_of(' ') == string::npos) { continue; }
                block.push_back(line); nlines++;

                // New block of navigation message
                if (nlines == 8) {
                    // Now we must process the block of lines
                    Rinex3Nav::DataBEI BEI = epochNavOrganizerBEI(block);
                    block.clear(); line.clear();
                    // Add organized data to data holder
                    // Save to Map: if PRN exists in map, then add NavInfo to vector of structs
                    // Else add new PRN as key and BEI data structure as Value
                    if (mapBEI.find(BEI.PRN) == mapBEI.end()) {
                        // not found, therefore insert PRN and corresponding value
                        vector<DataBEI> mapNavVector; mapNavVector.push_back(BEI);
                        mapBEI.insert(pair<int, vector<DataBEI>>(BEI.PRN, mapNavVector));
                    }
                    else {
                        // found, therefore add to existing PRN
                        mapBEI[BEI.PRN].push_back(BEI);
                    }
                    break;
                }
                if (nlines < 8) {
                    line.clear();
                    getline(infile, line, '\n');
                }
            }
        }
    }

    // Update the attribute of Navigation Objects
    _navGPS = mapGPS;
    _navGLO = mapGLO;
    _navGAL = mapGAL;
    _navBEI = mapBEI;
}



void Rinex3Nav::clear()
{
    _headerGAL.clear();
    _headerGLO.clear();
    _headerGPS.clear();
    _headerBEI.clear();

    _navGAL.clear();
    _navGLO.clear();
    _navGPS.clear();
    _navBEI.clear();
}

template<typename T>
void Rinex3Nav::readBody(map<int, vector<T> > &mapData, ifstream& infile, SatelliteSystem sys)
{
    vector<string> block;
    string line;
    int nlines = 0;
    int size = (sys == SatelliteSystem::Glonass ? 4 : 8);
    // Reading Navigation Data Body
    while (!infile.eof()) {
        line.clear();
        // Temporarily store line from input file
        getline(infile, line, '\n'); nlines++;
        if (line.find_first_not_of(' ') == std::string::npos) { continue; }
        // Adjust line spaces before adding to block
        if (nlines != 1) {
            line = line.substr(4, line.length());
        }
        block.push_back(line);
        // New block of navigation message
        if (nlines == size) {
            // Now we must process the block of lines
            T data;
            switch (sys) {
            case SatelliteSystem::GPS:{
                data = epochNavOrganizerGPS(block);
                break;
            }
            case SatelliteSystem::Galileo:{
                data = epochNavOrganizerGAL(block);
                break;
            }
            case SatelliteSystem::Glonass:{
                data = epochNavOrganizerGLO(block);
                break;
            }
            case SatelliteSystem::BeiDou:{
                data = epochNavOrganizerBEI(block);
                break;
            }
            case SatelliteSystem::Mixed:{

                break;
            }
            default:
                break;
            }

            block.clear();
            nlines = 0;
            // Add organized data to data holder
            // Save to Map: if PRN exists in map, then add NavInfo to vector of structs
            // Else add new PRN as key and GPS data structure as Value

            if (mapData.find(data.PRN) == mapData.end()) {
                // not found, therefore insert PRN and corresponding value
                vector<T> mapNavVector;
                mapNavVector.push_back(data);
                mapData.insert(pair<int, vector<T>>(data.PRN, mapNavVector));
            }
            else {
                // found, therefore add to existing PRN
                mapData[data.PRN].push_back(data);
            }
        }
    }
}



//===================================Structs============================================
Rinex3Nav::DataGPS::DataGPS(){}

Rinex3Nav::DataGPS &Rinex3Nav::DataGPS::operator=(DataGPS &&other)
{
    if (this != &other){
        this->isAvailable = other.isAvailable;
        this->PRN = other.PRN;
        this->epochInfo = other.epochInfo;
        this->gpsTime = other.gpsTime;
        this->clockBias = other.clockBias;
        this->clockDrift = other.clockDrift;
        this->clockDriftRate = other.clockDriftRate;

        this->IODE = other.IODE;
        this->Crs = other.Crs;
        this->Delta_n = other.Delta_n;
        this->M0 = other.M0;

        this->Cuc = other.Cuc;
        this->Eccentricity = other.Eccentricity;
        this->Cus = other.Cus;
        this->Sqrt_a = other.Sqrt_a;

        this->TOE = other.TOE;
        this->Cic = other.Cic;
        this->OMEGA0 = other.OMEGA0;
        this->Cis = other.Cis;

        this->i0 = other.i0;
        this->Crc = other.Crc;
        this->omega = other.omega;
        this->OMEGA_DOT = other.OMEGA_DOT;

        this->IDOT = other.IDOT;
        this->L2_codes_channel = other.L2_codes_channel;
        this->GPS_week = other.GPS_week;
        this->L2_P_data_flag = other.L2_P_data_flag;

        this->svAccuracy = other.svAccuracy;
        this->svHealth = other.svHealth;
        this->TGD = other.TGD;
        this->IODC = other.IODC;

        this->transmission_time = other.transmission_time;
        this->fit_interval = other.fit_interval;
        this->spare1 = other.spare1;
        this->spare2 = other.spare2;
    }
    return *this;
}

std::vector<std::optional<double>> Rinex3Nav::DataGPS::toVec()
{
    std::vector<std::optional<double>> vecData;
    vecData.push_back(clockBias);
    vecData.push_back(clockDrift);
    vecData.push_back(clockDriftRate);

    vecData.push_back(IODE);
    vecData.push_back(Crs);
    vecData.push_back(Delta_n);
    vecData.push_back(M0);

    vecData.push_back(Cuc);
    vecData.push_back(Eccentricity);
    vecData.push_back(Cus);
    vecData.push_back(Sqrt_a);

    vecData.push_back(TOE);
    vecData.push_back(Cic);
    vecData.push_back(OMEGA0);
    vecData.push_back(Cis);

    vecData.push_back(i0);
    vecData.push_back(Crc);
    vecData.push_back(omega);
    vecData.push_back(OMEGA_DOT);

    vecData.push_back(IDOT);
    vecData.push_back(L2_codes_channel);
    vecData.push_back(GPS_week);
    vecData.push_back(L2_P_data_flag);

    vecData.push_back(svAccuracy);
    vecData.push_back(svHealth);
    vecData.push_back(TGD);
    vecData.push_back(IODC);

    vecData.push_back(transmission_time);
    vecData.push_back(fit_interval);
    vecData.push_back(spare1);
    vecData.push_back(spare2);

    return vecData;
}

Rinex3Nav::DataGLO::DataGLO(){}

Rinex3Nav::DataGLO &Rinex3Nav::DataGLO::operator=(DataGLO &&other)
{
    if (this != &other){
        this->isAvailable = other.isAvailable;
        this->PRN = other.PRN;
        this->epochInfo = other.epochInfo;
        this->gpsTime = other.gpsTime;
        this->clockBias = other.clockBias;
        this->relFreqBias = other.relFreqBias;
        this->messageFrameTime = other.messageFrameTime;

        this->satPosX = other.satPosX;
        this->satVelX = other.satVelX;
        this->satAccX = other.satAccX;
        this->satHealth = other.satHealth;

        this->satPosY = other.satPosY;
        this->satVelY = other.satVelY;
        this->satAccY = other.satAccY;
        this->freqNum = other.freqNum;

        this->satPosZ = other.satPosZ;
        this->satVelZ = other.satVelZ;
        this->satAccZ = other.satAccZ;
        this->infoAge = other.infoAge;
    }
    return *this;
}

std::vector<std::optional<double>> Rinex3Nav::DataGLO::toVec()
{
    std::vector<std::optional<double>> vecData;
    vecData.push_back(clockBias);
    vecData.push_back(relFreqBias);
    vecData.push_back(messageFrameTime);

    vecData.push_back(satPosX);
    vecData.push_back(satVelX);
    vecData.push_back(satAccX);
    vecData.push_back(satHealth);

    vecData.push_back(satPosY);
    vecData.push_back(satVelY);
    vecData.push_back(satAccY);
    vecData.push_back(freqNum);

    vecData.push_back(satPosZ);
    vecData.push_back(satVelZ);
    vecData.push_back(satAccZ);
    vecData.push_back(infoAge);

    return vecData;
}

Rinex3Nav::DataGAL::DataGAL(){}

Rinex3Nav::DataGAL &Rinex3Nav::DataGAL::operator=(DataGAL &&other)
{

}

std::vector<std::optional<double>> Rinex3Nav::DataGAL::toVec()
{
    std::vector<std::optional<double>> vecData;
    vecData.push_back(clockBias);
    vecData.push_back(clockDrift);
    vecData.push_back(clockDriftRate);

    vecData.push_back(IOD);
    vecData.push_back(Crs);
    vecData.push_back(Delta_n);
    vecData.push_back(M0);

    vecData.push_back(Cuc);
    vecData.push_back(Eccentricity);
    vecData.push_back(Cus);
    vecData.push_back(Sqrt_a);

    vecData.push_back(TOE);
    vecData.push_back(Cic);
    vecData.push_back(OMEGA0);
    vecData.push_back(Cis);

    vecData.push_back(i0);
    vecData.push_back(Crc);
    vecData.push_back(omega);
    vecData.push_back(OMEGA_DOT);

    vecData.push_back(IDOT);
    vecData.push_back(Data_sources);
    vecData.push_back(GAL_week);
    vecData.push_back(spare1);

    vecData.push_back(SISA);
    vecData.push_back(svHealth);
    vecData.push_back(BGD_E5a);
    vecData.push_back(BGD_E5b);

    vecData.push_back(transmission_time);
    vecData.push_back(spare2);
    vecData.push_back(spare3);
    vecData.push_back(spare4);


    return vecData;
}

Rinex3Nav::DataBEI::DataBEI(){}

Rinex3Nav::DataBEI &Rinex3Nav::DataBEI::operator=(DataBEI &&other)
{

}

std::vector<std::optional<double>> Rinex3Nav::DataBEI::toVec()
{
    std::vector<std::optional<double>> vecData;
    vecData.push_back(clockBias);
    vecData.push_back(clockDrift);
    vecData.push_back(clockDriftRate);

    vecData.push_back(AODE);
    vecData.push_back(Crs);
    vecData.push_back(Delta_n);
    vecData.push_back(M0);

    vecData.push_back(Cuc);
    vecData.push_back(Eccentricity);
    vecData.push_back(Cus);
    vecData.push_back(Sqrt_a);

    vecData.push_back(TOE);
    vecData.push_back(Cic);
    vecData.push_back(OMEGA0);
    vecData.push_back(Cis);

    vecData.push_back(i0);
    vecData.push_back(Crc);
    vecData.push_back(omega);
    vecData.push_back(OMEGA_DOT);

    vecData.push_back(IDOT);
    vecData.push_back(spare1);
    vecData.push_back(BDT_week);
    vecData.push_back(spare2);

    vecData.push_back(SVaccuracy);
    vecData.push_back(SatH1);
    vecData.push_back(TGD1);
    vecData.push_back(TGD2);

    vecData.push_back(transmission_time);
    vecData.push_back(AODC);
    vecData.push_back(spare3);
    vecData.push_back(spare4);


    return vecData;
}
//======================================================================================

//======================================================================================
ViewNav::ViewNav(){}

ViewNav &ViewNav::operator=(const Rinex3Nav &other)
{
    this->_headerGPS = other._headerGPS;
    this->_headerGLO = other._headerGLO;
    this->_headerGAL = other._headerGAL;
    this->_headerBEI = other._headerBEI;

    this->_navGPS = other._navGPS;
    this->_navGLO = other._navGLO;
    this->_navGAL = other._navGAL;
    this->_navBEI = other._navBEI;
    return *this;
}

ViewNav::ViewNav(const Rinex3Nav &other)
{
    this->_headerGPS = other._headerGPS;
    this->_headerGLO = other._headerGLO;
    this->_headerGAL = other._headerGAL;
    this->_headerBEI = other._headerBEI;

    this->_navGPS = other._navGPS;
    this->_navGLO = other._navGLO;
    this->_navGAL = other._navGAL;
    this->_navBEI = other._navBEI;
}
//======================================================================================


//======================================================================================
void Rinex3Nav::HeaderGPS::clear()
{
    GPUT.clear();
    ialpha.clear();
    ibeta.clear();
    leapSec = 0;
    pgm.clear();
}

void Rinex3Nav::HeaderGLO::clear()
{
    GLUT.clear();
    leapSec = 0;
    pgm.clear();
}

void Rinex3Nav::HeaderGAL::clear()
{
    GAUT.clear();
    gal.clear();
    pgm.clear();
    leapSec = 0;
}

void Rinex3Nav::HeaderBEI::clear()
{
    BDUT.clear();
    leapSec = 0;
    ialpha.clear();
    ibeta.clear();
    pgm.clear();
}
//======================================================================================
