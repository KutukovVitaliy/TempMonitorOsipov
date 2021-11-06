//
// Created by kvitadmin on 29.10.2021.
//

#ifndef TEMPMONITOROSIPOV_DS9490R_H
#define TEMPMONITOROSIPOV_DS9490R_H
#include "owcapi.h"
#include <string>
#include <fstream>
#include <map>
#include <vector>

class DS9490R {
    std::string paramFileName;
    std::map<std::string,std::string> sensorsName;

public:
    static int CreateParamFile(std::string fullFileName);
    std::map<std::string, float> GetSensorsTemperature();
    std::vector<std::string> GetCustomSensorsName();
    DS9490R(std::string fileName);
    ~DS9490R();
};


#endif //TEMPMONITOROSIPOV_DS9490R_H
