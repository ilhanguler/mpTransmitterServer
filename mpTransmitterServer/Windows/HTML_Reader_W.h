#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <list>
#include <map>

#include "DOM_W.h"

void readHtml(std::string& path);

dom::element* tokenizer(std::string& str);

#endif

