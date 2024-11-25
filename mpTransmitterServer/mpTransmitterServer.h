// mpTransmitterServer.h : Include file for standard system include files,
// or project specific include files.

#pragma once


#ifdef __linux__
#include <iostream>
#include "Linux/Networking.h"
#endif
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <iostream>
#include <fstream>

#include "Windows/Networking_W.h"

#endif 






// Greetings
#ifdef __linux__
void greetings() {
	std::cout << "Hello Linux, server is starting..." << std::endl << "CPP version: " << __cplusplus << std::endl;
}
#endif
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
void greetings() {
	std::cout << "Hello Windows, server is starting...\nPOCO version:\t" << Poco::format("%d.%d.%d",
		static_cast<int>(Poco::Environment::libraryVersion() >> 24),
		static_cast<int>((Poco::Environment::libraryVersion() >> 16) & 0xFF),
		static_cast<int>((Poco::Environment::libraryVersion() >> 8) & 0xFF)) << std::endl << "CPP version:\t" << _MSVC_LANG << std::endl;
}
#endif
// Greetings
