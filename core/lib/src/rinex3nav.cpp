#include "rinex3nav.h"
#include <regex>

using namespace std;

rr::Rinex3Nav::Rinex3Nav() {}

rr::Rinex3Nav::~Rinex3Nav() {}

// A function to help with organizing headers
// Works for the alpha/beta ionospheric constants and time correction
rr::Rinex3Nav::IonCorr getIonCorr(string line) {
    rr::Rinex3Nav::IonCorr ic;
    ic.type = line.substr(0,4);
    string temp_str;
    for (unsigned i = 5; i < 53; i += 12) {
        temp_str = line.substr(i, 12);
        if (temp_str.find_first_not_of(' ') == std::string::npos) { continue; }
        ic.params.push_back(stod(replaceChars(temp_str, 'D', 'e')));
        temp_str.clear();
    }
    temp_str = line.substr(54,1);
    if(temp_str.find_first_not_of(' ') != string::npos)
        ic.time_mark = temp_str;
    temp_str =line.substr(56,2);
    if(temp_str.find_first_not_of(' ') != string::npos)
        ic.sv_id = stoi(temp_str);
    return ic;
}

// A function to organization and reading data for time system correction
rr::Rinex3Nav::TimeSysCorr rr::Rinex3Nav::getTimeSysCorr(std::string line)
{
    TimeSysCorr t;
    t.type = line.substr(0,4);
    t.a0_coef = stod(replaceChars(line.substr(5,17), 'D','e'));
    t.a1_coef = stod(replaceChars(line.substr(22,16), 'D','e'));
    t.ref_time = stoi(line.substr(38,7));
    t.week_number = stoi(line.substr(45,5));
    string temp = line.substr(51,5);
    if(temp.find_first_not_of(' ') != string::npos)
        t.source = temp;
    temp = line.substr(57,2);
    if(temp.find_first_not_of(' ') != string::npos)
        t.utc_id = stoi(temp);
    return t;
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
int rr::Rinex3Nav::EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGPS> NAV) {
    return epochMatcherHelper(obsTime,NAV);
}

// Epoch Time Matcher, returns index of most appropriate data from Navigation vector
int rr::Rinex3Nav::EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGLO> NAV) {
    return epochMatcherHelper(obsTime,NAV);
}

// Epoch Time Matcher, returns index of most appropriate data from Navigation vector
int rr::Rinex3Nav::EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGAL> NAV) {
    return epochMatcherHelper(obsTime,NAV);
}

// Epoch Time Matcher, returns index of most appropriate data from Navigation vector
int rr::Rinex3Nav::EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataBEI> NAV) {
    return epochMatcherHelper(obsTime,NAV);
}
//=======================================================================================

