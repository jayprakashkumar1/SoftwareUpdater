#pragma once
#ifndef LOGGER_HPP
#define LOGGER_HPP
// Enable SPDLOG_TRACE macro, see: spdlog/tweakme.h
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#define SPDLOG_DEBUG_ON
#define SPDLOG_TRACE_ON 

#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
extern std::shared_ptr<spdlog::logger> LOGGER; // Global Logger as shared_ptr
extern std::shared_ptr<spdlog::logger> LOGGER_DC; // Global Logger For DeviceControl
extern std::shared_ptr<spdlog::logger> LOGGER_AC; // Global Logger for AppControl
extern std::shared_ptr<spdlog::logger> LOGGER_DLP; // Global Logger for DLP
extern std::shared_ptr<spdlog::logger> EVENTS_LOGGER; // Global Logger for Events

#define TAG_ALL 0 //TAG for ALL EVENTS
#define TAG_DC 1 //TAG_DEVICE_CONTROL
#define TAG_AC 2 //TAG_APP_CONTROL
#define TAG_DLP 3 //TAG_DLP 

//Logs File path
#define EPS_LOGS_FILE_PATH "logs\\EPSLog.log"
#define DEVICE_CONTROL_LOGS_FILE_PATH "logs\\deviceControlLog.log"
#define APP_CONTROL_LOGS_FILE_PATH "logs\\appControlLog.log"
#define DLP_LOGS_FILE_PATH "logs\\dlpLog.log"

//Logger Name
#define EPS_LOGGER_NAME "EpsLoggerName"
#define DEVICE_CONTROL_LOGGER_NAME "DeviceControlLoggerName"
#define APP_CONTROL_LOGGER_NAME "AppControlLoggerName"
#define DLP_LOGGER_NAME "DlpLoggerName"

//Events File Path
#define EVENTS_LOGS_FILE_PATH "events/eventsLog.log"

//Events Logger Name
#define EVENTS_LOGGER_NAME "EventsLoggerName"


#define EVENTS_TYPE_ALL "All Events"
#define EVENTS_TYPE_DC "Device Control"
#define EVENTS_TYPE_AC "App Control"
#define EVENTS_TYPE_DLP "DLP"

#define LOGS_TYPE_DC "Device Control"
#define LOGS_TYPE_AC "App Control"
#define LOGS_TYPE_DLP "DLP"

// default log file size = 200MB, file = 1, deleteContentOnOpen = false
bool logInit(std::shared_ptr<spdlog::logger>& LOGGER,
    std::string loggerName,
    const spdlog::filename_t filepath,
    bool rotateOnOpen = false,
    unsigned long long maxFileSize = 1048576 * 200, // log file size = 200MB
    unsigned long long maxFiles = 1
);
//TODO: As more if needed 

/*Events Logs: 
eventsTag: Tag to determine what kind of events.(i.e deviceControl,AppControl,Dlp,etc)
actionStr: What Kind of action performed.(i.e Allowed,Blocked,ReadOnly,ServiceStarted,etc)
desc: The Actual message/description
*/
void eventsLogging(int eventsTag, std::string actionStr, std::string desc);

#define DEBUG_LOG 1 // If enabled then detailed all logs will be written to file 
#define IMPORTANT_LOG 1 // If enabled then detailed all logs will be written to file 

//
//GLOBAL EPS
// 
    //LOG For EPS in Debug mode only as Info.
    #define LOG_DI(fmt, ...) \
                do { if(DEBUG_LOG) LOGGER->info(fmt, __VA_ARGS__); } while (0)
    //LOG For EPS in Debug mode only as Error.
    #define LOG_DE(fmt, ...) \
                do { if(DEBUG_LOG) LOGGER->error(fmt, __VA_ARGS__); } while (0)

    //LOG For EPS in Important mode only as Info.
    #define LOGI(fmt, ...) \
                do { if(IMPORTANT_LOG) LOGGER->info(fmt, __VA_ARGS__); } while (0)
    //LOG For EPS in Important mode only as Error.
    #define LOGE(fmt, ...) \
                do { if(IMPORTANT_LOG) LOGGER->error(fmt, __VA_ARGS__); } while (0)


//
//For DeviceControl
//
    //LOG For DeviceControl in Debug mode only as Info.
    #define LOG_DC_DI(fmt, ...) \
                do { if(DEBUG_LOG) LOGGER_DC->info(fmt, __VA_ARGS__); } while (0)
    //LOG For DeviceControl in Debug mode only as Error.
    #define LOG_DC_DE(fmt, ...) \
                do { if(DEBUG_LOG) LOGGER_DC->error(fmt, __VA_ARGS__); } while (0)

    //LOG For DeviceControl in Important mode only as Info.
    #define LOG_DC_I(fmt, ...) \
                do { if(IMPORTANT_LOG) LOGGER_DC->info(fmt, __VA_ARGS__); } while (0)
    //LOG For DeviceControl in Important mode only as Error.
    #define LOG_DC_E(fmt, ...) \
                do { if(IMPORTANT_LOG) LOGGER_DC->error(fmt, __VA_ARGS__); } while (0)

//
//For AppControl
//

//LOG For AppControl in Debug mode only as Info.
#define LOG_AC_DI(fmt, ...) \
            do { if(DEBUG_LOG) LOGGER_AC->info(fmt, __VA_ARGS__); } while (0)
//LOG For AppControl in Debug mode only as Error.
#define LOG_AC_DE(fmt, ...) \
            do { if(DEBUG_LOG) LOGGER_AC->error(fmt, __VA_ARGS__); } while (0)

//LOG For AppControl in Important mode only as Info.
#define LOG_AC_I(fmt, ...) \
            do { if(IMPORTANT_LOG) LOGGER_AC->info(fmt, __VA_ARGS__); } while (0)
//LOG For AppControl in Important mode only as Error.
#define LOG_AC_E(fmt, ...) \
            do { if(IMPORTANT_LOG) LOGGER_AC->error(fmt, __VA_ARGS__); } while (0)

//
//For Dlp
//
//LOG For Dlp in Debug mode only as Info.
#define LOG_DLP_DI(fmt, ...) \
            do { if(DEBUG_LOG) LOGGER_DLP->info(fmt, __VA_ARGS__); } while (0)
//LOG For Dlp in Debug mode only as Error.
#define LOG_DLP_DE(fmt, ...) \
            do { if(DEBUG_LOG) LOGGER_DLP->error(fmt, __VA_ARGS__); } while (0)

//LOG For Dlp in Important mode only as Info.
#define LOG_DLP_I(fmt, ...) \
            do { if(IMPORTANT_LOG) LOGGER_DLP->info(fmt, __VA_ARGS__); } while (0)
//LOG For Dlp in Important mode only as Error.
#define LOG_DLP_E(fmt, ...) \
            do { if(IMPORTANT_LOG) LOGGER_DLP->error(fmt, __VA_ARGS__); } while (0)

#endif 
