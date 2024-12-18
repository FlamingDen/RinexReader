#pragma once

#include <iostream>
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
        Pdp(int id_,
            std::string name_,
            std::string identifier_name_,
            Coordinates coordinates_);

        const int GetId() const;

        const std::string& GetName() const;

        const std::string& GetIdentifierName() const;

        const Coordinates& GetCoordinates() const;

        const static int GetSizeIdentifireNamePDP();
    };

    std::ostream& operator<<(std::ostream& out, Pdp pdp);
}