void rr::Rinex3Nav::readHead(std::ifstream& infile){
    //common tokens
    const string sTokenVER = "RINEX VERSION / TYPE";
    const string sTokenPGM = "PGM / RUN BY / DATE";
    const string sTokenCOM = "COMMENT";
    const string sTokenIONO = "IONOSPHERIC CORR";
    const string sTokenCORR = "TIME SYSTEM CORR";
    const string sTokenLEAP = "LEAP SECONDS";
    const string sTokenEND = "END OF HEADER";

    string line;

    switch (curr_sys) {
    case SatelliteSystem::GPS:{
        //iono
        const string sTokenGPSA = "GPSA";//= GPS alpha0 - alpha3
        const string sTokenGPSB = "GPSB";//= GPS beta0 - beta3
        //time sys core
        const string sTokenGPUT = "GPUT";//= GPS - UTC (a0, a1)

        while (!infile.eof()) {
            line.clear();
            // Temporarily store line from input file
            getline(infile, line, '\n');
            // Looking for keywords in Header Part...
            size_t found_VER = line.find(sTokenVER);
            size_t found_PGM = line.find(sTokenPGM);
            size_t found_COM = line.find(sTokenCOM);
            size_t found_IONO = line.find(sTokenIONO);
            size_t found_CORR = line.find(sTokenCORR);
            size_t found_LEAP = line.find(sTokenLEAP);
            size_t found_END = line.find(sTokenEND);

            if (found_VER != string::npos) {
                istringstream iss(line.substr(0, 60));
                // Rinex type should be stored in 4th word of line
                vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
                _headerGPS.version = std::stod(words[0]);
                _headerGPS.type = words[words.size() - 1];
                words.clear();
                continue;
            }
            if(found_PGM != string::npos){
                _headerGPS.pgm = getPMGHeader(line);
                continue;
            }
            // Finding Comments, meaning skip!
            if (found_COM != string::npos){
                _headerGPS.comments.push_back(line.substr(0, 60));
                continue;
            }
            // Finding Ionophseric Constants as per new format
            if (found_IONO != string::npos) {
                size_t found_GPSA = line.find(sTokenGPSA);
                size_t found_GPSB = line.find(sTokenGPSB);
                if (found_GPSA != string::npos) {
                    _headerGPS.ialpha = getIonCorr(line);
                }
                if (found_GPSB != string::npos) {
                    _headerGPS.ibeta = getIonCorr(line);
                }
                continue;
            }
            // Finding GPS to UTC Time Correction
            if (found_CORR != string::npos) {
                size_t found_GPUT = line.find(sTokenGPUT);
                if (found_GPUT != string::npos)
                    _headerGPS.GPUT = getTimeSysCorr(line);
                continue;
            }
            if (found_LEAP != string::npos) {
                line = line.substr(0, 24);
                istringstream iss(line);
                copy(istream_iterator<int>(iss),
                     istream_iterator<int>(),
                     back_inserter(_headerGPS.leap_seconds));
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

            size_t found_VER = line.find(sTokenVER);
            size_t found_PGM = line.find(sTokenPGM);
            size_t found_COM = line.find(sTokenCOM);
            size_t found_IONO = line.find(sTokenIONO);
            size_t found_CORR = line.find(sTokenCORR);
            size_t found_LEAP = line.find(sTokenLEAP);
            size_t found_END = line.find(sTokenEND);

            if (found_VER != string::npos) {
                istringstream iss(line.substr(0, 60));
                // Rinex type should be stored in 4th word of line
                vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
                _headerGAL.version = std::stod(words[0]);
                _headerGAL.type = words[words.size() - 1];
                words.clear();
                continue;
            }
            if(found_PGM != string::npos){
                _headerGAL.pgm = getPMGHeader(line);
            }
            // Finding Comments, meaning skip!
            if (found_COM != string::npos) {
                _headerGAL.comments.push_back(line.substr(0, 60));
                continue;
            }
            // Finding Ionophseric Constants as per new format
            if (found_IONO != string::npos) {
                size_t found_GAL = line.find(sTokenGAL);
                if (found_GAL != string::npos) {
                    _headerGAL.gal = getIonCorr(line);
                }
                continue;
            }
            // Finding Time Correction
            if (found_CORR != string::npos) {
                size_t found_GAUT = line.find(sTokenGAUT);
                if (found_GAUT != string::npos)
                    _headerGAL.GAUT = getTimeSysCorr(line);
                continue;
            }
            // Finding Leap Second
            if (found_LEAP != string::npos) {
                line = line.substr(0, 24);
                istringstream iss(line);
                copy(istream_iterator<int>(iss),
                     istream_iterator<int>(),
                     back_inserter(_headerGAL.leap_seconds));
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

            size_t found_VER = line.find(sTokenVER);
            size_t found_PGM = line.find(sTokenPGM);
            size_t found_COM = line.find(sTokenCOM);
            size_t found_CORR = line.find(sTokenCORR);
            size_t found_LEAP = line.find(sTokenLEAP);
            size_t found_END = line.find(sTokenEND);

            if (found_VER != string::npos) {
                istringstream iss(line.substr(0, 60));
                // Rinex type should be stored in 4th word of line
                vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
                _headerGLO.version = std::stod(words[0]);
                _headerGLO.type = words[words.size() - 1];
                words.clear();
                continue;
            }
            if(found_PGM != string::npos){
                _headerGLO.pgm = getPMGHeader(line);
            }
            if (found_COM != string::npos) {
                _headerGLO.comments.push_back(line.substr(0, 60));
                continue;
            }
            if (found_CORR != string::npos) {
                size_t found_GLUT = line.find(sTokenGLUT);
                if (found_GLUT != string::npos)
                    _headerGLO.GLUT = getTimeSysCorr(line);
                continue;
            }
            if (found_LEAP != string::npos) {
                line = line.substr(0, 24);
                istringstream iss(line);
                copy(istream_iterator<int>(iss),
                     istream_iterator<int>(),
                     back_inserter(_headerGLO.leap_seconds));
                continue;
            }
            if (found_END != string::npos) {
                break;
            }
        }
        break;
    }
    case SatelliteSystem::BeiDou:{
        //iono
        const string sTokenBDSA = "BDSA";//= BDS alpha0 - alpha3
        const string sTokenBDSB = "BDSB";//= BDS beta0 - beta3
        //time sys cores
        const string sTokenBDUT = "GDUT";//= BDS - UTC (a0=A0UTC, a1=A1UTC )

        while (!infile.eof()) {
            line.clear();

            getline(infile, line, '\n');

            size_t found_VER = line.find(sTokenVER);
            size_t found_PGM = line.find(sTokenPGM);
            size_t found_COM = line.find(sTokenCOM);
            size_t found_IONO = line.find(sTokenIONO);
            size_t found_CORR = line.find(sTokenCORR);
            size_t found_LEAP = line.find(sTokenLEAP);
            size_t found_END = line.find(sTokenEND);


            if (found_VER != string::npos) {
                istringstream iss(line.substr(0, 60));
                // Rinex type should be stored in 4th word of line
                vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
                _headerBEI.version = std::stod(words[0]);
                _headerBEI.type = words[words.size() - 1];
                words.clear();
                continue;
            }
            if(found_PGM != string::npos){
                _headerBEI.pgm = getPMGHeader(line);
            }
            // Finding Comments, meaning skip!
            if (found_COM != string::npos){
                _headerBEI.comments.push_back(line.substr(0, 60));
                continue;
            }
            // Finding Ionophseric Constants as per new format
            if (found_IONO != string::npos) {
                size_t found_DBSA = line.find(sTokenBDSA);
                size_t found_DBSB = line.find(sTokenBDSB);
                if (found_DBSA != string::npos) {
                    _headerBEI.ialpha = getIonCorr(line);
                }
                if (found_DBSB != string::npos) {
                    _headerBEI.ibeta = getIonCorr(line);
                }
                continue;
            }
            // Finding GPS to UTC Time Correction
            if (found_CORR != string::npos) {
                size_t found_BDUT = line.find(sTokenBDUT);
                if (found_BDUT != string::npos)
                    _headerBEI.BDUT = getTimeSysCorr(line);
                continue;
            }
            if (found_LEAP != string::npos) {
                line = line.substr(0, 24);
                istringstream iss(line);
                copy(istream_iterator<int>(iss),
                     istream_iterator<int>(),
                     back_inserter(_headerBEI.leap_seconds));
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

            size_t found_VER = line.find(sTokenVER);
            size_t found_PGM = line.find(sTokenPGM);
            size_t found_COM = line.find(sTokenCOM);
            size_t found_IONO = line.find(sTokenIONO);
            size_t found_CORR = line.find(sTokenCORR);
            size_t found_LEAP = line.find(sTokenLEAP);
            size_t found_END = line.find(sTokenEND);

            if (found_VER != string::npos) {
                istringstream iss(line.substr(0, 60));
                // Rinex type should be stored in 4th word of line
                vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
                _headerGPS.version = std::stod(words[0]);
                _headerGPS.type = words[words.size() - 1];

                _headerGAL.version = std::stod(words[0]);
                _headerGAL.type = words[words.size() - 1];

                _headerGLO.version = std::stod(words[0]);
                _headerGLO.type = words[words.size() - 1];

                _headerBEI.version = std::stod(words[0]);
                _headerBEI.type = words[words.size() - 1];
                words.clear();
                continue;
            }
            if(found_PGM != string::npos){
                _headerBEI.pgm = getPMGHeader(line);
                _headerGPS.pgm = getPMGHeader(line);
                _headerGAL.pgm = getPMGHeader(line);
                _headerGLO.pgm = getPMGHeader(line);
            }
            // Finding Comments, meaning skip!
            if (found_COM != string::npos){
                _headerGPS.comments.push_back(line.substr(0, 60));
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
                    _headerGPS.ialpha = getIonCorr(line);
                    continue;
                }
                if (found_GPSB != string::npos) {
                    _headerGPS.ibeta = getIonCorr(line);
                    continue;
                }
                if (found_GAL != string::npos) {
                    _headerGAL.gal = getIonCorr(line);
                    continue;
                }
                if (found_DBSA != string::npos) {
                    _headerBEI.ialpha = getIonCorr(line);
                    continue;
                }
                if (found_DBSB != string::npos) {
                    _headerBEI.ibeta = getIonCorr(line);
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
                    _headerGPS.GPUT = getTimeSysCorr(line);
                if (found_GAUT != string::npos)
                    _headerGAL.GAUT = getTimeSysCorr(line);
                if (found_BDUT != string::npos)
                    _headerBEI.BDUT = getTimeSysCorr(line);
                if (found_GLUT != string::npos)
                    _headerGLO.GLUT = getTimeSysCorr(line);
                continue;
            }
            if (found_LEAP != string::npos) {
                line = line.substr(0, 24);
                istringstream iss(line);
                copy(istream_iterator<int>(iss),
                     istream_iterator<int>(),
                     back_inserter(_headerGPS.leap_seconds));
                _headerBEI.leap_seconds = _headerGPS.leap_seconds;
                _headerGLO.leap_seconds = _headerGPS.leap_seconds;
                _headerGAL.leap_seconds = _headerGPS.leap_seconds;
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


std::vector<string> rr::Rinex3Nav::getPMGHeader(std::string line)
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
rr::Rinex3Nav::DataGPS epochNavOrganizerGPS(vector<string> block) {
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
    rr::Rinex3Nav::DataGPS GPS;
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
void rr::Rinex3Nav::readGPS(std::ifstream& infile) {
    curr_sys = SatelliteSystem::GPS;
    _headerGPS.clear();
    readHead(infile);

    _navGPS.clear();
    readBody(infile);
    curr_sys = SatelliteSystem::None;
}
//=======================================================================================


//=================================GLONASS===============================================
// Navigation Body Organizer for GLONASS Navigation File
rr::Rinex3Nav::DataGLO epochNavOrganizerGLO(vector<string> block) {
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
    rr::Rinex3Nav::DataGLO GLO;
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
void rr::Rinex3Nav::readGLO(std::ifstream& infile) {
    curr_sys = SatelliteSystem::Glonass;
    _headerGLO.clear();
    readHead(infile);

    _navGLO.clear();
    readBody(infile);
    curr_sys = SatelliteSystem::None;
}
//=======================================================================================


//=================================GALILEO===============================================
// Navigation Body Organizer for GAL Navigation File
rr::Rinex3Nav::DataGAL epochNavOrganizerGAL(vector<string> block) {
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
    rr::Rinex3Nav::DataGAL GAL;
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
void rr::Rinex3Nav::readGAL(std::ifstream& infile) {
    curr_sys = SatelliteSystem::Galileo;
    _headerGAL.clear();
    readHead(infile);

    _navGAL.clear();
    readBody(infile);
    curr_sys = SatelliteSystem::None;
}
//=======================================================================================


//=================================BeiDou================================================
rr::Rinex3Nav::DataBEI epochNavOrganizerBEI(vector<string> block) {
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
    rr::Rinex3Nav::DataBEI BEI;
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

void rr::Rinex3Nav::readBEI(std::ifstream& infile)
{
    curr_sys = SatelliteSystem::BeiDou;
    _headerBEI.clear();
    readHead(infile);

    _navBEI.clear();
    readBody(infile);
    curr_sys = SatelliteSystem::None;
}
//=======================================================================================

// Reader for GPS navigation file
void rr::Rinex3Nav::readMixed(std::ifstream& infile) {
    curr_sys = SatelliteSystem::Mixed;
    clear();

    readHead(infile);
    readBody(infile);

    if(_navGPS.empty())
        _headerGPS.clear();
    if(_navGAL.empty())
        _headerGAL.clear();
    if(_navGLO.empty())
        _headerGLO.clear();
    if(_navBEI.empty())
        _headerBEI.clear();
    curr_sys = SatelliteSystem::None;
}


void rr::Rinex3Nav::clear()
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


void rr::Rinex3Nav::readBody(ifstream& infile)
{
    vector<string> block;
    string line;
    int nlines = 0;
    int size = (curr_sys == SatelliteSystem::Glonass ? 4 : 8);
    // Reading Navigation Data Body
    while (!infile.eof()) {
        line.clear();
        getline(infile, line, '\n'); nlines++;

        if (line.find_first_not_of(' ') == std::string::npos) { continue; }

        string ID;
        if (nlines != 1){
            line = line.substr(4, line.length());
        }
        if (nlines == 1 && curr_sys == SatelliteSystem::Mixed){
            ID = line.substr(0, 1);
            ID == "G" ? size = 4 : size = 8;
        }

        block.push_back(line);
        // New block of navigation message
        if (nlines == size) {
            saveBlock(block, curr_sys, ID);

            block.clear();
            nlines = 0;
        }
    }
}

void rr::Rinex3Nav::saveBlock(std::vector<std::string>& block, SatelliteSystem sys, std::string ID)
{
    switch (sys) {
    case SatelliteSystem::GPS:{
        DataGPS data = epochNavOrganizerGPS(block);

        if (_navGPS.find(data.PRN) == _navGPS.end()) {
            // not found, therefore insert PRN and corresponding value
            vector<DataGPS> mapNavVector;
            mapNavVector.push_back(data);
            _navGPS.insert(pair<int, vector<DataGPS>>(data.PRN, mapNavVector));
        }
        else {
            // found, therefore add to existing PRN
            _navGPS[data.PRN].push_back(data);
        }
        break;
    }
    case SatelliteSystem::Galileo:{
        DataGAL data = epochNavOrganizerGAL(block);

        if (_navGAL.find(data.PRN) == _navGAL.end()) {
            // not found, therefore insert PRN and corresponding value
            vector<DataGAL> mapNavVector;
            mapNavVector.push_back(data);
            _navGAL.insert(pair<int, vector<DataGAL>>(data.PRN, mapNavVector));
        }
        else {
            // found, therefore add to existing PRN
            _navGAL[data.PRN].push_back(data);
        }
        break;
    }
    case SatelliteSystem::Glonass:{
        DataGLO data = epochNavOrganizerGLO(block);

        if (_navGLO.find(data.PRN) == _navGLO.end()) {
            // not found, therefore insert PRN and corresponding value
            vector<DataGLO> mapNavVector;
            mapNavVector.push_back(data);
            _navGLO.insert(pair<int, vector<DataGLO>>(data.PRN, mapNavVector));
        }
        else {
            // found, therefore add to existing PRN
            _navGLO[data.PRN].push_back(data);
        }
        break;
    }
    case SatelliteSystem::BeiDou:{
        DataBEI data = epochNavOrganizerBEI(block);

        if (_navBEI.find(data.PRN) == _navBEI.end()) {
            // not found, therefore insert PRN and corresponding value
            vector<DataBEI> mapNavVector;
            mapNavVector.push_back(data);
            _navBEI.insert(pair<int, vector<DataBEI>>(data.PRN, mapNavVector));
        }
        else {
            // found, therefore add to existing PRN
            _navBEI[data.PRN].push_back(data);
        }

        break;
    }
    case SatelliteSystem::Mixed:{
        saveBlock(block, getSatelliteSystem(ID.data()));
        break;
    }
    default:
        break;
    }
}


//===================================Structs============================================

std::vector<std::optional<double>> rr::Rinex3Nav::DataGPS::toVec()
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

std::vector<std::optional<double>> rr::Rinex3Nav::DataGLO::toVec()
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

std::vector<std::optional<double>> rr::Rinex3Nav::DataGAL::toVec()
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

std::vector<std::optional<double>> rr::Rinex3Nav::DataBEI::toVec()
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
rr::ViewNav::ViewNav(){}

rr::ViewNav &rr::ViewNav::operator=(const Rinex3Nav &other)
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

rr::ViewNav::ViewNav(const Rinex3Nav &other)
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
void rr::Rinex3Nav::HeaderGPS::clear()
{
    type.clear();
    version = 0;

    pgm.clear();
    comments.clear();

    ialpha.clear();
    ibeta.clear();

    GPUT.clear();
    leap_seconds.clear();

}

void rr::Rinex3Nav::HeaderGLO::clear()
{
    type.clear();
    version = 0;

    pgm.clear();
    comments.clear();

    GLUT.clear();
    leap_seconds.clear();
}

void rr::Rinex3Nav::HeaderGAL::clear()
{
    type.clear();
    version = 0;

    pgm.clear();
    comments.clear();

    gal.clear();

    GAUT.clear();
    leap_seconds.clear();
}

void rr::Rinex3Nav::HeaderBEI::clear()
{
    type.clear();
    version = 0;

    pgm.clear();
    comments.clear();

    ialpha.clear();
    ibeta.clear();

    BDUT.clear();
    leap_seconds.clear();
}
//======================================================================================

void rr::Rinex3Nav::TimeSysCorr::clear()
{
    this->type.clear();
    a0_coef = 0;
    a1_coef = 0;
    ref_time = 0;
    week_number = 0;
    if(source.has_value())
        source = nullopt;
    if(utc_id.has_value())
        utc_id = nullopt;

}

void rr::Rinex3Nav::IonCorr::clear()
{
    type.clear();
    params.clear();
    if(time_mark.has_value())
        time_mark = nullopt;
    if(sv_id.has_value())
        sv_id = nullopt;
}
