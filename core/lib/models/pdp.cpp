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

rr::Pdp::Pdp(){}

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

void rr::Pdp::SetId(const int &id_)
{
    this->id = id_;
}

void rr::Pdp::SetName(const std::string &name_)
{
    this->name = name_;
}

void rr::Pdp::SetIdentifierName(const std::string &identifier_name_)
{  
    this->identifier_name = identifier_name_;
}

void rr::Pdp::SetCoordinates(const Coordinates &coord_)
{
    this->coordinates = coord_;
}

void rr::Pdp::SetCoordinates(const std::tuple<double, double, double> &coord_)
{
    this->coordinates = {
        .x = std::get<0>(coord_),
        .y = std::get<1>(coord_),
        .z = std::get<2>(coord_)
        };
}

const std::string rr::Pdp::ToString() const
{
    std::ostringstream ostr;
    ostr.width(18);
    ostr << "Id : " << GetId() << "\n";

    ostr.width(18);
    ostr << "Name : " << GetName() << "\n";

    ostr.width(18);
    ostr << "Identifire name : " << GetIdentifierName() << "\n";

    ostr.width(18);
    rr::Coordinates c = GetCoordinates();
    ostr << "Coordinates : " << "{" << c.x << ", " << c.y << ", " << c.z << "}";
    return ostr.str();
}

const std::string rr::Pdp::ToStringShort() const
{
    std::ostringstream ostr;
    ostr << "{Id : " << GetId() << " | ";

    ostr.width(18);
    ostr << "Identifire name : " << GetIdentifierName() << "}";

    return ostr.str();
}

//=======================================================================================
std::ostream& rr::operator<<(std::ostream &out, rr::Pdp pdp)
{
    out << pdp.ToString();
    return out;
}


