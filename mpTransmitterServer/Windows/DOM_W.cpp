#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "DOM_W.h"


namespace dom {

	class element {
	public:
		std::string tag;
		std::map<std::string, std::string> properties;
		size_t index;

		std::vector<element*> children;
		element* head = nullptr;

		element(std::string& tag, std::map<std::string, std::string>& properties) {
			this->tag = tag;
			this->properties = properties;
		}

		element(std::string& tag) {
			this->tag = tag;
		}
	};

	element* addElement(element*& parent, std::string& tag, std::map<std::string, std::string>& properties) {
		element* child = new element(tag, properties);
		child->head = parent;
		parent->children.push_back(child);
		child->index = parent->children.size() - 1;
		return child;
	}

	// Do not use. It is not ready.
	void delElement(element* element) {
		if (!element->children.empty()) {
			for (auto child : element->children) {
				delElement(child);
			}
		}
		else {
			delete element;
		}
	}

	void moveOut(element*& parent) {
		parent = parent->head;
	}

	void moveIn(element*& parent, size_t index) {
		parent = parent->children.at(index);
	}

	void next(element*& parent) {
		parent = parent->head->children.at(parent->index + 1);
	}

	void previous(element*& parent) {
		parent = parent->head->children.at(parent->index - 1);
	}

}

// Scribble. Do not use.
namespace domalt {

	class test {
	public:
		std::string tag;
		std::map<std::string, std::string> properties;

		std::vector<test*> children;

		test(std::string& tag, std::map<std::string, std::string>& properties) {
			this->tag = tag;
			this->properties = properties;
		}

		test(std::string& tag) {
			this->tag = tag;
		}

		test() {

		}
	};

}
#endif