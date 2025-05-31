// mpTransmitterServer.cpp : Defines the entry point for the application.
//

#include "mpTransmitterServer.h"
#include "Windows/HTML_Reader_W.h"
 
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
void sendHTTPSRequest(std::string url) {
try {
	// Parse URL
	Poco::URI uri(url);
	std::string path = uri.getPathAndQuery();
	std::string host = uri.getHost();
	Poco::UInt16 port = uri.getPort();

	// Create HTTPS session
	Poco::Net::Context::Ptr context = new Poco::Net::Context(
		Poco::Net::Context::TLS_CLIENT_USE,
		"../../test/cacert.pem",
		Poco::Net::Context::VERIFY_STRICT,
		9,
		true,
		"ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"
	);
	Poco::Net::HTTPSClientSession session(host, port, context);

	// Set up request
	Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
	request.setHost(host);

	// Send the request
	session.sendRequest(request);

	// Get the response
	Poco::Net::HTTPResponse response;
	std::istream& responseStream = session.receiveResponse(response);

	// Display response status
	std::cout << "Response: " << response.getStatus() << " " << response.getReason() << std::endl;

	// Print the response body
	std::ofstream ofs("../../test/get.html");
	Poco::StreamCopier::copyStream(responseStream, ofs);

}
catch (const Poco::Exception& ex) {
	std::cerr << "Poco Exception: " << ex.displayText() << std::endl;
}
catch (const std::exception& ex) {
	std::cerr << "Standard Exception: " << ex.what() << std::endl;
}
}
bool doRequest(Poco::Net::HTTPSClientSession& session,
	Poco::Net::HTTPRequest& request,
	Poco::Net::HTTPResponse& response)
{
	session.sendRequest(request);
	std::istream& rs = session.receiveResponse(response);
	std::cout << response.getStatus() << " " << response.getReason() << std::endl;
	if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
	{

		Poco::StreamCopier::copyStream(rs, std::cout);
		return true;
	}
	else
	{
		//it went wrong ?
		return false;
	}
}
int crequest() {
	Poco::URI uri("https://stackoverflow.com/questions/3260296/error-w-c-poco-and-httpsstreamfactory");
	std::string path(uri.getPathAndQuery());
	if (path.empty()) { path = "/"; } 
	const Poco::Net::Context::Ptr context = new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
	std::string str1, str2, str3;
	Poco::Net::Context::Ptr context2 = new Poco::Net::Context(
		Poco::Net::Context::TLS_CLIENT_USE,
		str1, str2, str3,
		Poco::Net::Context::VERIFY_STRICT,
		9,
		false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"
	);
	Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), context2);
	HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
	HTTPResponse response;

	if (!doRequest(session, request, response))
	{
		std::cerr << "Invalid username or password" << std::endl;
		return 1;
	}
}
int main(int argc, char* argv[])
{
	std::locale::global(std::locale(".UTF8"));
	system("node C:/Users/ciger/Desktop/kl/a.js");
	system("pause");
	sendHTTPSRequest("https://bilgisayarmuhendislik.iuc.edu.tr/tr/duyurular/1/1");

	greetings();

	HTTPServerApp app;
	return app.run(argc, argv);
	
	
	system("pause");
}
#endif

#ifdef __linux__
int main()
{

}
#endif