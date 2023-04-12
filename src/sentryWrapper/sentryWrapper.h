#pragma once
#ifndef SENTRY_WRAPPERR_HPP
#define SENTRY_WRAPPERR_HPP
#include <iostream>
#include<Windows.h>
#include <string>
#include"sentry.h"

class SentryWrapper {

	bool isStarted;
	std::string username;
	std::string useremail;
	UINT userid;

	std::string sentryDatabasePath;
	std::string dsn;
	std::string productName;
	std::string projectReleaseVerInfo;
	std::string env; //development or production

	
	bool fileExist(const std::string& filepath);
	void trigger_crash();

	void setDsn(std::string dsn);
	void setProductName(std::string productName);
	void setProjectReleaseVerInfo(std::string projectReleaseVerInfo);
	void setUseremail(std::string useremail);
	void setEnvironment(std::string env); //development or production

	void setUserid(UINT userid);

	void createSampleExceptionEvent();
	void createCrashEvent();

	SentryWrapper();

public:

	~SentryWrapper();

	// Copy construct
	SentryWrapper(SentryWrapper const&) = delete;
	// Move construct
	SentryWrapper(SentryWrapper&&) = delete;
	// Copy assign
	SentryWrapper& operator = (SentryWrapper const&) = delete;
	// Move assign
	SentryWrapper& operator = (SentryWrapper&&) = delete;

	static SentryWrapper& getInstance();

	//dsn,
	//productName,
	//projectReleaseVerInfo,
	//env = "development,
	//userid = 101,
	//useremail = "user101@example.com"
	bool startErrorTracking(std::string& errmsg, 
		std::string dsn,
		std::string productName,
		std::string projectReleaseVerInfo,
		std::string env = "development",
		UINT userid = 101,
		std::string useremail = "user101@example.com"
	);

	bool stopErrorTracking(std::string& errmsg);

	bool createMsgEvent(std::string msg, sentry_level_e level = SENTRY_LEVEL_ERROR, std::string logger = "my-logger");
	
};

#define g_sentry SentryWrapper::getInstance()

#endif // !SENTRY_WRAPPERR_HPP
