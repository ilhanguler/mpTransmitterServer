// mpTransmitterServer.cpp : Defines the entry point for the application.
//

#include "mpTransmitterServer.h"

// Greetings platform specific
#ifdef __linux__
void greetings() {
    std::cout << "Hello Linux, server is starting..." << std::endl << "CPP version: " << __cplusplus << std::endl;
}
#endif
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    void greetings() {
        std::cout << "Hello Windows, server is starting" << std::endl << "CPP version: " << _MSVC_LANG << std::endl;
    }
#endif
// Greetings platform specific

 
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
int main(int argc, char* argv[])
{
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