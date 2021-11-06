//
// Created by kvitadmin on 03.11.2021.
//

#include "SmsModem.h"
#include <iostream>

std::vector<std::string> SmsModem::GetFilesList(std::string &fullPathName){
    DIR *d;
    struct dirent *dir;
    std::vector<std::string> fileNames;

    d = opendir(fullPathName.c_str());
    if(d){
        while((dir = readdir(d)) != nullptr){
            fileNames.emplace_back(dir->d_name);
        }
        closedir(d);
    }

    std::vector<std::string>::iterator it;
    for(it = fileNames.begin(); it < fileNames.end(); ++it){
        if(it->c_str()[0] == '.') {
            fileNames.erase(it);
            if(!fileNames.empty())it--;
        }
    }
    return fileNames;
}

std::vector<std::string> SmsModem::GetIncomingMessages(std::string &telNumber){
    std::vector<std::string> messages, fileList, telFileList;
    std::string messageText;
    fileList = GetFilesList(inboxDir);
    if(!fileList.empty()){
        for(const auto& el : fileList){
            if(el.find(telNumber) != std::string::npos){
                telFileList.emplace_back(el);
            }
        }
        for(const auto& el : telFileList){
            std::ifstream inFile;
            try{
                inFile.open(inboxDir + "/" + el);
            }
            catch (std::ifstream::failure e) {
                std::cerr << "Exception opening/reading/closing file" << std::endl;
            }
            while (!inFile.eof()){
                inFile >> messageText;
                messages.emplace_back(messageText);
            }
            inFile.close();
        }
    }

    return messages;
}

bool SmsModem::DeleteMessagesFromDir(std::string& fullPathName){
    std::vector<std::string> fileList = GetFilesList(fullPathName);
    std::string fullFileName;
    int error = 0;
    for(const auto& el : fileList){
        fullFileName = fullPathName + "/" + el;
        if(remove(fullFileName.c_str()) == -1) error++;
    }
    if(error) return false;
    return true;
}
bool SmsModem::sendSmsMessage(std::string& telNumber, std::string& message){
    std::vector<std::string> fileList = GetFilesList(outboxDir);
    for(const auto& el : fileList){
        if(el.find("OUT") != std::string::npos) return false;
    }
    std::ofstream outFile(outboxDir + "/OUT" + telNumber + ".txt");
    if(!outFile.is_open()) return false;
    outFile << message;
    outFile.close();
    return true;
}