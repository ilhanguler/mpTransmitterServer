#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "DOM_W.h"

// Custom Data Object Model
namespace dom {

	// Node constructor
	Element::Element(std::string_view tag, const std::map<std::string, std::string>& properties) {
		this->tag = tag;
		this->properties = properties;
	}

	// Node constructor
	Element::Element(std::string_view tag) {
		this->tag = tag;
	}

	Element::~Element() { };

	// Add node into given node input
	Element* addElement(Element*& parent, std::string_view tag, const std::map<std::string, std::string>& properties) {
		Element* child = new Element(tag, properties);
		child->head = parent;
		parent->children.push_back(child);
		child->index = parent->children.size() - 1;
		return child;
	}

	// Deletes whole tree.
	bool delTree(Element*& parent) {
		moveTop(parent);
		return delSubTree(parent);
	}

	// Deletes sub tree that belongs to the current parent and parents itself.
	bool delSubTree(Element*& parent) {

		for (size_t i = 0; moveIn(parent, i); i++) {
			delSubTree(parent);
		}

		Element* tobedeleted = parent;
		moveOut(parent);
		delete tobedeleted;

		return true;
	}

	// Move to parent
	bool moveOut(Element*& parent) {
		if (parent->head == nullptr) {
			return false;
		}
		parent = parent->head;
		return true;
	}

	// Move to child at given index
	bool moveIn(Element*& parent, size_t index) {
		if (parent->children.size() <= index) {
			return false;
		}
		parent = parent->children.at(index);
		return true;
	}

	// Move to next node that belongs to the same parent
	bool moveNext(Element*& parent) {
		if ((parent->head == nullptr) || (parent->head->children.size() <= (parent->index + 1))) {
			return false;
		}
		parent = parent->head->children.at(parent->index + 1);
		return true;
	}

	// Move to previous node that belongs to the same parent
	bool movePrvs(Element*& parent) {
		if ((parent->head == nullptr) || (parent->index == 0)) {
			return false;
		}
		parent = parent->head->children.at(parent->index - 1);
	}

	// Move to top of the tree
	bool moveTop(Element*& parent) {
		for (; moveOut(parent); ) {

		}
		return true;
	}

}

#endif