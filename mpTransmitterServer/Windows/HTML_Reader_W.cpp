#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "HTML_Reader_W.h"

std::vector<std::string> voidElems{ "area","base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr" };

std::vector<std::string> extElems{ "style", "script" };

const std::string spaces = " \f\n\r\t\v";

const std::string htmlAttrHol = "\"\'";

const std::string htmlTagDef = "<=/>";

const std::string htmlDelims = spaces + htmlAttrHol + htmlTagDef;

size_t openedTag = 0;
size_t closedTag = 0;

// Read html file into string then parse it to generate DOM tree.
void readHtml(std::string_view path, dom::element*& elem) {

	// Open HTML file
	std::ifstream html(path.data(), std::ifstream::ate);
	std::string htmlStr;

	size_t eof = html.tellg();
	htmlStr.resize(eof);
	html.seekg(0);
	
	// Read the file
	for (; html.good();) {
		html.read(htmlStr.data(), eof);
	}

	// Parse it to generate DOM
	elem = htmlToDOM(htmlStr);
}

// Parse the input to generate DOM tree.
dom::element* htmlToDOM(std::string_view html) {

	// Initial declarations
	std::string cutstr;
	dom::element* elem;
	size_t beg = 0;
	size_t cur = 0;
	size_t end = 0;
	size_t aux_index = 0;

	// Auxiliary containers to generate element.
	// tokens layout -> 0-tagname, 1-attribute, 2-attr_value, (odd)attribute , (even)attr_value...
	std::vector<std::string> tokens;
	std::map<std::string, std::string> properties;

	// Check if the html string is correct document type. Returns nullptr otherwise.
	// TODO: Check this before reading whole file. This is extremely inefficient!!!
	if (std::string::npos != (cur = html.find("<!DOCTYPE html>"))) {
		cutstr = "<!DOCTYPE html>";
		elem = new dom::element(cutstr);
		cur = cur + 15;
		beg = cur;
		end = html.size();
	}
	else {
		return nullptr;
	}
	
	// Starting to parse by checking '<' character and so on.
	for ( ; std::string::npos != (cur = html.find("<", beg)); ) {

		// If current position is larger than beginning reference, then check if there is meaningful string between tag points.
		// Then finally parse the string and add element to DOM tree.
		if (cur > beg) {

			// Substr and trim white-spaces
			cutstr = html.substr(beg, cur - beg);
			ltrim(cutstr);
			rtrim(cutstr);

			// If sub string is empty, then skip it. Otherwise add elemenent.
			if (cutstr != "") {
				properties.insert({ "s", cutstr });
				dom::addElement(elem, "s", properties);
				properties.clear();
			}
		}

		// Get sub string first 4 character of HTML to check if it is comment tag or end of tag.
		beg = cur;
		cutstr = html.substr(beg, 4);

		// Check if end of tag. For example "</a>" indicates end of tag in HTML syntax.
		// So, it moves out of current element, to the parent. Then continues.
		if (cutstr[1] == '/') {
			dom::moveOut(elem);

			if (elem->head != nullptr) { // DEBUG
				if (elem->head->tag == "head") {
					std::cout << "in your head...\n";
				}
				else if (elem->head->tag == "body") {
					std::cout << "in your body...\n";
				}
				else if (elem->head->tag == "html") {
					std::cout << "in your hmtl...\n";
				}
				else if (elem->head->tag == "<!DOCTYPE html>") {
					std::cout << "in your DOCTYPE...\n";
				}// DEBUG
			}
			{std::cout << "CLOSED:\t" << closedTag++ << std::endl; } // DEBUG

			cur = html.find(">", beg) + 1;
			beg = cur;
			continue;
		}

		// Check if comment. Adds comment element into current element but does not switch to created one (child).
		// Stays on parent, since comment elements can't contain another element. Then continues.
		if (cutstr == "<!--") {
			
			// Find comment trailer.
			beg = beg + 4;
			cur = html.find("-->", beg);

			cutstr = html.substr(beg, cur - beg);
			properties.insert({ "s", cutstr });
			dom::addElement(elem, "c", properties);

			properties.clear();
			beg = cur + 3;
			continue;
		}

		// If the loop instance survives to this point then that means it is a new tag.
		// Find tag trailer, then parse it to tokenize.
		cur = html.find(">", beg) + 1;
		cutstr = html.substr(beg, cur - beg);
		tokens = tagToTokens(cutstr);
		beg = cur;

		// Check if tag name matches with void or external HTML tags. Otherwise it is normal HTML tag.
		if (std::string::npos != contains(voidElems, tokens.front())) { // Void tags
			for (size_t i = 1; i < tokens.size() - 1; i = i + 2) {
				properties.insert({ tokens.at(i), tokens.at(i + 1) });
			}
			dom::addElement(elem, tokens.at(0), properties); // Void tags cannot contain another tag, therefore stay on parent
			properties.clear();
			tokens.clear();
			continue;
		}
		else if(std::string::npos != (aux_index = contains(extElems, tokens.front()))){ // External tags
			cur = html.find("</" + extElems.at(aux_index) +">", beg);
			beg = cur + 2;
			cur = html.find(">", beg);
			beg = cur + 1;
			tokens.clear();
			continue;
		}
		else { // Normal Tags

			{std::cout << "OPENED:\t" << openedTag++ << std::endl; } // DEBUG

			for (size_t i = 1; i < tokens.size() - 1; i = i + 2) {
				properties.insert({ tokens.at(i), tokens.at(i + 1) });
			}
			elem = dom::addElement(elem, tokens.at(0), properties); // Normal tags can contain another tag, therefore switch to child
			properties.clear();
			tokens.clear();
			continue;
		}
	}

	return elem;
}

// Tokenize the input according to HTML Tag syntax. Input is tested for tag only.
std::vector<std::string> tagToTokens(std::string_view tag) {

	std::vector<std::string> tokens;
	std::string cutstr;
	size_t cur = 0;
	size_t beg = 0;
	bool ignore = false;
	char foundHol = 0;
	
	// Tokenize according to HTML delimeters. \' and \" characters are holder/container adverb.
	for (; std::string::npos != (cur = tag.find_first_of(htmlDelims, beg)); ) {

		// Checks if found a holder/container character (\"\') then proceeds to find the pair
		if (std::string::npos != htmlAttrHol.find(foundHol = tag[cur])) {
			ignore = true;
			beg = cur + 1;
			cur = tag.find_first_of(foundHol, beg);
		}

		// Fecth and trim
		cutstr = tag.substr(beg, cur - beg);
		ltrim(cutstr);
		rtrim(cutstr);

		// If nothing found then skip and continue
		if (cutstr == "" && !ignore) {
			beg = cur + 1;
			continue;
		}

		tokens.push_back(cutstr);
		beg = cur + 1;
		ignore = false;
		
	}
	
	return tokens;
}

// Check if the list contains the tag name.
size_t contains(const std::vector<std::string>& list, std::string_view tag) {
	for (size_t i = 0; i < list.size(); i++) {
		if (list.at(i) == tag) {
			return i;
		}
	}
	return -1;
}

// Trim the string from front.
inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// Trim the string from end.
inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}



#endif