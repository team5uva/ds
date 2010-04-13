#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <iostream>
#include <vector>

#include "configFile.h"

using namespace std;

string arg_Comment = ("#");
string arg_Admin = ("$");
string arg_CSAddress = ("CSAddress ");
string arg_CSPort = ("CSPort ");
string arg_listenPort = ("listenPort ");
string arg_Tag = ("Tag ");

size_t found;

/* Parse supplied configuration file.
 * Not possible yet to set own path. */
configFile::ERROR_CODE configFile::parseFile() {
    string line;
    ifstream cfgFile(path.c_str());
    if (cfgFile.is_open()) {
        while (!cfgFile.eof()) {
            getline(cfgFile, line);
            if (line.at(0) == arg_Comment.at(0)) {
                /* Comment in config file, skipping line. */
            } else if (line.at(0) == arg_Admin.at(0)) {
                /* Administrator username and password */
                if (found = line.find("|")) {
                    Admin *admin = new Admin();
                    admin->name = line.substr(1, found - 1);
                    admin->password = line.substr(found + 1, line.length() - 1);
                    adminAccess.push_back(admin);
                } else {
                    return CORRUPT_CONFIG_FILE;
                }
            } else {
                /* Line not a comment, or admin username check if there is any
                 * info to gather. */
                if (found = line.find(arg_CSAddress) != string::npos) {
                    CSAddress = line.substr(
                            (found + (arg_CSAddress.length() - 1)));
                }
                if (found = line.find(arg_CSPort) != string::npos) {
                    CSPort = convertS2I(line.substr(
                            found + (arg_CSPort.length() - 1)));
                }
                if (found = line.find(arg_listenPort) != string::npos) {
                    listenPort = convertS2I(line.substr(
                            found + (arg_listenPort.length() - 1)));
                }
                if (found = line.find(arg_Tag) != string::npos) {
                    Tag = line.substr(found + (arg_Tag.length() - 1));
                }
            }
        }
        cfgFile.close();
    } else {
        return configFile::FILE_NOT_FOUND;
    }
    return configFile::SUCCESS;
}

int configFile::convertS2I(string s) {
    stringstream ss(s);
    int j;
    ss >> j;
    return j;
}

int configFile::getListenPort() {
    return listenPort;
}

int configFile::getCSPort() {
    return CSPort;
}

string configFile::getCSAddress() {
    return CSAddress;
}

string configFile::getTag() {
    return Tag;
}

vector<Admin*> configFile::getAdmin() {
    return adminAccess;
}