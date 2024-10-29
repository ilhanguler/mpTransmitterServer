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


using namespace Poco::Net;

using namespace Poco::Data::Keywords;

class SessionManager {
public:
    SessionManager() = default;
    
    // Create a new session for a user
    std::string createSession(const std::string& username) {
        std::lock_guard<std::mutex> lock(sessionMutex);

        // Generate a unique session token
        Poco::UUID uuid = Poco::UUIDGenerator().create();
        std::string token = uuid.toString();

        // Store the session
        sessions[token] = { username, std::chrono::steady_clock::now() };
        return token;
    }

    // Validate a session token
    bool validateSession(const std::string& token) {
        std::lock_guard<std::mutex> lock(sessionMutex);
        auto it = sessions.find(token);
        if (it != sessions.end()) {
            // Optionally, check session expiration here
            it->second.lastActive = std::chrono::steady_clock::now();
            return true;
        }
        return false;
    }

    // Clear expired sessions
    void cleanupSessions() {
        std::lock_guard<std::mutex> lock(sessionMutex);
        auto now = std::chrono::steady_clock::now();
        for (auto it = sessions.begin(); it != sessions.end();) {
            // Check for session expiration (e.g., 30 min timeout)
            if (std::chrono::duration_cast<std::chrono::minutes>(now - it->second.lastActive).count() > 30) {
                it = sessions.erase(it);
            }
            else {
                ++it;
            }
        }
    }

private:
    struct Session {
        std::string username;
        std::chrono::steady_clock::time_point lastActive;
    };

    std::unordered_map<std::string, Session> sessions;
    std::mutex sessionMutex;
};



class AuthManager {
public:
    AuthManager() {
        // Register the SQLite connector
        Poco::Data::SQLite::Connector::registerConnector();

        // Create a session to the SQLite database
        try {
            session = new Poco::Data::Session("SQLite", "users.db");

            // Create the 'users' table if it doesn't exist
            *session << "CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT)", now;
        }
        catch (Poco::Exception& ex) {
            std::cerr << "Database error: " << ex.displayText() << std::endl;
        }
    }

    ~AuthManager() {
        if (session) {
            delete session;
            session = nullptr;
        }
        Poco::Data::SQLite::Connector::unregisterConnector();
    }

    bool registerUser(std::string& username, std::string& password) {
        if (userExists(username)) {
            std::cerr << "User already exists." << std::endl;
            return false;
        }

        std::string hashedPassword = hashPassword(password);
        try {
            Poco::Data::Statement insert(*session);
            insert << "INSERT INTO users (username, password) VALUES(?, ?)",
                use(username), use(hashedPassword);  // Removed `now` to separate execution
            insert.execute(); // Execute separately
            return true;
        }
        catch (Poco::Exception& ex) {
            std::cerr << "Error registering user: " << ex.displayText() << std::endl;
            return false;
        }
    }

    bool authenticateUser(std::string& username, std::string& password) {
        std::string storedHashedPassword;
        try {
            Poco::Data::Statement select(*session);
            select << "SELECT password FROM users WHERE username = ?",
                use(username), into(storedHashedPassword);
            select.execute();  // Separate execution

            return !storedHashedPassword.empty() && storedHashedPassword == hashPassword(password);
        }
        catch (Poco::Exception& ex) {
            std::cerr << "Authentication error: " << ex.displayText() << std::endl;
            return false;
        }
    }
    
    std::string authenticateAndGetSession( std::string& username,  std::string& password, SessionManager& sessionManager) {
        if (authenticateUser(username, password)) {
            return sessionManager.createSession(username);  // Create a session on successful auth
        }
        return "";  // Return empty token if authentication fails
    }

private:
    Poco::Data::Session* session = nullptr;

    bool userExists(std::string& username) {
        int count = 0;
        try {
            Poco::Data::Statement select(*session);
            select << "SELECT COUNT(*) FROM users WHERE username = ?",
                use(username), into(count);
            select.execute();
        }
        catch (Poco::Exception& ex) {
            std::cerr << "Error checking user existence: " << ex.displayText() << std::endl;
        }
        return count > 0;
    }

    std::string hashPassword(std::string& password) {
        Poco::SHA1Engine sha1;
        Poco::DigestOutputStream dos(sha1);
        dos << password;
        dos.close();
        return Poco::DigestEngine::digestToHex(sha1.digest());
    }
};




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




class FileUploadHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& req, HTTPServerResponse& res) override {
        PartHandler partHandler;
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
    class PartHandler : public Poco::Net::PartHandler {
    public:
        void handlePart(const MessageHeader& header, std::istream& stream) override {
            std::ofstream file("uploaded_file.bin", std::ios::binary);
            Poco::StreamCopier::copyStream(stream, file);
            file.close();
        }
    };
};




class StringUploadHandler : public HTTPRequestHandler {
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




class RequestHandlerFactory : public HTTPRequestHandlerFactory {
public:
    RequestHandlerFactory(AuthManager& authManager) : authManager(authManager) {}

    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override {
        std::cout << "http request received... " << request.getURI() << std::endl;
        if (request.getURI() == "/auth") {
            return new AuthRequestHandler(authManager);
        }
        else if (request.getURI() == "/uploadFile") {
            return new FileUploadHandler();
        }
        else if (request.getURI() == "/uploadString") {
            return new StringUploadHandler();
        }
        return nullptr;
    }

private:
    AuthManager& authManager;
};




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

