//
// Created by kvitadmin on 03.11.2021.
//

#ifndef TESTDIR_SMSMODEM_H
#define TESTDIR_SMSMODEM_H
#include <dirent.h>
#include <utility>
#include <vector>
#include <map>
#include <fstream>

class SmsModem {
    std::string pathToDir;
    std::string inboxDir;
    std::string outboxDir;

    static std::vector<std::string> GetFilesList(std::string &fullPathName);
public:
    SmsModem(std::string inPathToModemDir){
        pathToDir = std::move(inPathToModemDir);
        inboxDir = pathToDir + "/inbox";
        outboxDir = pathToDir + "/outbox";
    }
    std::vector<std::string> GetIncomingMessages(std::string &telNumber);
    bool sendSmsMessage(std::string& telNumber, std::string& message);
    static bool DeleteMessagesFromDir(std::string& fullPathName);
};


#endif //TESTDIR_SMSMODEM_H
