//
// Created by kvitadmin on 29.10.2021.
//

#include "DS9490R.h"
#include <vector>

DS9490R::DS9490R(std::string fileName) {
    paramFileName = fileName;
    std::string OwSensorName;
    std::string CustomSensorName;

    std::ifstream paramFile;
    paramFile.open(fileName, std::ios::in);
    if(paramFile.is_open()){
        while (!paramFile.eof()){
            paramFile >> OwSensorName;
            paramFile >> CustomSensorName;
            sensorsName.insert(std::make_pair(OwSensorName, CustomSensorName));
        }
        paramFile.close();
    }

}
std::vector<std::string> DS9490R::GetCustomSensorsName(){
    std::vector<std::string> customSensName;
    for(auto el : sensorsName){
        customSensName.emplace_back(el.second);
    }
    return customSensName;
}
DS9490R::~DS9490R() {

}
int DS9490R::CreateParamFile(std::string fullFileName){

    std::vector<std::string> sensorsName;
    std::ofstream ParamFile(fullFileName);
    if(ParamFile){
        char* buf;
        size_t s ;

        OW_init("u#");
        OW_set_error_print("2");
        int nb = OW_get("/",&buf,&s);
        std::string allSensors(buf);
        OW_finish();
        std::size_t nextPos = 0;
        bool next = true;
        do{
            std::size_t pos = allSensors.find("/", nextPos);
            if(pos != std::string::npos){
                if(allSensors.at(pos - 15)=='2' && allSensors.at(pos - 14) == '8' && allSensors.at(pos - 13) == '.'){
                    sensorsName.push_back(allSensors.substr(pos-15,15));
                    nextPos = pos + 1;
                }
                else next = false;
            }
        }while(next);
        if(sensorsName.size() > 0){
            for(auto el : sensorsName){
                ParamFile << el << std::endl;
            }
        }
        ParamFile.close();
    }
    return sensorsName.size();
}

std::map<std::string, float> DS9490R::GetSensorsTemperature(){
    std::map<std::string, float> result;
    std::string tmpName;
    char* buf;
    size_t s ;
    if(sensorsName.size() != 0){
        std::map<std::string, std::string>::iterator it;
        if(OW_init("u") != 0) return result;
        OW_set_error_print("2");
        for(it = sensorsName.begin(); it != sensorsName.end(); ++it){
            tmpName = it->first + "/temperature9";
            OW_get(tmpName.c_str(),&buf,&s);
            if(s > 0){
                float temp = std::atof(buf);
                result.insert(std::make_pair(it->second, temp));
            }
        }
    }
    OW_finish();
    return result;
}