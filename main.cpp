#include <iostream>
#include "DS9490R.h"
#include "SmsModem.h"
#include <fstream>

void SaveHotelParams(std::string& pathToFile, std::string& telNumber, int& minStreetT, int& minRoomT, bool& monitor){
    std::ofstream firstHostelParam(pathToFile);
    if(firstHostelParam.is_open()){
        std::string record = "tel=" + telNumber;
        firstHostelParam << record;
        firstHostelParam << std::endl;
        record = "minStreetT=" + std::to_string(minStreetT);
        firstHostelParam << record;
        firstHostelParam << std::endl;
        record = "minRoomT=" + std::to_string(minRoomT);
        firstHostelParam << record;
        firstHostelParam << std::endl;
        record = "monitor=";
        if(monitor) record += "start";
        else record += "stop";
        firstHostelParam << record;
        firstHostelParam << std::endl;
        firstHostelParam.close();
    }
}

int main() {
    //int nb = DS9490R::CreateParamFile("/home/kvitadmin/tmp/param_file.txt");
    //std::cout << "Sensors number is " << nb << std::endl;
    std::vector<std::string> messages;
    int minStreetT = -5, minRoomT = 5;
    std::string telNumber = "+79283216509";
    std::string pathToModemDir ="/home/kvitadmin/gammu/spool/gammu";
    bool monitor = true;
    std::string hostelParam = "/home/kvitadmin/tmp/hostel_param.txt";
    std::map<std::string, float> masTemp;
    int diffAlarm = 2;
    std::map<std::string , int> masRooms;
    std::ifstream paramHostelFile(hostelParam);
    if(!paramHostelFile.is_open()){
        SaveHotelParams(hostelParam,telNumber,minStreetT,minRoomT,monitor);
    }
    else{
        std::string record;
        for(int i = 0; i < 3; ++i){
            paramHostelFile >> record;
            if(record.find("tel=") != std::string::npos){
                telNumber=record.substr(4,12);
            }
            else if(record.find("minStreetT=") != std::string::npos){
                minStreetT = std::stoi(record.substr(11,3));
            }
            else if(record.find("minRoomT=") != std::string::npos){
                minRoomT = std::stoi(record.substr(9,3));
            }
            else if(record.find("monitor=") != std::string::npos){
                if(record.find("start") != std::string::npos) monitor = true;
                else if(record.find("stop") != std::string::npos) monitor = false;
            }
        }
    }
    DS9490R myTempSensors("/home/kvitadmin/tmp/param_file.txt");
    std::vector<std::string> roomNames = myTempSensors.GetCustomSensorsName();
    for(const auto& el : roomNames){
        masRooms.insert(std::make_pair(el, 0));
    }
    SmsModem myModem(pathToModemDir);
    while(1){
        messages = myModem.GetIncomingMessages(telNumber);
        if(!messages.empty()){
            std::string tmpstring;
            for(const auto& el : messages){
                if(el.find("tel=") != std::string::npos){
                    telNumber=el.substr(4,12);
                    tmpstring = "Ok! tel=" + el.substr(4,12);
                    myModem.sendSmsMessage(telNumber,tmpstring);
                }
                else if(el.find("ul=") != std::string::npos){
                    minStreetT = std::stoi(el.substr(3,3));
                    tmpstring = "Ok! ul=" + el.substr(3,3);
                    myModem.sendSmsMessage(telNumber,tmpstring);
                }
                else if(el.find("t=") != std::string::npos){
                    minRoomT = std::stoi(el.substr(2,3));
                    tmpstring = "Ok! t=" + el.substr(2,3);
                    myModem.sendSmsMessage(telNumber,tmpstring);
                }
                else if(el.find("start") != std::string::npos){
                    monitor = true;
                    tmpstring = "Ok! monitor = start";
                    myModem.sendSmsMessage(telNumber,tmpstring);
                }
                else if(el.find("stop") != std::string::npos){
                    monitor = false;
                    tmpstring = "Ok! monitor = stop";
                    myModem.sendSmsMessage(telNumber,tmpstring);
                }
                else if(el.find("temp") != std::string::npos){
                    masTemp = std::move(myTempSensors.GetSensorsTemperature());
                    std::string tmpString;
                    if(!masTemp.empty()){

                        for(const auto& el : masTemp) {
                            tmpString+= el.first + " ";
                            tmpString += std::to_string(static_cast<int>(el.second));
                            tmpString += "\n";
                        }
                    }
                    tmpString += "tel=";
                    tmpString += telNumber;
                    tmpString += "\nul=";
                    tmpString += std::to_string(minStreetT);
                    tmpString += "\nt=";
                    tmpString += std::to_string(minRoomT);
                    tmpString += "\nmonitor=";
                    if(monitor) tmpString += "start";
                    else tmpString += "stop";
                    myModem.sendSmsMessage(telNumber,tmpString);
                }
            }
            SaveHotelParams(hostelParam,telNumber,minStreetT,minRoomT,monitor);
            std::string tmp = pathToModemDir + "/inbox";
            myModem.DeleteMessagesFromDir(tmp);
        }

        if(monitor){
            masTemp = std::move(myTempSensors.GetSensorsTemperature());
            if(!masTemp.empty()){
                for(const auto& el : masTemp) {
                    //std::cout << el.first << "--" << el.second << std::endl;
                    if(el.first == "ul"){
                        if((static_cast<int>(el.second) <= minStreetT) && masRooms.find(el.first)->second == 0){
                            masRooms.find(el.first)->second = 1;//надо отправить сообщение об аварии
                        }
                        else if((static_cast<int>(el.second) > (minStreetT + diffAlarm)) && masRooms.find(el.first)->second == 2){
                            masRooms.find(el.first)->second = 3;//надо отправить сообщение об отмене аварии
                        }
                    }
                    else {
                        if((static_cast<int>(el.second) <= minRoomT) && masRooms.find(el.first)->second == 0){
                            masRooms.find(el.first)->second = 1;//надо отправить сообщение об аварии
                        }
                        else if((static_cast<int>(el.second) > (minRoomT + diffAlarm)) && masRooms.find(el.first)->second == 2){
                            masRooms.find(el.first)->second = 3;//надо отправить сообщение об отмене аварии
                        }
                    }
                }
            }
            masTemp.clear();
            ////////////////////////////////
            std::map<std::string,int>::iterator el;
            for( el = masRooms.begin(); el != masRooms.end(); el++){
                if(el->second == 1){
                    std::string mes = "Alarm! " + el->first + " Temperature = " + std::to_string(static_cast<int>(masTemp.find(el->first)->second));
                    if(myModem.sendSmsMessage(telNumber,mes )){
                        el->second = 2;
                    }

                }
                else if(el->second == 3){
                    std::string mes = "Alarm canceled! " + el->first + " Temperature = " + std::to_string(static_cast<int>(masTemp.find(el->first)->second));
                    if(myModem.sendSmsMessage(telNumber,mes )){
                        el->second = 0;
                    }
                }
            }
        }
    }
        return 0;
}
