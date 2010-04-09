#include <fstream>

class configFile {
  String path;

public:
  enum ERROR_CODE{FILE_NOT_FOUND, UNABLE_TO_OPEN_FILE, ERROR_IN_FILE_FORMAT, SUCCESS}
  int listenPort, CSPort;
  String CSAddress, Tag;

  configFile() {
    this.path = config/config.cfg;
  }

  configFile(String path) {
    this.path = path
  }

  enum parseFile() {
    ifstream configFile (path);
      if (configFile.is_open()) {
      
        configFile.close();
      } else {
        return 1;
      }
  return 0;
  }

  int getListenPort() {
    return listenPort;
  }

  int getCSPort() {
    return CSPort;
  }

  String getCSAddress() {
    return CSAddress;
  }

  String getTag() {
    return Tag;
  }
};
