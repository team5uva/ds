#ifndef PARSECONFIG_H_
#define PARSECONFIG_H_

#include <string>
#include "Admin.h"

using namespace std;

class configFile {
private:


public:
    enum ERROR_CODE {
        SUCCESS, FAIL, CORRUPT_CONFIG_FILE, COULD_NOT_OPEN_FILE, FILE_NOT_FOUND
    };
    string path;
    int listenPort, CSPort;
    string CSAddress, Tag;
    vector<Admin*> adminAccess;

    configFile() {
        path = "config.cfg";
    }

    configFile(string path) {
        this->path = path;
    }

    int convertS2I(string s);
    ERROR_CODE parseFile();
    int getListenPort();
    int getCSPort();
    string getCSAddress();
    string getTag();
    vector<Admin*> getAdmin();
};
#endif /* PARSECONFIG_H_ */
