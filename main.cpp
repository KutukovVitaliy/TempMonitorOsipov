#include <iostream>
#include "DS9490R.h"

int main() {
    //int nb = DS9490R::CreateParamFile("/home/kvitadmin/tmp/param_file.txt");
    //std::cout << "Sensors number is " << nb << std::endl;
    std::map<std::string, float> masTemp;
    DS9490R myTempSensors("/home/kvitadmin/tmp/param_file.txt");
    masTemp = myTempSensors.GetSensorsTemperature();
    if(masTemp.size() > 0){
        std::map<std::string, float>::iterator it;
        for(auto el : masTemp) std::cout << el.first << "--" << el.second << std::endl;
    }
    return 0;
}
