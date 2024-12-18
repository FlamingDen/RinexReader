﻿#pragma once

#include <ostream>
#include <sstream>
#include <string>
#include <stdexcept>

namespace rr{
    struct Coordinates{
        double x;
        double y;
        double z;
    };


    class Pdp{
    private:
        int id;
        std::string name;
        std::string identifier_name;
        Coordinates coordinates;

        const static int SIZE_IDENTIFIER_NAME_PDP{4};

        bool IsValid(int id_,
                    std::string name_,
                    std::string identifier_name_,
                    Coordinates coordinates_);

    public:
        Pdp();

        Pdp(int id_,
            std::string name_,
            std::string identifier_name_,
            Coordinates coordinates_);

        const int GetId() const;

        const std::string& GetName() const;

        const std::string& GetIdentifierName() const;

        const Coordinates& GetCoordinates() const;

        const static int GetSizeIdentifireNamePDP();


        void SetId(const int& id_);

        void SetName(const std::string& name_);

        void SetIdentifierName(const std::string& identifier_name_);

        void SetCoordinates(const Coordinates& coord_);

        void SetCoordinates(const std::tuple<double, double, double>&  coord_);


        const std::string ToString() const;

        const std::string ToStringShort() const;
    };

    std::ostream& operator<<(std::ostream& out, Pdp pdp);
}