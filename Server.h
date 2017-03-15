#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#pragma once

class Server: public boost::enable_shared_from_this<Server> {
	private:
		boost::asio::streambuf buffer;
		std::ostringstream ss;
		std::string res, req;

	public:
		boost::asio::ip::tcp::socket socket;
		int counter;

		Server(boost::asio::io_service &io):socket(io),counter(0) {};
		static void connect(boost::shared_ptr<Server> conn);
		void writeHandler(const boost::system::error_code &err, std::size_t bytes);
		void readHeader(boost::system::error_code const &err, size_t bytes);
		void acceptHandler(const boost::system::error_code &err);
};

boost::shared_ptr<Server> listen(std::string address, std::string port);
void pollSocket();
