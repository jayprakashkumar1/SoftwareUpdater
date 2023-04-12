#include <iostream>
#include <Windows.h>
#include <string>
#include <chrono>
#include "softwareUpdater.hpp"
#include"src/logger/logger.hpp"

using namespace std;
using namespace std::chrono;

#define LOGS_INIT
#define DRIVER_STOP_UTIL
#define SERVICE_STOP_UTIL

// get output of command as a string when run using system
std::string exec(std::string command) {
    char buffer[128];
    string result = "";

    // Open pipe to file
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        LOGE("ERROR: couldn't open _popen! ERROR: [{}]", GetLastError());
        return result;
    }

    // read till end of process:
    while (!feof(pipe)) {
        try {
            // use buffer to read and add to result
            if (fgets(buffer, 128, pipe) != NULL) {
                result += buffer;
            }
        }
        catch (std::exception& e) {
            std::string msg = std::string("Exception in Func [") + std::string(__FUNCTION__) + std::string(":") +
                std::to_string(__LINE__) + "]: " + std::string(e.what());
            LOG_DC_DE(msg);
            //g_sentry.createMsgEvent(msg);
        }
    }

    _pclose(pipe);
    return result;
}


/*
Argc: Number of argument, 
argv[0]: processPath
argv[1]: minifilterDriverName
argv[2]: installerSetup path
argv[3-...]:  process ids of apps(space separated) which are currently running
*/
int main(int argc, char *argv[]) {
    //disable console popup
   AllocConsole();
   ShowWindow(GetConsoleWindow(), SW_HIDE);

    std::string driverName, serviceName,setupPath, errmsg = "", command = "",commandOutput = "";
    std::vector<DWORD> processIdList;

    //
    //logs
    //
#ifdef LOGS_INIT
    std::string appDataRoamingPath = "C:\\softwareUpdaterLogs";
    if (!(logInit(LOGGER, EPS_LOGGER_NAME, appDataRoamingPath + EPS_LOGS_FILE_PATH, true) &&
        logInit(LOGGER_DC, DEVICE_CONTROL_LOGGER_NAME, appDataRoamingPath + DEVICE_CONTROL_LOGS_FILE_PATH, true) &&
        logInit(LOGGER_AC, APP_CONTROL_LOGGER_NAME, appDataRoamingPath + APP_CONTROL_LOGS_FILE_PATH) &&
        logInit(LOGGER_DLP, DLP_LOGGER_NAME, appDataRoamingPath + DLP_LOGS_FILE_PATH) &&
        logInit(EVENTS_LOGGER, EVENTS_LOGGER_NAME, appDataRoamingPath + EVENTS_LOGS_FILE_PATH)
        )) {
        std::string logmsg = "Couldn't init the ALL EPS logs. Restart the app with permissions!";
        OutputDebugStringA("WijUngleEpsSvc ServiceMain: logInit failed!!");
        return 1;
    }
#endif // LOGS_INIT

    if (argc < 4) {
        LOGE("Usage:\n{} <MinifilterDriverName> <ServiceName> <InstallerSetupPath> <processIdListSpaceSeparated>", argv[0]);
        LOGE("Not enough arguments! Exiting...");
        return 0;
    }
    //
    //parsing
    //
    driverName = std::string(argv[1]);
    serviceName = std::string(argv[2]);
    setupPath = std::string(argv[3]);
    for (int i = 4; i < argc; i++) {
        try {
            processIdList.push_back(std::stoi(argv[i]));
        }
        catch (std::exception& e){}
    }
    
    //
    // Stop the driver first
    //
#ifdef DRIVER_STOP_UTIL
        command = "net stop " + driverName;
        commandOutput = exec(command);
        LOGE("command: [{}], output: [{}]", command, commandOutput);
#endif // DRIVER_START_UTIL

    //
    // Stop the service first
    //
#ifdef SERVICE_STOP_UTIL
        command = "sc stop " + serviceName;
        commandOutput = exec(command);
        LOGE("command: [{}], output: [{}]", command, commandOutput);
#endif // SERVICE_STOP_UTIL

        //
        // Wait for exiting the apps,  Max wait 2 mins
        //
        bool allExited = false;

        auto finish = system_clock::now() + 2min;
        do{
            allExited = true;
            for (int i = 0; i < processIdList.size(); ++i) {
                try {
                    ProcessChecker processChecker(processIdList.at(i));
                    if (processChecker.IsProcessRunning()) {
                       LOG_DI("pid: [{}], Process is still running.", processIdList.at(i));
                        allExited = false;
                    }
                    else {
                        LOG_DI("pid: [{}], Process is NOT running.", processIdList.at(i));
                    }
                }
                catch (std::exception& e) {}
            }
            Sleep(1000);
        } while ((system_clock::now() < finish) && !allExited);

        if (allExited) {
            LOG_DI("All process with given Ids are exited!!!");
        }
        else {
            LOG_DI("Timeout occured while waiting for process close!!!");
            LOG_DI("Terminating process...");

            do {
                allExited = true;
                for (int i = 0; i < processIdList.size(); i++) {
                    try {
                        ProcessTerminator psKillObj(processIdList.at(i));
                        psKillObj.TerminateProcess(errmsg);

                        ProcessChecker processChecker(processIdList.at(i));
                        if (processChecker.IsProcessRunning()) {
                            LOG_DI("pid: [{}], Process is still running.", processIdList.at(i));
                            allExited = false;
                        }
                        else {
                            LOG_DI("pid: [{}], Process is NOT running.", processIdList.at(i));
                        }
                    }
                    catch (std::exception& e) {}
                }
                Sleep(1000);
            } while (!allExited);
        }
    
        //
        //Now Silently Install from innosetup
        //
        LOG_DI("Silent Installation steps");

        InnoSetupInstaller installerObj(setupPath);
        if (installerObj.RunSilent(true, errmsg)) {
            LOG_DI("Installation successful! Installer path: [{}]", setupPath);

            //TODO: Start the GUI, svc

        }
        else {
            LOG_DI("Installation was NOT successful! Installer path: [{}], error: [{}]", setupPath, errmsg);
        }

    return 0;

}



