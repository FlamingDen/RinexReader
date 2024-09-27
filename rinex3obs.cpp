#include "rinex3obs.h"
#include "timeutils.h"
#include "stringutils.h"
#include <regex>

using namespace std;

// CONSTRUCTOR AND DESTRUCTOR DEFINITIONS
Rinex3Obs::Rinex3Obs() {}
Rinex3Obs::~Rinex3Obs() {}

// A function to organize specific observation types (for eg: prn - > pseudorange map)
// PRN - > Pseudorange Map is used for satellite position
map<int, double> Rinex3Obs::specificObsMapper(map<int, vector<double>> obsSAT, vector<string> obsTypes, string specificObs) {
    map<int, double> rangeMap;
    map<int, vector<double>>::iterator it = obsSAT.begin();
    size_t ind = std::find(obsTypes.begin(), obsTypes.end(), specificObs) - obsTypes.begin();
    if (ind >= obsTypes.size()) {
        cout << "Requested C1 Observation is Unavailable" << endl;
    }
    else if (ind < obsTypes.size()) {
        for (it = obsSAT.begin(); it != obsSAT.end(); ++it) {
            rangeMap.insert(std::pair<int, double>(it->first, it->second.at(ind)));
        }
    }

    return rangeMap;
}


//===========================Header======================================================
// A function to organize observation types as stated in header of rinex observation file
map<string, vector<string>> obsTypesHeader(vector<string> block) {
    // Initializing variables to hold information
    map<string, vector<string>> types;
    // Satellite system identifier
    string sys;
    // Number of observation types
    int nTypes;
    // Temporary vars
    string line; vector<string> temp;
    for (unsigned int i = 0; i < block.size(); i++) {
        line = block[i];
        // Splitting words in the line
        istringstream iss(line);
        vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
        // Challenge is to organize based on satellite system identifier
        // First letter of line gives the satellite system
        // If number of types > 13, types extended on next line
        size_t sLength = words[0].length();
        if (sLength == 1) {
            sys = words[0];
            nTypes = stoi(words[1]);
            if (nTypes > 13 ) {
                int start_lines = i++;
                double number_of_lines = ceil(nTypes / 13.);
                for (int j(i); j < start_lines + number_of_lines ; j++, i++)
                    line += block[j];
                words.clear();
                istringstream iss2(line);
                copy(istream_iterator<string>(iss2), istream_iterator<string>(), back_inserter(words));
                i--;
            }
            for (unsigned int i = 2; i < words.size(); i++) {
                temp.push_back(words[i]);
            }
            types[sys] = temp;
            temp.clear();
        }
    }
    return types;
}

// A function to organize scale factor as stated in header of rinex observation file
std::vector<Rinex3Obs::ObsHeaderInfo::ScaleFactor> Rinex3Obs::obsScaleFactorHeader(std::vector<std::string> block) {
    // Initializing variables to hold information
    map<string, vector<string>> types;
    vector<Rinex3Obs::ObsHeaderInfo::ScaleFactor> vec_sf;
    Rinex3Obs::ObsHeaderInfo::ScaleFactor sf;
    // Satellite system identifier
    string sys;
    int scale;
    // Number of observation types
    int nTypes;
    // Temporary vars
    string line; vector<string> temp;
    for (unsigned int i = 0; i < block.size(); i++) {
        line = block[i];
        // Splitting words in the line
        istringstream iss(line);
        vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
        // Challenge is to organize based on satellite system identifier
        // First letter of line gives the satellite system
        // If number of types > 13, types extended on next lines.
        size_t sLength = words[0].length();
        if (sLength == 1) {
            sys = words[0];
            scale = stoi(words[1]);
            string count = line.substr(6,3);
            if(count.find_first_not_of(' ') == string::npos || stoi(count) == 0){
                sf.sat_system = sys;
                sf.scale = scale;
                sf.types = _Header.obsTypes[sys];
                vec_sf.push_back(sf);
                sf.clear();
                continue;
            }

            nTypes = stoi(count);
            if (nTypes > 13 ) {
                int start_lines = i++;
                double number_of_lines = ceil(nTypes / 13.);
                for (int j(i); j < start_lines + number_of_lines ; j++, i++)
                    line += block[j];
                words.clear();
                istringstream iss2(line);
                copy(istream_iterator<string>(iss2), istream_iterator<string>(), back_inserter(words));
                i--;
            }
            for (unsigned int i = 2; i < words.size(); i++) {
                temp.push_back(words[i]);
            }
        }
        sf.sat_system = sys;
        sf.scale = scale;
        sf.types = temp;
        vec_sf.push_back(sf);
        temp.clear();
        sf.clear();
    }
    return vec_sf;
}

