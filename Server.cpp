/*
	Boost.Asio - Simple http server
	Boost version: 1.55.0

Compile: g++ test.cpp -o test -lboost_system -lpthread -lboost_thread && ./test
*/
#include <iostream>
#include <string>
#include <boost/bind.hpp>

#include "Server.h"

boost::asio::io_service io;
boost::asio::ip::tcp::acceptor acceptor(io);
boost::asio::ip::tcp::resolver resolver(io);

void Server::connect(boost::shared_ptr<Server> conn) {
	acceptor.async_accept(conn->socket,
		boost::bind(&Server::acceptHandler, conn, boost::asio::placeholders::error));
};

void Server::writeHandler(const boost::system::error_code &err, std::size_t bytes) {
	socket.close();
	connect(shared_from_this());
};

void Server::acceptHandler(const boost::system::error_code &err) {
	boost::asio::async_read_until(socket, buffer, "\r\n\r\n",
		boost::bind(&Server::readHeader, shared_from_this(),
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
};

void Server::readHeader(boost::system::error_code const &err, size_t bytes) {
	ss << &buffer;
	req = ss.str();
	std::cout << req << std::endl;
	std::cout << "server class = " << shared_from_this()->counter << std::endl;

	std::string content = "Count = " + std::to_string(counter) + "\r\n";
	res = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;

	boost::asio::async_write(socket, boost::asio::buffer(res),
		boost::asio::transfer_all(), boost::bind(&Server::writeHandler, shared_from_this(),
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
};

boost::shared_ptr<Server> listen(std::string address, std::string port) {
	std::cout << "Listening..." << std::endl;
	boost::asio::ip::tcp::resolver::query query(address, port);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

	acceptor.open(endpoint.protocol());
	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor.bind(endpoint);
	acceptor.listen();
	boost::shared_ptr<Server> conn(new Server(io));
	Server::connect(conn);
	return conn;
};

void pollSocket() {
	io.poll();
};

