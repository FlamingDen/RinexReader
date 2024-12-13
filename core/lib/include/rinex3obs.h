#pragma once
#ifndef RINEX3OBS_H_
#define RINEX3OBS_H_

#include "pch.h"
#include <optional>

#include "ireading.h"

namespace rr {
class Rinex3Obs : IReading
{
public:
    Rinex3Obs();
    ~Rinex3Obs();

    // DATA STRUCTURES
    // To store important header information
    struct ObsHeaderInfo
    {

        struct ScaleFactor
        {
            std::string sat_system;
            int scale;
            std::vector<std::string> types;

            void clear();
        };

        struct PhaseShifts
        {
            std::string code;
            std::optional<double> shift_correction;
            std::optional<std::vector<std::string>> sats;

            void clear();
        };

        double version;
        std::string rinexType;
        std::vector<std::string> pmg;
        std::vector<std::string> comments;

        std::string marker_name = "";
        std::optional<std::string> marker_num;
        std::string marker_type = "GEODETIC/NON_GEODETIC";

        std::vector<std::string> obs_agency;
        std::vector<std::string> rec_type_vers;

        std::vector<std::string> ant_type;
        std::vector<double> approx_pos_xyz;
        std::vector<double> ant_delta_hen;
        std::vector<double> ant_delta_xyz;
        std::map<std::string, std::map<std::string, std::vector<double>>> ant_phasecenter;
        std::vector<double> ant_sight_xyz;
        std::optional<double> ant_zerodir_azi;
        std::vector<double> ant_zerodir_xyz;

        std::vector<double> center_of_mass_xyz;
        std::map<std::string, std::vector<std::string>> obsTypes;
        std::optional<std::string> sig_str_unit;
        std::optional<double> interval;
        std::vector<double> first_obs_time;
        std::vector<double> last_obs_time;
        int rcv_clock = 0;
        std::vector<std::string> dcbs_appl;
        std::vector<std::string> pcvs_appl;
        std::vector<ScaleFactor> scale_factor;
        std::map<std::string, std::vector<PhaseShifts>> phase_shifts;

        std::map<std::string, int> glonass_slot;
        std::map<std::string, double> glonass_cpd;
        std::vector<int> leap_seconds;
    };
    // To store observations in an epoch
    struct ObsEpochInfo
    {
        std::vector<double> epochRecord;
        std::optional<double> recClockOffset;
        double gpsTime;
        int numSatsGPS;
        int numSatsGLO;
        int numSatsGAL;
        int numSatsBEI;
        std::map<std::string, std::map<int, std::vector<double>>> observations;

        void clear();
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
    void readHead(std::ifstream &infile) override;
    void readBody(std::ifstream &infile) override;

    void clear(Rinex3Obs::ObsEpochInfo &obs);
    void clear(Rinex3Obs::ObsHeaderInfo &header);
    void setObservations(std::map<std::string, std::map<int, std::vector<double>>> observations);
    std::map<int, double> specificObsMapper(std::map<int, std::vector<double>> obsGPS, std::vector<std::string> obsTypes, std::string specificObs);

private:
    std::vector<Rinex3Obs::ObsHeaderInfo::ScaleFactor> obsScaleFactorHeader(std::vector<std::string> block);
    std::map<std::string, std::vector<Rinex3Obs::ObsHeaderInfo::PhaseShifts>> obsPhaseShiftsHeader(std::vector<std::string> block);
};
}
#endif // RINEX3OBS_H