// A function to organize phase shift as stated in header of rinex observation file
std::map<string, std::vector<Rinex3Obs::ObsHeaderInfo::PhaseShifts>> Rinex3Obs::obsPhaseShiftsHeader(std::vector<std::string> block)
{
    std::map<std::string, vector<ObsHeaderInfo::PhaseShifts>> phase_shifts;
    ObsHeaderInfo::PhaseShifts shift;
    string sys;

    string correction;
    int count;
    optional<vector<string>> sats;

    string line;
    for (size_t i(0); i < block.size(); i++) {
        line = block[i];
        istringstream iss(line);
        vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
        size_t sLength = words[0].length();
        if (sLength == 1) {
            sys = words[0];
            shift.code = words[1];

            correction = line.substr(6,8);
            if(correction.find_first_not_of(' ') != string::npos){
                shift.shift_correction = stod(correction);
            }

            string str_count = line.substr(16,2);
            if(str_count.find_first_not_of(' ') == string::npos || stoi(str_count) == 0){
                phase_shifts[sys].push_back(shift);
                shift.clear();
                continue;
            }

            count = stoi(str_count);
            if (count > 10){
                int start_lines = i++;
                double number_of_lines = ceil(count / 10.);
                for (int j(i); j < start_lines + number_of_lines ; j++, i++)
                    line += block[j];
                words.clear();
                istringstream iss2(line);
                copy(istream_iterator<string>(iss2), istream_iterator<string>(), back_inserter(words));
                i--;
            }
            for (size_t i(4); i < words.size(); i++) {
                sats->push_back(words[i]);
            }
        }
        shift.sats = sats;
        sats->clear();
        phase_shifts[sys].push_back(shift);
        shift.clear();
    }
    return phase_shifts;
}

