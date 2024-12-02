#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string_view>

#include "DOM_W.h"

namespace hr {
	extern size_t openedTag;
	extern size_t closedTag;

	// Read html file into string then parse it to generate DOM tree.
	void readHtml(std::string_view path, dom::Element*& elem);

	// Parse the input to generate DOM tree.
	dom::Element* htmlToDOM(std::string_view html);

	// Tokenize the input according to HTML Tag syntax. Input is tested for tag only.
	std::vector<std::string> tagToTokens(std::string_view str);

	// Check if the list contains the tag name.
	size_t contains(const std::vector<std::string>& list, std::string_view tag);

	// Trim the string from front.
	void ltrim(std::string& s);

	// Trim the string from end.
	void rtrim(std::string& s);

}

#endif

// Note to myself
// Passing string to function
// string&& -> forwarding and modify
// string& -> modify and not suitable for temp
// string_view -> read but not owning
// const string&& -> literal perfect forwarding
// const string& -> read and owning
// string -> extra copy
// 
//reinterpret_cast<const char   *>(u8"text"); // Ok.
//reinterpret_cast<const char8_t*>("text");   // Undefined behavior.