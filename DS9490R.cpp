//
// Created by kvitadmin on 29.10.2021.
//

#include "DS9490R.h"
#include <vector>

DS9490R::DS9490R(const std::string& fileName) {
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
        if(OW_init("localhost:4304") != 0) error = true;
    }

}
std::vector<std::string> DS9490R::GetCustomSensorsName(){
    std::vector<std::string> customSensName;
    for(const auto& el : sensorsName){
        customSensName.emplace_back(el.second);
    }
    return customSensName;
}
DS9490R::~DS9490R() {
    OW_finish();
}

int DS9490R::CreateParamFile(const std::string& fullFileName){

    if(error) return 0;
    std::vector<std::string> sensorsName;
    std::ofstream ParamFile(fullFileName);
    if(ParamFile){
        char* buf;
        size_t s ;
        int nb = OW_get("/",&buf,&s);
        std::string allSensors(buf);
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
        if(!sensorsName.empty()){
            for(const auto& el : sensorsName){
                ParamFile << el << std::endl;
            }
        }
        ParamFile.close();
    }
    return sensorsName.size();
}

std::map<std::string, float> DS9490R::GetSensorsTemperature(){
    std::map<std::string, float> result;
    if(error) return result;
    std::string tmpName;
    char* buf = NULL;
    size_t s ;
    if(sensorsName.size() != 0){
        std::map<std::string, std::string>::iterator it;

        for(it = sensorsName.begin(); it != sensorsName.end(); ++it){
            tmpName = it->first + "/temperature9";
            int err = OW_get(tmpName.c_str(),&buf,&s);
            if(err != -1){
                float temp = static_cast<float>(std::atof(buf));
                result.insert(std::make_pair(it->second, temp));
            }
            free(buf);
        }
    }


    return result;
}