#pragma once
#include"sentryWrapper.h"
#include"sentry.h"
#include <shlwapi.h>
#include <Shlobj.h>
#include <Lmcons.h>
#include"logger/logger.hpp"
#pragma comment(lib,"shlwapi.lib")
using namespace std;
#ifdef SENTRY_PLATFORM_AIX
// AIX has a null page mapped to the bottom of memory, which means null derefs
// don't segfault. try dereferencing the top of memory instead; the top nibble
// seems to be unusable.
static void* invalid_mem = (void*)0xFFFFFFFFFFFFFF9B; // -100 for memset
#else
static void* invalid_mem = (void*)1;
#endif

bool SentryWrapper::fileExist(const std::string& filepath) {
    struct stat buffer;
    return (stat(filepath.c_str(), &buffer) == 0);
}

SentryWrapper::SentryWrapper() {
    isStarted = false;
}
SentryWrapper& SentryWrapper::getInstance() {
    static SentryWrapper instance;
    return instance;
}
bool SentryWrapper::startErrorTracking(std::string& errmsg,
    std::string dsn,
	std::string productName,
	std::string projectReleaseVerInfo,
	std::string env,
	UINT userid,
	std::string useremail
) {
	this->dsn = dsn;
	this->productName = productName;
	this->projectReleaseVerInfo = projectReleaseVerInfo;
	this->env = env;
	this->userid = userid;
	this->useremail = useremail;

    CHAR szpath[MAX_PATH];
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    std::string dbPath, dsnin;

    const char crashpadHandlerPath[] = "D:\\JayCode\\projects\\DevService\\x64\\Release\\crashpad_handler.exe";
    //TODO: Make sure it exists in pwd
    if (!fileExist(crashpadHandlerPath)) {
        errmsg = "ERROR: " + std::string(crashpadHandlerPath) + " doesn't exist in pwd!";
        return false;
    }

    // get current username
    GetUserNameA(username, &username_len);

    //Local App Data Path for user specific
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szpath))) {
        // Append product-specific path
        dbPath = szpath + string("\\") + productName + "\\.sentry-native";
    }
    else {
        dbPath = string("C:\\Users\\") + username + string("\\AppData\\Local\\") + productName + "\\.sentry-native";
    }
    LOG_DI("User AppData Local Folder Path for sentry-native dbpath: [{}]", dbPath);

    sentryDatabasePath = dbPath;

    sentry_options_t* options = sentry_options_new();
    sentry_options_set_dsn(options, dsn.c_str());
    // an app specific cache location.
    sentry_options_set_database_path(options, sentryDatabasePath.c_str());
    sentry_options_set_release(options, projectReleaseVerInfo.c_str());
    sentry_options_set_handler_path(options, crashpadHandlerPath);
    sentry_options_set_auto_session_tracking(options, false);
    sentry_options_set_symbolize_stacktraces(options, true);

    sentry_options_set_environment(options, env.c_str());
    sentry_options_set_debug(options, 1);
    if (sentry_init(options)) {
        errmsg = "ERROR:  sentry_init Failed!! Check options...";
        return false;
    }

    sentry_value_t user = sentry_value_new_object();
    //user info has these 4 types: id,username,ip_address,email
    sentry_value_set_by_key(user, "id", sentry_value_new_int32(userid));
    sentry_value_set_by_key(user, "username", sentry_value_new_string(username));
    sentry_value_set_by_key(user, "ip_address", sentry_value_new_string("{{auto}}"));
    sentry_value_set_by_key(user, "email", sentry_value_new_string(useremail.c_str()));
    sentry_set_user(user);
    isStarted = true;
    return true;
}
bool SentryWrapper::stopErrorTracking(std::string& errmsg) {
    // make sure everything flushes
    if (sentry_close()) {
        errmsg = "ERROR:  sentry_close Failed!! Check options...";
        return false;
    }
    return true;



}

SentryWrapper::~SentryWrapper() {
    std::string errmsg;
    stopErrorTracking(errmsg);
}

void SentryWrapper::setDsn(std::string dsn) {
    this->dsn = dsn;
}
void SentryWrapper::setProductName(std::string productName) {
    this->productName = productName;
}
void SentryWrapper::setProjectReleaseVerInfo(std::string projectReleaseVerInfo) {
    this->projectReleaseVerInfo = projectReleaseVerInfo;
}
void SentryWrapper::setUseremail(std::string useremail) {
    this->useremail = useremail;
}
//development or production
void SentryWrapper::setEnvironment(std::string env) {
    this->env = env;
}
void SentryWrapper::setUserid(UINT userid) {
    this->userid = userid;
}
bool SentryWrapper::createMsgEvent(std::string msg, sentry_level_e level, std::string logger) {
    if (isStarted) {
        try {
            sentry_capture_event(sentry_value_new_message_event(level, logger.c_str(), msg.c_str()));
            return true;
        }
        catch (std::exception& e) {
            std::string msgTmp = std::string("Exception in Func [") + std::string(__FUNCTION__) + std::string(":") +
                std::to_string(__LINE__) + "]: " + std::string(e.what());
            LOG_DE(msgTmp);
        }
    }
    return false;
}
void SentryWrapper::createSampleExceptionEvent() {
    sentry_value_t excpEvent = sentry_value_new_event();
    sentry_value_t exception = sentry_value_new_exception("ParseIntError", "Invalid digit found in string!");
    sentry_value_set_stacktrace(exception, NULL, 0); // capture current stack
    sentry_event_add_exception(excpEvent, exception);
    sentry_capture_event(excpEvent); // sent events to sentry

}
void SentryWrapper::createCrashEvent() {
    // divide by zero exception
    int x = 10 - 10;
    int y = 10 / x; // exception

    //Trigger crash
    trigger_crash();
}
void SentryWrapper::trigger_crash() {
    memset((char*)invalid_mem, 1, 100);
}
