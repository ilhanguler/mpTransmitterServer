#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string_view>

// Custom Data Object Model
namespace dom {

	class Element {
	public:
		// HTML element tag name
		std::string tag;

		// HTML tag properties
		std::map<std::string, std::string> properties;

		// Index of DOM object in a parent. Default value (-1) means it has not a parent
		size_t index = -1;

		// List of pointers to children of this object
		std::vector<Element*> children;

		// Pointer to parent of this object
		Element* head = nullptr;

		// Node constructor
		Element(std::string_view tag, const std::map<std::string, std::string>& properties);

		// Node constructor, handy for the beginning of a tree.
		Element(std::string_view tag);

		~Element();
	};

	// Add node into given node input
	Element* addElement(Element*& parent, std::string_view tag, const std::map<std::string, std::string>& properties);

	// Deletes whole tree.
	bool delTree(Element*& parent);

	// Do not use. It is not ready.
	bool delSubTree(Element*& parent);

	// Move to parent
	bool moveOut(Element*& parent);

	// Move to child at given index
	bool moveIn(Element*& parent, size_t index);

	// Move to next node that belongs to the same parent
	bool moveNext(Element*& parent);

	// Move to previous node that belongs to the same parent
	bool movePrvs(Element*& parent);

	// Move to top of the tree
	bool moveTop(Element*& parent);
}

#endif