#ifndef IREADING_H
#define IREADING_H

#include <fstream>

class IReading
{
protected:
    ~IReading() = default;
public:
    virtual void readHead(std::ifstream& infile) = 0;
    virtual void readBody(std::ifstream& infile) = 0;
};

#endif // IREADING_H
