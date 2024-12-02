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
#include "Poco/CountingStream.h"
#include "Poco/NullStream.h"

#include "SessionAuth_W.h"

using namespace Poco::Net;

using namespace Poco::Data::Keywords;

// Placeholder
class AuthRequestHandler : public HTTPRequestHandler {
public:
	AuthRequestHandler(AuthManager& authManager) : authManager(authManager) { }

	void handleRequest(HTTPServerRequest& req, HTTPServerResponse& res) override {
		HTMLForm form(req, req.stream());

		if (form.has("username") && form.has("password")) {
			std::string username = form.get("username");
			std::string password = form.get("password");

			if (authManager.authenticateUser(username, password)) {
				res.setStatus(HTTPResponse::HTTP_OK);
				res.setContentType("text/plain");
				res.send() << "Authentication successful!";
			}
			else {
				res.setStatus(HTTPResponse::HTTP_UNAUTHORIZED);
				res.send() << "Invalid credentials!";
			}
		}
		else {
			res.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
			res.send() << "Username and password required!";
		}
	}

private:
	AuthManager& authManager;
};

class MyPartHandler : public Poco::Net::PartHandler
{
public:
	MyPartHandler() :
		_length(0)
	{
	}

	void handlePart(const MessageHeader& header, std::istream& stream)
	{
		_type = header.get("Content-Type", "(unspecified)");
		if (header.has("Content-Disposition"))
		{
			std::string disp;
			NameValueCollection params;
			MessageHeader::splitParameters(header["Content-Disposition"], disp, params);
			_name = params.get("name", "(unnamed)");
			_fileName = params.get("filename", "(unnamed)");
		}

		Poco::CountingInputStream istr(stream);
		Poco::NullOutputStream ostr;
		Poco::StreamCopier::copyStream(istr, ostr);
		_length = istr.chars();
	}

	int length() const
	{
		return _length;
	}

	const std::string& name() const
	{
		return _name;
	}

	const std::string& fileName() const
	{
		return _fileName;
	}

	const std::string& contentType() const
	{
		return _type;
	}

private:
	int _length;
	std::string _type;
	std::string _name;
	std::string _fileName;
};


// Placeholder function
class FileClientToServerHandler : public HTTPRequestHandler {
public:
	void handleRequest(HTTPServerRequest& req, HTTPServerResponse& res) override {
		MyPartHandler partHandler;
		HTMLForm form(req, req.stream(), partHandler);

		if (form.has("file")) {
			form.load(req, req.stream(), partHandler);
			res.setStatus(HTTPResponse::HTTP_OK);
			res.setContentType("text/plain");
			res.send() << "File received successfully!";
		}
		else {
			res.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
			res.send() << "File upload failed!";
		}
	}

private:
	class AltPartHandler : public Poco::Net::PartHandler {
	public:
		void handlePart(const MessageHeader& header, std::istream& stream) override {
			std::ofstream file("uploaded_file.bin", std::ios::binary);
			Poco::StreamCopier::copyStream(stream, file);
			file.close();
		}
	};
};



// Placeholder function
class StringClientToServerHandler : public HTTPRequestHandler {
public:
	void handleRequest(HTTPServerRequest& req, HTTPServerResponse& res) override {
		std::string data;
		Poco::StreamCopier::copyToString(req.stream(), data);
		
		// Here, handle the uploaded string data (e.g., save to file or database)
		std::cout << "Received string data: " << data << std::endl;

		res.setStatus(HTTPResponse::HTTP_OK);
		res.setContentType("text/plain");
		res.send() << "String data received successfully!";
	}
};


class StringServerToClientHandler : public HTTPRequestHandler {
public:
	void handleRequest(HTTPServerRequest& req, HTTPServerResponse& res) override {
		res.setStatus(HTTPResponse::HTTP_OK);
		res.setContentType("text/event-stream");
		res.setChunkedTransferEncoding(true);
		res.setKeepAlive(true);
		std::ostream& ostr = res.send();
		for (int i=0;i<100;i++) {
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			ostr << fetchString();
		}
	}

	std::string fetchString() {
		return "Placeholder String\n";
	}
};


class FileServerToClientHandler : public HTTPRequestHandler {
public:
	FileServerToClientHandler() {}

