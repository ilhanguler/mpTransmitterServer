// mpTransmitterServer.cpp : Defines the entry point for the application.
//

#include "mpTransmitterServer.h"
#include "Windows/HTML_Reader_W.h"
 
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
int main(int argc, char* argv[])
{
	std::locale::global(std::locale(".UTF8"));

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