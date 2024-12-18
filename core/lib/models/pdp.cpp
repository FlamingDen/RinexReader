#include "pdp.h"

bool rr::Pdp::IsValid(
    int id_,
    std::string name_,
    std::string identifier_name_, 
    Coordinates coordinates_)
{
    if (identifier_name_.size() != SIZE_IDENTIFIER_NAME_PDP)
        throw std::invalid_argument("Size of identifier name PDP aren't 4");
    return true;
}

rr::Pdp::Pdp(
    int id_, 
    std::string name_, 
    std::string identifier_name_, 
    Coordinates coordinates_)
{
    if(IsValid(id_, name_, identifier_name_, coordinates_)){
        id = id_;
        name = name_;
        identifier_name = identifier_name_;
        coordinates = coordinates_;
    }
}

const int rr::Pdp::GetId() const
{
    return id;
}

const std::string &rr::Pdp::GetName() const
{
   return name;
}

const std::string &rr::Pdp::GetIdentifierName() const
{
    return identifier_name;
}

const rr::Coordinates &rr::Pdp::GetCoordinates() const
{
    return coordinates;
}

const int rr::Pdp::GetSizeIdentifireNamePDP()
{
    return rr::Pdp::SIZE_IDENTIFIER_NAME_PDP;
}

std::ostream& rr::operator<<(std::ostream &out, rr::Pdp pdp)
{
    out.width(18);
    out << "Id : " << pdp.GetId() << "\n";

    out.width(18);
    out << "Name : " << pdp.GetName() << "\n";

    out.width(18);
    out << "Identifire name : " << pdp.GetIdentifierName() << "\n";

    out.width(18);
    rr::Coordinates c = pdp.GetCoordinates();
    out << "Coordinates : " << "{"<< c.x << ", " << c.y << ", " << c.z << "}" << "\n";
    return out;
}


