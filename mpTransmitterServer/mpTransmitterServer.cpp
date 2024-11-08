// mpTransmitterServer.cpp : Defines the entry point for the application.
//

#include "mpTransmitterServer.h"

// Greetings
#ifdef __linux__
void greetings() {
    std::cout << "Hello Linux, server is starting..." << std::endl << "CPP version: " << __cplusplus << std::endl;
}
#endif
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    void greetings() {
        std::cout << "Hello Windows, server is starting...\nPOCO cersion:\t" << Poco::format("%d.%d.%d",
            static_cast<int>(Poco::Environment::libraryVersion() >> 24),
            static_cast<int>((Poco::Environment::libraryVersion() >> 16) & 0xFF),
            static_cast<int>((Poco::Environment::libraryVersion() >> 8) & 0xFF)) << std::endl << "CPP version:\t" << _MSVC_LANG << std::endl;
    }
#endif
// Greetings

 
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
int main(int argc, char* argv[])
{
    //setlocale(LC_ALL, ""); // Support both cout and wcout unlike the function below (_setmode)
    //_setmode(_fileno(stdout), _O_U16TEXT); // Activates UTF-16 streaming #include <corecrt_io.h> #include <fcntl.h>
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