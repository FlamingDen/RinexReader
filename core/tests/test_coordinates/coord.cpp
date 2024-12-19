#include <iostream>

#include "pdp.h"
#include "condb.h"


int main()
{
    rr::ConDb* db = rr::ConDb::GetInstance();

    QHash<QString, rr::Pdp> pdps = db->GetPdpAll();
    int i(1);
    for(auto it = std::begin(pdps); it != std::end(pdps); ++it){
        std::cout.width(2);
        std::cout << i++ << " - " << it->ToStringShort() << std::endl;
    }    
    std::cout<< std::boolalpha << db->IsExist("BARA") << std::endl;
    
}
