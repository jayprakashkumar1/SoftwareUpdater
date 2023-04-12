#include "logger.hpp"
//#include"..\app.hpp"



//Extern
    /* For Logs*/
std::shared_ptr<spdlog::logger> LOGGER; // Global Logger for EPS 
std::shared_ptr<spdlog::logger> LOGGER_DC; // Global Logger For DeviceControl
std::shared_ptr<spdlog::logger> LOGGER_AC; // Global Logger for AppControl
std::shared_ptr<spdlog::logger> LOGGER_DLP; // Global Logger for DLP

    /* For Events*/
std::shared_ptr<spdlog::logger> EVENTS_LOGGER; // Global Events Logger

// default log file size = 200MB, file = 1, deleteContentOnOpen = false
bool logInit(std::shared_ptr<spdlog::logger>& LOGGER,
    std::string loggerName,
    const spdlog::filename_t filepath,
    bool rotateOnOpen,
    unsigned long long maxFileSize,
    unsigned long long maxFiles
) {
    try {
        LOGGER = spdlog::rotating_logger_mt(loggerName, filepath, maxFileSize, maxFiles, rotateOnOpen);
        spdlog::flush_on(spdlog::level::info);
        spdlog::set_pattern("[source %s][function %!][line %#] [%d-%m-%Y %X:%e msSinceLastMsg: %o] [thread %t] [%l] [%s:%#] %v ");
        // spdlog::set_pattern("[%d-%m-%Y %r] [thread %t] [%l] [%s:%#] %v ");
        spdlog::set_level(spdlog::level::trace);


        LOGGER->info("........................... InfoDemo.............................\n");
        return TRUE;
    }
    catch (const spdlog::spdlog_ex& ex) {
        char buffer[512];
        sprintf_s(buffer, sizeof(buffer), "Log init failed : %s", ex.what());
        MessageBoxA(NULL, buffer, "Log Error!", MB_ICONERROR);
        return FALSE;
    }
}

void eventsLogging(int eventsTag, std::string actionStr, std::string desc) {
    try {
        EVENTS_LOGGER->info("[TagId:{}] [{}] [{}]", eventsTag, actionStr, desc);
    }
    catch (std::exception& e) {}
}



