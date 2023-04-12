#pragma once
#ifndef SOFTWARE_UPDATERR_HPP
#define SOFTWARE_UPDATERR_HPP
#include <iostream>
#include <Windows.h>
#include <Windows.h>
#include <fltuser.h> // Include the FltLib user-mode header
#include "src/logger/logger.hpp"

class ProcessChecker {
public:
    ProcessChecker(DWORD processId) : m_processId(processId) {
    }

    bool IsProcessRunning() {
        if (m_processId != 0) {
            // Check if the process is still running
            HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, m_processId);
            if (hProcess != NULL) {
                DWORD exitCode = STILL_ACTIVE;
                if (GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE) {
                    CloseHandle(hProcess);
                    return true;
                }
                CloseHandle(hProcess);
            }
            else {
                std::cout << std::endl << std::to_string(m_processId)<<": Open process failed!! ERROR: " << GetLastError();
            }
        }
        return false;
    }

private:
    DWORD m_processId;
};

class ProcessTerminator {
public:
    ProcessTerminator(DWORD processId) : m_processId(processId), lastErrCode(0){}

    bool TerminateProcess(std::string &errMsgIfAny) {
        // Open the process with PROCESS_TERMINATE access
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, m_processId);
        if (hProcess == NULL) {
            lastErrCode = GetLastError();
            errMsgIfAny = std::string("Failed to open process with ID: ") + std::to_string(m_processId) + ", Error code: " +  std::to_string(lastErrCode);
            
            return (lastErrCode == ERROR_ACCESS_DENIED ? false : true);
        }

        // Terminate the process
        if (!::TerminateProcess(hProcess, 0)) {
            lastErrCode = GetLastError();
            errMsgIfAny = std::string("Failed to terminate process with ID: ") + std::to_string(m_processId) + ", Error code: " + std::to_string(lastErrCode);
            CloseHandle(hProcess);
            return false;
        }

        // Wait for the process to terminate
        DWORD waitResult = WaitForSingleObject(hProcess, 60000);
        if (waitResult == WAIT_FAILED) {
            lastErrCode = GetLastError();
            errMsgIfAny = std::string("Failed to wait for process termination process with ID: ") + std::to_string(m_processId) + ", Error code: " + std::to_string(lastErrCode);
            CloseHandle(hProcess);
            return false;
        }
        CloseHandle(hProcess);
        return true;
    }
    DWORD getLastErrorCode() { return lastErrCode; }

private:
    DWORD m_processId, lastErrCode;
};

#include <iostream>
#include <Windows.h>

class InnoSetupInstaller {
public:
    InnoSetupInstaller(std::string setupPath) : installerPath(setupPath) {}

    bool RunSilent(bool isCompletelySilent, std::string& errMsgAny) {
        std::string commandLine;

        // Use /VERYSILENT for completely silent mode
        // Use /SILENT for regular silent mode
        commandLine = isCompletelySilent ? installerPath + " /VERYSILENT /SUPPRESSMSGBOXES /NORESTART" : installerPath + " /SILENT";

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if (CreateProcessA(NULL, &commandLine[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, 3*60*1000);
            DWORD exitCode;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            errMsgAny = "CreateProcess setup exit code: " + std::to_string(exitCode);
            return (static_cast<int>(exitCode) == 0 ? true: false);
        }

        errMsgAny = std::string("Failed to start installer with exit code: ") + std::to_string(GetLastError()) + ", installer path: " + installerPath;
        return false;
    }

private:
    std::string installerPath;
};

#endif // !SOFTWARE_UPDATERR_HPP