	void handleRequest(HTTPServerRequest& req, HTTPServerResponse& res) override {
		
		res.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		res.setContentType("video/mkv");
		res.setChunkedTransferEncoding(true);
		res.set("Content-Disposition", "attachment; filename=\"file.mkv\"");

		std::ifstream fileStream("test/downloadedFileMovie.mkv", std::ifstream::binary);
		char* buffer = new char[16777216]; // 16 megabyte of heap allocated
		std::ostream& ostr = res.send();
		unsigned int cycle = 0;
		
		auto start = std::chrono::high_resolution_clock::now();
		
		for (; fileStream.good();cycle++) {
			if (ostr.good()) {
				app.logger().information("Block " + std::to_string(cycle) + "...");

				fileStream.read(buffer, 16777216);
				ostr.write(buffer, fileStream.gcount());

				app.logger().information("Block " + std::to_string(cycle) + " is sent!");
			}
			else {
				res.setKeepAlive(false);
				app.logger().warning("Request aborted!");
				delete [] buffer;
				return;
			}
		}

		fileStream.close();
		delete [] buffer;

		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = duration_cast<std::chrono::milliseconds>(stop - start);

		app.logger().information("Read cycle:\t" + std::to_string(cycle));
		app.logger().information("Transfer time:\t" + std::to_string(duration.count()) + " milliseconds");
		app.logger().information("End of file transfer...");
		
	}


	Poco::Util::Application& app = Poco::Util::Application::instance();
};

class SetServiceHandler : public HTTPRequestHandler {
public:
	SetServiceHandler() {}

	void handleRequest(HTTPServerRequest& req, HTTPServerResponse& res) override {

		MyPartHandler partHandler;
		HTMLForm form(req, req.stream(), partHandler);

		res.setChunkedTransferEncoding(true);
		res.setContentType("text/html");

		std::ostream& ostr = res.send();

		ostr <<
			"<html>\n"
			"<head>\n"
			"<title>POCO Form Server Sample</title>\n"
			"</head>\n"
			"<body>\n"
			"<h1>POCO Form Server Sample</h1>\n"
			"<h2>GET Form</h2>\n"
			"<form method=\"GET\" action=\"/form\">\n"
			"<input type=\"text\" name=\"text\" size=\"31\">\n"
			"<input type=\"submit\" value=\"GET\">\n"
			"</form>\n"
			"<h2>POST Form</h2>\n"
			"<form method=\"POST\" action=\"/form\">\n"
			"<input type=\"text\" name=\"text\" size=\"31\">\n"
			"<input type=\"submit\" value=\"POST\">\n"
			"</form>\n"
			"<h2>File Upload</h2>\n"
			"<form method=\"POST\" action=\"/form\" enctype=\"multipart/form-data\">\n"
			"<input type=\"file\" name=\"file\" size=\"31\"> \n"
			"<input type=\"submit\" value=\"Upload\">\n"
			"</form>\n";

		ostr << "<h2>Request</h2><p>\n";
		ostr << "Method: " << req.getMethod() << "<br>\n";
		ostr << "URI: " << req.getURI() << "<br>\n";
		NameValueCollection::ConstIterator it = req.begin();
		NameValueCollection::ConstIterator end = req.end();
		for (; it != end; ++it)
		{
			ostr << it->first << ": " << it->second << "<br>\n";
		}
		ostr << "</p>";

		if (!form.empty())
		{
			ostr << "<h2>Form</h2><p>\n";
			it = form.begin();
			end = form.end();
			for (; it != end; ++it)
			{
				ostr << it->first << ": " << it->second << "<br>\n";
			}
			ostr << "</p>";
		}

		if (!partHandler.name().empty())
		{
			ostr << "<h2>Upload</h2><p>\n";
			ostr << "Name: " << partHandler.name() << "<br>\n";
			ostr << "File Name: " << partHandler.fileName() << "<br>\n";
			ostr << "Type: " << partHandler.contentType() << "<br>\n";
			ostr << "Size: " << partHandler.length() << "<br>\n";
			ostr << "</p>";
		}
		ostr << "</body>\n";
	}
};

class RequestHandlerFactory : public HTTPRequestHandlerFactory {
public:
	RequestHandlerFactory(AuthManager& authManager) : authManager(authManager) {}

	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override {
		app.logger().information("HTTP Request Recevied:\t" + request.getURI());
		app.logger().information("Method:\t" + request.getMethod());
		app.logger().information("IP Adress:\t" + request.clientAddress().toString() + "\n");

		
		if (request.getURI() == "/login") {
			return new AuthRequestHandler(authManager);
		}
		else if (request.getURI() == "/uploadFile") {
			return new FileClientToServerHandler();
		}
		else if (request.getURI() == "/uploadString") {
			return new StringClientToServerHandler();
		}
		else if (request.getURI() == "/signUp") {
			
		}
		else if (request.getURI() == "/downloadFile") {
			return new FileServerToClientHandler();
		}
		else if (request.getURI() == "/downloadString") {
			return new StringServerToClientHandler();
		}
		else if (request.getURI() == "/setService") {
		}
		else if (request.getURI() == "/favicon.ico") {
			
		}
		return new SetServiceHandler();
		return nullptr;
	}

private:
	AuthManager& authManager;
	Poco::Util::Application& app = Poco::Util::Application::instance();
};

#endif