#pragma once

#include "pch.h"
#include "timeutils.h"
#include "stringutils.h"
#include <optional>

#ifndef RINEX3OBS_H_
#define RINEX3OBS_H_

class Rinex3Obs
{
public:
    Rinex3Obs();
    ~Rinex3Obs();

    // DATA STRUCTURES
    // To store important header information
    struct ObsHeaderInfo {
        float version;
        std::string rinexType;
        std::vector<std::string> pmg;

        std::string marker_name = "";
        std::string marker_num = "";
        std::string marker_type = "";

        std::vector<std::string> obs_agency;
        std::vector<std::string> rec_type_vers;

        std::vector<std::string> ant_type;
        std::vector<float> approx_pos_xyz;
        std::vector<float> ant_delta_hen;
        std::vector<float> ant_delta_xyz;
        std::vector<float> ant_phasecenter;
        std::vector<float> ant_sight_xyz;
        float ant_zerodir_azi;
        std::vector<float> ant_zerodir_xyz;

        std::vector<float> center_of_mass_xyz;
        std::string sig_str_unit;
        float interval;
        bool rcv_clocl = 0;
        std::vector<float> dcbs_appl;
        std::vector<float> pcvs_appl;
        std::vector<float> scale_factor;
        std::map<std::string, std::vector<float>> phase_shift;

        std::map<std::string, int> glonass_slot;
        std::vector<int> leap_seconds;

        std::vector<double> first_obs_time;
        std::vector<double> last_obs_time;
        std::map<std::string, std::vector<std::string>> obsTypes;
    };
    // To store observations in an epoch
    struct ObsEpochInfo {
        std::vector<double> epochRecord;
        std::optional<double> recClockOffset;
        double gpsTime;
        int numSatsGPS;
        int numSatsGLO;
        int numSatsGAL;
        int numSatsBEI;
        std::map<std::string, std::map<int, std::vector<double>>> observations;
        void clear() {
            epochRecord.clear();
            numSatsGAL = 0;
            numSatsGLO = 0;
            numSatsGPS = 0;
            numSatsBEI = 0;
            observations.clear();
            recClockOffset = 0;
        }
    };

    // Attributes
    Rinex3Obs::ObsHeaderInfo _Header;
    Rinex3Obs::ObsEpochInfo _EpochObs;

    // * Epoch observations mapped to PRN for ease of use
    std::map<int, std::vector<double>> _obsGPS;
    std::map<int, std::vector<double>> _obsGLO;
    std::map<int, std::vector<double>> _obsGAL;
    std::map<int, std::vector<double>> _obsBEI;

    // * Available observation types (C1C, L1C,...)
    std::vector<std::string> _obsTypesGPS;
    std::vector<std::string> _obsTypesGLO;
    std::vector<std::string> _obsTypesGAL;
    std::vector<std::string> _obsTypesBEI;

    // Functions
    void obsHeader(std::ifstream& infile);
    bool obsEpoch(std::ifstream& infile);
    void clear(Rinex3Obs::ObsEpochInfo& obs);
    void clear(Rinex3Obs::ObsHeaderInfo& header);
    void setObservations(std::map<std::string, std::map<int, std::vector<double>>> observations);
    std::map<int, double> specificObsMapper(std::map<int, std::vector<double>> obsGPS, std::vector<std::string> obsTypes, std::string specificObs);
};

#endif // RINEX3OBS_H
