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