// Extracts and stores the header information from Rinex v3 File
void Rinex3Obs::obsHeader(ifstream& infile) {
    // String tokens to look for
    const string sTokenVER = "RINEX VERSION / TYPE";
    const string sTokenPGM = "PGM / RUN BY / DATE";
    const string sTokenCOM = "COMMENT";

    const string sTokenMarkerNAME = "MARKER NAME";
    const string sTokenMarkerNUM = "MARKER NUMBER";
    const string sTokenMarkerTYPE = "MARKER TYPE ";

    const string sTokenObsAGENCY = "OBSERVER / AGENCY";
    const string sTokenObsRTV = "REC # / TYPE / VERS";

    const string sTokenANTTYPE = "ANT # / TYPE";
    const string sTokenPOS = "APPROX POSITION XYZ";
    const string sTokenDEL = "ANTENNA: DELTA H/E/N";
    const string sTokenDELXYZ = "ANTENNA: DELTA X/Y/Z";
    const string sTokenANTPHAS = "ANTENNA:PHASECENTER";
    const string sTokenANTSIGHTXYZ = "ANTENNA: B.SIGHT XYZ";
    const string sTokenANTZERODIRAZI = "ANTENNA: ZERODIR AZI";
    const string sTokenANTZERODIRXYZ = "ANTENNA: ZERODIR XYZ";

    const string sTokenCENOFMASSXYZ = "CENTER OF MASS: XYZ";
    const string sTokenOBS = "SYS / # / OBS TYPES";
    const string sTokenSIGSTRUNIT = "SIGNAL STRENGTH UNIT";
    const string sTokenINTERVAL = "INTERVAL";
    const string sTokenFIR = "TIME OF FIRST OBS";
    const string sTokenLAS = "TIME OF LAST OBS";
    const string sTokenRCV = "RCV CLOCK OFFS APPL";
    const string sTokenDCBSAPPL = "SYS / DCBS APPLIED";
    const string sTokenPCVSAPPL = "SYS / PCVS APPLIED";
    const string sTokenSCALEFACTOR = "SYS / SCALE FACTOR";
    const string sTokenPHASESH = "SYS / PHASE SHIFT";

    const string sTokenGLOSLOT = "GLONASS SLOT / FRQ";
    const string sTokenGLOSCPB = "GLONASS COD/PHS/BIS";


    const string sTokenLEAP = "LEAP SECONDS";
    const string sTokenEND = "END OF HEADER";

    //in the end
    //const string sTokenSAT = "OF SATELLITES";
    //const string sTokenPRNOFOBS = "PRN / # OF OBS";



    // A vector to hold block of sentences pertaining to observation types
    vector<string> types;
    vector<string> scale_factor;
    vector<string> phase_shifts;
    // To hold contents of a line from input file
    string line;

    // Reading line by line...
    while (!infile.eof()) {
        line.clear();
        // Temporarily store line from input file
        getline(infile, line, '\n');
        // Looking for keywords in Header Part...
        size_t found_VER = line.find(sTokenVER);
        size_t found_PGM = line.find(sTokenPGM);
        size_t found_COM = line.find(sTokenCOM);

        size_t found_MarkerNAME = line.find(sTokenMarkerNAME);
        size_t found_MarkerNUM = line.find(sTokenMarkerNUM);
        size_t found_MarkerTYPE = line.find(sTokenMarkerTYPE);

        size_t found_ObsAGENCY = line.find(sTokenObsAGENCY);
        size_t found_ObsRTV = line.find(sTokenObsRTV);

        size_t found_ANTTYPE = line.find(sTokenANTTYPE);
        size_t found_POS = line.find(sTokenPOS);
        size_t found_DEL = line.find(sTokenDEL);
        size_t found_DELXYZ = line.find(sTokenDELXYZ);
        size_t found_ANTPHAS = line.find(sTokenANTPHAS);
        size_t found_ANTSIGHTXYZ = line.find(sTokenANTSIGHTXYZ);
        size_t found_ANTZERODIRAZI = line.find(sTokenANTZERODIRAZI);
        size_t found_ANTZERODIRXYZ = line.find(sTokenANTZERODIRXYZ);


        size_t found_CENOFMASSXYZ = line.find(sTokenCENOFMASSXYZ);
        size_t found_OBS = line.find(sTokenOBS);
        size_t found_SIGSTRUNIT = line.find(sTokenSIGSTRUNIT);
        size_t found_INTERVAL = line.find(sTokenINTERVAL);
        size_t found_FIR = line.find(sTokenFIR);
        size_t found_LAS = line.find(sTokenLAS);
        size_t found_RCV = line.find(sTokenRCV);
        size_t found_DCBSAPPL = line.find(sTokenDCBSAPPL);
        size_t found_PCVSAPPL = line.find(sTokenPCVSAPPL);
        size_t found_SCALEFACTOR = line.find(sTokenSCALEFACTOR);
        size_t found_PHASESH = line.find(sTokenPHASESH);

        size_t found_GLOSLOT = line.find(sTokenGLOSLOT);
        size_t found_GLOSCPB = line.find(sTokenGLOSCPB);

        size_t found_LEAP = line.find(sTokenLEAP);
        size_t found_END = line.find(sTokenEND);

        //size_t found_SAT = line.find(sTokenSAT);
        //size_t found_PRNOFOBS = line.find(sTokenPRNOFOBS);


        if (found_VER != string::npos) {
            istringstream iss(line.substr(0, 60));
            // Rinex type should be stored in 4th word of line
            vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
            _Header.version = std::stod(words[0]);
            _Header.rinexType = words[words.size() - 1];
            words.clear();
            continue;
        }
        if (found_PGM != string::npos){
            line = line.substr(0, 60);
            string word;
            for (unsigned i = 0; i < line.length(); i += 20) {
                word = line.substr(i, 20);
                if (word.find_first_not_of(' ') == std::string::npos)
                    continue;
                word = regex_replace(word,regex{R"(^\s+|\s+$)"}, "");
                _Header.pmg.push_back(word);
                word.clear();
            }
            continue;
        }
        if (found_COM != string::npos) {
            line = regex_replace(line.substr(0,60), regex{R"(\s+$)"}, "");
            _Header.comments.push_back(line);
            continue;
        }

        if (found_MarkerNAME != string::npos){
            line = line.substr(0, 60);
            _Header.marker_name.append(regex_replace(line,regex{R"(^\s+|\s+$)"}, ""));
            continue;
        }
        if (found_MarkerNUM != string::npos){
            line = line.substr(0, 20);
            _Header.marker_num = regex_replace(line,regex{R"(^\s+|\s+$)"}, "");
            continue;
        }
        if (found_MarkerTYPE != string::npos){
            line = line.substr(0, 20);
            _Header.marker_type.append(regex_replace(line,regex{R"(^\s+|\s+$)"}, ""));
            continue;
        }

        if (found_ObsAGENCY != string::npos){
            string lineObserver = line.substr(0, 20);
            line = line.substr(20, 40);
            _Header.obs_agency.push_back(regex_replace(lineObserver,regex{R"(^\s+|\s+$)"}, ""));
            _Header.obs_agency.push_back(regex_replace(line,regex{R"(^\s+|\s+$)"}, ""));
            continue;
            ;}
        if (found_ObsRTV != string::npos){
            line = line.substr(0, 60);
            string word;
            for (size_t  i(0); i < line.length(); i += 20) {
                word = line.substr(i, 20);
                if (word.find_first_not_of(' ') == std::string::npos)
                    continue;
                word = regex_replace(word,regex{R"(^\s+|\s+$)"}, "");
                _Header.rec_type_vers.push_back(word);
                word.clear();
            }
            continue;
        }

        if (found_ANTTYPE != string::npos){
            line = line.substr(0, 40);
            string word;
            for (unsigned i = 0; i < line.length(); i += 20) {
                word = line.substr(i, 20);
                if (word.find_first_not_of(' ') == std::string::npos)
                    continue;
                word = regex_replace(word,regex{R"(^\s+|\s+$)"}, "");
                _Header.ant_type.push_back(word);
                word.clear();
            }
            continue;
        }
        if (found_POS != string::npos) {
            line = line.substr(0,60);
            istringstream iss(line);
            copy(istream_iterator<double>(iss),
                 istream_iterator<double>(),
                 back_inserter(_Header.approx_pos_xyz));
            continue;
        }
        if (found_DEL != string::npos) {
            line = line.substr(0,60);
            istringstream iss(line);
            copy(istream_iterator<double>(iss),
                 istream_iterator<double>(),
                 back_inserter(_Header.ant_delta_hen));
            continue;
        }
        if (found_DELXYZ != string::npos) {
            line = line.substr(0,60);
            istringstream iss(line);
            copy(istream_iterator<double>(iss),
                 istream_iterator<double>(),
                 back_inserter(_Header.ant_delta_xyz));
            continue;
        }
        if (found_ANTPHAS != string::npos) {
            line = line.substr(0,60);
            string sys = line.substr(0,1);
            string code = line.substr(2,3);
            map<string, vector<double>> data{make_pair(code,vector<double>{})};
            istringstream iss(line.substr(5,55));
            copy(istream_iterator<double>(iss),
                 istream_iterator<double>(),
                 back_inserter(data[code]));
            _Header.ant_phasecenter.insert(make_pair(sys, data));
            continue;
        }
        if (found_ANTSIGHTXYZ != string::npos) {
            line = line.substr(0,60);
            istringstream iss(line);
            copy(istream_iterator<double>(iss),
                 istream_iterator<double>(),
                 back_inserter(_Header.ant_sight_xyz));
            continue;
        }
        if (found_ANTZERODIRAZI != string::npos) {
            line = line.substr(0,14);
            _Header.ant_zerodir_azi = std::stod(line);
            continue;
        }
        if (found_ANTZERODIRXYZ != string::npos) {
            line = line.substr(0,60);
            istringstream iss(line);
            copy(istream_iterator<double>(iss),
                 istream_iterator<double>(),
                 back_inserter(_Header.ant_zerodir_xyz));
            continue;
        }

        if (found_CENOFMASSXYZ != string::npos) {
            line = line.substr(0,60);
            istringstream iss(line);
            copy(istream_iterator<double>(iss),
                 istream_iterator<double>(),
                 back_inserter(_Header.center_of_mass_xyz));
            continue;
        }
        if (found_OBS != string::npos) {
            // Removing identifier from block of lines
            eraseSubStr(line, sTokenOBS);
            // Storing in vector for processing later
            types.push_back(line);
            continue;
        }
        if (found_SIGSTRUNIT != string::npos) {
            line = line.substr(0,20);
            _Header.sig_str_unit = regex_replace(line,regex{R"(\s+$)"}, "");
            continue;
        }
        if (found_INTERVAL != string::npos) {
            line = line.substr(0,10);
            _Header.interval = std::stod(line);
            continue;
        }
        if (found_FIR != string::npos) {
            istringstream iss(line);
            copy(istream_iterator<double>(iss),
                 istream_iterator<double>(),
                 back_inserter(_Header.first_obs_time));
            continue;
        }
        if (found_LAS != string::npos) {
            istringstream iss(line);
            copy(istream_iterator<double>(iss),
                 istream_iterator<double>(),
                 back_inserter(_Header.last_obs_time));
            continue;
        }
        if (found_RCV != string::npos) {
            line = line.substr(0,6);
            _Header.rcv_clock = std::stoi(line);
            continue;
        }
        if (found_DCBSAPPL != string::npos) {
            istringstream iss(line.substr(0, 60));
            copy(istream_iterator<string>(iss),
                 istream_iterator<string>(),
                 back_inserter(_Header.dcbs_appl));
            continue;
        }
        if (found_PCVSAPPL != string::npos) {
            istringstream iss(line.substr(0, 60));
            copy(istream_iterator<string>(iss),
                 istream_iterator<string>(),
                 back_inserter(_Header.pcvs_appl));
            continue;
        }
        if (found_SCALEFACTOR != string::npos) {
            scale_factor.push_back(line.substr(0,60));
            continue;
        }
        if (found_PHASESH != string::npos) {
            phase_shifts.push_back(line.substr(0, 60));
            continue;
        }

        if (found_GLOSLOT != string::npos) {
            line = line.substr(0, 60);
            int size = std::stoi(line.substr(0, 3));
            while(size != 0){
                istringstream iss(line.substr(4, 56));
                vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
                for (std::size_t i = 0; i < words.size(); i += 2){
                    _Header.glonass_slot.insert(std::pair<string, int>(words[i], stoi(words[i+1])));
                    size--;
                }
                if (size == 0)
                    break;
                line.clear();
                getline(infile, line ,'\n');
                line = line.substr(0, 60);
            }
            continue;
        }
        if (found_GLOSCPB != string::npos) {
            line = line.substr(0, 60);
            istringstream iss(line);
            vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
            for (std::size_t i = 0; i < words.size(); i += 2){
                _Header.glonass_cpd.insert(std::pair<string, double>(words[i], stod(words[i+1])));
            }
            continue;
        }


        if (found_LEAP != string::npos) {
            line = line.substr(0, 24);
            istringstream iss(line);
            copy(istream_iterator<int>(iss),
                 istream_iterator<int>(),
                 back_inserter(_Header.leap_seconds));
            continue;
        }
        if (found_END != string::npos) {
            break;
        }
    }

    // Organizing the observation types
    _Header.obsTypes = obsTypesHeader(types);
    if(!scale_factor.empty())
        _Header.scale_factor = obsScaleFactorHeader(scale_factor);
    if(!phase_shifts.empty())
        _Header.phase_shifts = obsPhaseShiftsHeader(phase_shifts);


    if (_Header.obsTypes.count("G") > 0) { _obsTypesGPS = _Header.obsTypes["G"]; }
    if (_Header.obsTypes.count("R") > 0) { _obsTypesGLO = _Header.obsTypes["R"]; }
    if (_Header.obsTypes.count("E") > 0) { _obsTypesGAL = _Header.obsTypes["E"]; }
    if (_Header.obsTypes.count("C") > 0) { _obsTypesBEI = _Header.obsTypes["C"]; }
}
//=======================================================================================


