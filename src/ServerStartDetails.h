#ifndef SERVER_START_DETAILS
#define SERVER_START_DETAILS

#include <string>
#include <boost/asio.hpp>


struct ServerStartDetails
{
	const boost::asio::ip::address		Address;
	const unsigned short				Port;
	const std::string					DocumentRoot;
	const int							NumberOfWorkers;
	const bool							Spin;

	/**
	 * argv[1] = IP Address,
	 * argv[2] = Port,
	 * argv[3] = DocumentRoot,
	 * argv[4] = "spin" or "block"
	 */
	ServerStartDetails(char* argv[])
	:
		Address(boost::asio::ip::make_address(argv[1])),
		Port(boost::lexical_cast<unsigned short>(argv[2])),
		DocumentRoot(argv[3]),
		NumberOfWorkers(boost::lexical_cast<int>(argv[4])),
		Spin(std::strcmp(argv[5], "spin") == 0)
	{}		

};


#endif