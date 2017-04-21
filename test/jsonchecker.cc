#include "../json.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;
using namespace json;

string getJsonStr(const string& filename) {
  ifstream ifstrm(filename);
  if (!ifstrm.is_open()) throw runtime_error("can't open " + string(filename));
  string jsonStr;
  while (ifstrm) {
    string line;
    getline(ifstrm, line);
    jsonStr += line + "\n";
  }
  return jsonStr;
}

void failJson(const string& filename) {
  string jsonStr = getJsonStr(filename);
  string errMsg;
  Json json_ = Json::parse(jsonStr, errMsg);
  if (errMsg == "") {
    cerr << "ERROR! expect fail, but pass" << endl;
    cerr << "file: " << filename << endl;
    cerr << jsonStr << endl;
    cerr << endl;
  }
}

void passJson(const string& filename) {
  string jsonStr = getJsonStr(filename);
  string errMsg;
  Json json_ = Json::parse(jsonStr, errMsg);
  if (errMsg != "") {
    cerr << "ERROR! expect pass, but fail" << endl;
    cerr << "file: " << filename << endl;
    cerr << jsonStr << endl;
    cerr << endl;
  }
}

int main() {
  struct stat stat;
  if (lstat(".", &stat) < 0) {
    cerr << "lstat error" << endl;
    return 0;
  }
  DIR* dp = opendir("../Data");
  if (!dp) {
    perror("can't open Data");
    return 0;
  }
  dirent* dirp;
  while ((dirp = readdir(dp)) != nullptr) {
    string filename = "../Data/";
    switch (dirp->d_name[0]) {
      case 'f':
        failJson(filename + dirp->d_name);
        break;
      case 'p':
        passJson(filename + dirp->d_name);
        break;
      default:
        break;
    }
  }
  closedir(dp);
}
