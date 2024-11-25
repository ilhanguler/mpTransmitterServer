#pragma once
// TODO:
// 1. Null safe implementation
// 2. Implement moving between elements safely
// 3. Deletion and addition logic overhaul is a must.

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <list>
#include <map>

namespace dom {
	class element;
	element* addElement(element*& parent, std::string& tag, std::map<std::string, std::string>& properties);
	void delElement(element* element);
	void moveOut(element*& parent);
	void moveIn(element*& parent, size_t index);
	void next(element*& parent);
	void previous(element*& parent);
}

#endif