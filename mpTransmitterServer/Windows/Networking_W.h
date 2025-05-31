#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>


#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/PartHandler.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Statement.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/DigestStream.h>
#include <Poco/SHA1Engine.h>
#include <Poco/StreamCopier.h>
#include <Poco/Data/TypeHandler.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/File.h>
#include <Poco/Net/HTTPServerSession.h>
#include <Poco/URI.h>
#include <Poco/Net/HTTPSClientSession.h>

#include "SessionAuth_W.h"
#include "HTTP_R_Handlers_W.h"

using namespace Poco::Net;

using namespace Poco::Data::Keywords;



class HTTPServerApp : public Poco::Util::ServerApplication {
protected:
	int main(const std::vector<std::string>& args) override {

		ServerSocket svs(8080);

		HTTPServerParams* params = new HTTPServerParams;
		params->setMaxQueued(100);
		params->setMaxThreads(100);
		
		AuthManager authManager;

		HTTPServer server(new RequestHandlerFactory(authManager), svs, params);
		server.start();

		std::cout << "Server started on http://localhost:8080" << std::endl;
		
		waitForTerminationRequest();
		server.stop();
		return Application::EXIT_OK;
	}
};



#endif