//================================Body===================================================
// Splits Epoch Information
vector<double> rinex3EpochRecordOrganizer(string line) {
    vector<double> epochRecord;
    // Splitting words in the line
    istringstream iss(line);
    vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
    for (const string &s : words) {
        epochRecord.push_back(stod(s));
    }
    return epochRecord;
}

// Epoch Satellite Observation Data Organizer
void rinex3SatObsOrganizer(string line, map<string, map<int, vector<double>>>& data) {
    // First word contains satellite system and number
    string sys = line.substr(0, 1);
    int prn = stoi(line.substr(1, 2));
    // Rest of the words should contain observations
    // Obs format is 14.3, 14 for Obs and 3 for S/N
    line = line.substr(3, line.length());
    string word;
    vector<double> obs;
    for (unsigned i = 0; i < line.length(); i += 16) {
        word = line.substr(i, 16);
        if (word.find_first_not_of(' ') == string::npos) {
            word = "0";
        }
        obs.push_back(stod(word));
        word.clear();
    }
    // Update map data
    data[sys].insert(make_pair(prn, obs));
}

// This function is used to organize the string block of epoch info into data structure
void obsOrganizer(vector<string> block, Rinex3Obs::ObsEpochInfo& obs) {
    // First line contains epoch time information and receiver clock offset
    obs.epochRecord = rinex3EpochRecordOrganizer(block[0]);
    obs.epochRecord.size() > 8 ? obs.recClockOffset = obs.epochRecord.back(): obs.recClockOffset = std::nullopt;
    // Organize satellite observations in data structure
    map<string, map<int, vector<double>>> data;
    string line;
    for (unsigned int i = 1; i < block.size(); i++) {
        line.clear(); line = block[i];
        rinex3SatObsOrganizer(line, data);
    }
    obs.observations = data;
    obs.numSatsGAL = 0;
    obs.numSatsGLO = 0;
    obs.numSatsGPS = 0;
    obs.numSatsBEI = 0;
    map<string, map<int, vector<double>>>::iterator it;
    //Galileo
    it = data.find("E");
    if (it != data.end())
        obs.numSatsGAL = static_cast<int>(it->second.size());

    //Glonass
    it = data.find("R");
    if (it != data.end())
        obs.numSatsGLO = static_cast<int>(it->second.size());

    //Gps
    it = data.find("G");
    if (it != data.end())
        obs.numSatsGPS = static_cast<int>(it->second.size());

    //BeiDou
    it = data.find("C");
    if (it != data.end())
        obs.numSatsBEI = static_cast<int>(it->second.size());
}

