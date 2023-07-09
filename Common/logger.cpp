#include "logger.h"

namespace Log {

Logger::Logger()
{
    boost::filesystem::path full_path(boost::filesystem::current_path());
    logFileprotocol= full_path.string()+"/protocol.log";
    std::cout<<logFileprotocol<<std::endl;
}


string Logger::getCurrentDateTime( string s ){
    time_t now = time(0);
    tm  tstruct;
    char  buf[80];
    tstruct = *localtime(&now);
    if(s=="now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if(s=="date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return string(buf);
};

void Logger::writelogprotocol(const char *str)
{
    std::lock_guard<std::recursive_mutex> guard(mmutex);
    ofstream ofs(logFileprotocol.c_str(), std::ios_base::out | std::ios_base::app );
    string now = getCurrentDateTime("now");
    ofs << now << '\t' << str << '\n';
    ofs.close();
}

}
