#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "DOM_W.h"

// Custom Data Object Model
namespace dom {

	// Node constructor
	element::element(std::string_view tag, const std::map<std::string, std::string>& properties) {
		this->tag = tag;
		this->properties = properties;
	}

	// Node constructor
	element::element(std::string_view tag) {
		this->tag = tag;
	}

	// Add node into given node input
	element* addElement(element*& parent, std::string_view tag, const std::map<std::string, std::string>& properties) {
		element* child = new element(tag, properties);
		child->head = parent;
		parent->children.push_back(child);
		child->index = parent->children.size() - 1;
		return child;
	}

	// Do not use. It is not ready.
	bool delTree(element*& parent) {
		auto& children = parent->children;
		size_t s = children.size();
		if (s == 0) {
			element* tobedeleted = parent;
			moveOut(parent);
			delete tobedeleted;
		}
		else {
			for (size_t i = 0; i < s; i++) {
				delTree(children.at(i));
			}
		}
		return true;
	}

	// Move to parent
	bool moveOut(element*& parent) {
		if (parent->head == nullptr) {
			return false;
		}
		parent = parent->head;
		return true;
	}

	// Move to child at given index
	bool moveIn(element*& parent, size_t index) {
		if (parent->children.size() < index) {
			return false;
		}
		parent = parent->children.at(index);
		return true;
	}

	// Move to next node that belongs to the same parent
	bool moveNext(element*& parent) {
		if ((parent->head == nullptr) || (parent->head->children.size() < parent->index + 1)) {
			return false;
		}
		parent = parent->head->children.at(parent->index + 1);
		return true;
	}

	// Move to previous node that belongs to the same parent
	bool movePrvs(element*& parent) {
		if ((parent->head == nullptr) || (parent->index < 1)) {
			return false;
		}
		parent = parent->head->children.at(parent->index - 1);
	}

	// Move to top of the tree
	bool moveTop(element*& parent) {
		for (; moveOut(parent) == true;) {

		}
		return true;
	}

}

#endif