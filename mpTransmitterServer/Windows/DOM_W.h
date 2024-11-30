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

// Custom Data Object Model
namespace dom {

	class element {
	public:
		std::string tag;
		std::map<std::string, std::string> properties;
		size_t index;

		std::vector<element*> children;
		element* head = nullptr;

		// Node constructor
		element(std::string_view tag, const std::map<std::string, std::string>& properties);

		// Node constructor
		element(std::string_view tag);
	};

	// Add node into given node input
	element* addElement(element*& parent, std::string_view tag, const std::map<std::string, std::string>& properties);

	// Do not use. It is not ready.
	bool delTree(element*& parent);

	// Move to parent
	bool moveOut(element*& parent);

	// Move to child at given index
	bool moveIn(element*& parent, size_t index);

	// Move to next node that belongs to the same parent
	bool moveNext(element*& parent);

	// Move to previous node that belongs to the same parent
	bool movePrvs(element*& parent);

	// Move to top of the tree
	bool moveTop(element*& parent);
}

#endif