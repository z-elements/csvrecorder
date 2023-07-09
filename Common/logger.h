#include <iostream>
#include <stdexcept>
#include <mutex>
#include <fstream>
#include <boost/filesystem.hpp>

using namespace std;

namespace Log {
class Logger {

public:
    static Logger* getInstance()
    {
        static Logger logger_;
        return &logger_;

    }
    void writelogprotocol(const string &str){writelogprotocol(str.c_str());}
    void writelogprotocol(const char *str);
    string getCurrentDateTime(string s);
private:
    Logger();
    Logger& operator=(Logger const&);
    virtual ~Logger(){};
    std::string logFileprotocol;
    std::recursive_mutex mmutex;

};
}
