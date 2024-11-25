#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "HTML_Reader_W.h"


void readHtml(std::string& path) {

	std::ifstream html(path, std::ifstream::ate);
	std::string str;

	size_t eof = html.tellg();
	str.resize(eof);
	html.seekg(0);
	
	for (; html.good();) {
		html.read(str.data(), eof);
	}


}

dom::element* tokenizer(std::string& str) {
	dom::element* elem = new dom::element("<!DOCTYPE html>");
	size_t beg = 0;
	size_t cur = 0;
	size_t end = 0;
	int level = 0; // 0 = normal tags, 1 = style, 2 = script, 3 = comments. Higher does ignore lowers.

	std::vector<std::string> tokens;

	if (std::string::npos != (cur = str.find("<!DOCTYPE html>"))) {
		elem = new dom::element("<!DOCTYPE html>", nullptr);
		cur = cur + 15;
		beg = cur;
		end = str.size() - 1;
	}
	else {
		return nullptr;
	}
	
	for ( ; ; ) {
		if (std::string::npos == (cur = str.find("<", beg))) {
			if (cur > beg) {
				std::string sub;
			}
		}
		else {
			break;
		}
	}
	
}
#endif