// Setting observation attributes for each satellite constellations
void Rinex3Obs::setObservations(map<string, map<int, vector<double>>> observations) {
    if (observations.count("G") > 0) { _obsGPS.clear(); _obsGPS = observations["G"]; }
    if (observations.count("R") > 0) { _obsGLO.clear(); _obsGLO = observations["R"]; }
    if (observations.count("E") > 0) { _obsGAL.clear(); _obsGAL = observations["E"]; }
    if (observations.count("C") > 0) { _obsBEI.clear(); _obsBEI = observations["C"]; }
}

// This function extracts and stores epochwise observations from file
bool Rinex3Obs::obsEpoch(ifstream& infile) {
    // Rinex v3 special identifier for new epoch of observations
    const string sTokenEpoch = ">";
    const string sTokenSAT = "OF SATELLITES";
    // Collect the block of observation lines into a vector
    int nSatsEpoch = 0; int nLinesEpoch = 0; vector<int> blockFirstLine;
    streampos pos;
    string line;
    vector<string> block;
    // Reading line by line...
    while (!(infile >> std::ws).eof()) {
        // *** Deal with end of file error
        if (infile.fail()) { break; }
        // ***
        line.clear();
        pos = infile.tellg();
        // Temporarily store line from input file
        getline(infile, line); nLinesEpoch++;

        if (line.find_first_not_of(' ') == string::npos) { continue; }

        // we not read additional information at the end of the file
        size_t found_SAT = line.find(sTokenSAT);
        if ( found_SAT != string::npos ){
            infile.clear();
            infile.seekg(0,infile.end);
            return false;
        }

        // Look for special identifier in line
        size_t found_ID = line.find(sTokenEpoch);
        if ((found_ID != string::npos)) {
            if (block.size() == 0) {
                eraseSubStr(line, sTokenEpoch);
                // Find number of sats in epoch
                istringstream iss(line);
                vector<string> words{ istream_iterator<string>{iss}, istream_iterator<string>{} };
                for (string &s : words) {
                    blockFirstLine.push_back(stoi(s));
                }
                nSatsEpoch = blockFirstLine[7];
            }
            else {
                infile.seekg(pos);
                break;
            }
        }
        block.push_back(line);
        // Fail-safe epoch quitter
        if (nLinesEpoch == nSatsEpoch+1) { break; }
    }
    // Now we must process the block of lines
    _EpochObs.clear();
    obsOrganizer(block, _EpochObs);
    _EpochObs.gpsTime = gpsTime(_EpochObs.epochRecord);
    // Update observation attributes
    //setObservations(_EpochObs.observations);
    return true;
}
//=======================================================================================


//============================Clear======================================================
// To clear contents in observation data structure
void Rinex3Obs::clear(Rinex3Obs::ObsEpochInfo& obs) {
    obs.clear();
    obs.gpsTime = 0;
}

// To clear contents in observation data structure
void Rinex3Obs::clear(Rinex3Obs::ObsHeaderInfo& header) {

    header.ant_delta_hen.clear();
    header.approx_pos_xyz.clear();
    header.first_obs_time.clear();
    header.last_obs_time.clear();
    header.obsTypes.clear();
    header.rinexType.clear();
}
//=======================================================================================

void Rinex3Obs::ObsEpochInfo::clear()
{
    epochRecord.clear();
    numSatsGAL = 0;
    numSatsGLO = 0;
    numSatsGPS = 0;
    numSatsBEI = 0;
    observations.clear();
    recClockOffset = 0;
}

void Rinex3Obs::ObsHeaderInfo::ScaleFactor::clear()
{
    this->scale = 0;
    this->types.clear();
    this->types.clear();
}

void Rinex3Obs::ObsHeaderInfo::PhaseShifts::clear()
{
    this->code.clear();
    if (sats.has_value())
        sats->clear();
    this->shift_correction = nullopt;
}
