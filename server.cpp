/*
 * Job interview assignment.
 * Employer: TRA Robotics    Candidate: Andrey Chislenko
 * Date:     April 2018
 * Problem statement: Write a TCP/IP client-server demo; include a test.
 * 1) Server accepts connections from Clients.
 * 2) A Client sends a message to Server containing a positive
 * integer number (a wait interval in secs) and a block of text.
 * 3) Server:
 *     a) logs text
 *     b) goes into sleep for the specified number of seconds
 *     c) forms and sends a message to Client containing:
 *         1) original text
 *         2) reversed text, by char
 *         3) reversed text, by word
 *         4) char frequency statistics
 * Environment: CentOS 6.8 x86, GCC 4.4.7, Boost 1.41
 * This is Server.
 * Build: make clean; make
 * Run: ./server
 **/
#include <iostream>
#include <unistd.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#define BIND(x,y,z)  boost::bind(&Server::x, shared_from_this(),y,z)

#include <boost/algorithm/string/join.hpp>
#include "common.hpp"

using namespace std;
using namespace boost::asio;

typedef boost::system::error_code error_code;
typedef boost::recursive_mutex::scoped_lock scoped_lock;

io_service service;

class Server : public boost::enable_shared_from_this<Server>, boost::noncopyable {
public:
    static const int PORT = 10000;
    static const int MAX_MSG = 4096;
    static const int MAX_CLIENTS = 100;

    Server() : _sock(service), _started(false) {}
    
    void start() {
        scoped_lock lock(_mutex);
        _started = true;
        read_msg();
    }
    
    void stop() {
		scoped_lock lock(_mutex);
        if (! _started) return;
        _started = false;
        _sock.close();
    }
	
    bool started() const {
        scoped_lock lock(_mutex);
        return _started; 
    }
	
    ip::tcp::socket& sock() {
        scoped_lock lock(_mutex);
        return _sock;
    }

private:
    void read_msg() {
        async_read(_sock, buffer(_read_buf),
            BIND(completion_cond, _1, _2), BIND(read_handler, _1, _2));
    }
    
    void read_handler(const error_code &err, size_t bytes) {
        if (err) stop();
        if (! started()) return;
        scoped_lock lock(_mutex);

		string raw_msg(_read_buf, bytes - 1); // strip null char
		Msg msg(raw_msg);
		string response;
		if (! msg.malformed()) {
			cout << msg.body() << endl;
			sleep(msg.wait_secs());
			vector<string> svec;
			svec.push_back(msg.body());
			svec.push_back(msg.reverse_body());
			svec.push_back(msg.reverse_words());
			svec.push_back(Msg::print_freq(msg.char_freq()));
			response = boost::algorithm::join(svec, "\n");
		} else {
			response = "malformed message: " + raw_msg;
		}
		write_msg(response + '\0');
    }
    
    void write_msg(const string &msg) {
        if (! started()) return;
        scoped_lock lock(_mutex);
		
        copy(msg.begin(), msg.end(), _write_buf);
        _sock.async_write_some(buffer(_write_buf, msg.size()), 
                                BIND(write_handler, _1, _2));
    }
    
    void write_handler(const error_code &err, size_t bytes) { read_msg(); }
    
    size_t completion_cond(const error_code &err, size_t bytes) {
        if (err) return 0;
        char *last = _read_buf + bytes;
        bool found = find(_read_buf, last, '\0') != last;
        return ! found;
    }

    ip::tcp::socket _sock;
    bool _started;
    char _read_buf[MAX_MSG];
    char _write_buf[MAX_MSG];
    mutable boost::recursive_mutex _mutex;
};

ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), Server::PORT));

void accept_handler(boost::shared_ptr<Server> server, const error_code &err) {
    server->start();
    boost::shared_ptr<Server> new_server(new Server);
    acceptor.async_accept(new_server->sock(), boost::bind(accept_handler, new_server, _1));
}

void run() { service.run(); }

int main() {
    boost::shared_ptr<Server> server(new Server);
    acceptor.async_accept(server->sock(), boost::bind(accept_handler, server, _1));
	boost::thread_group thread_pool;
	
	for ( int i = 0; i < Server::MAX_CLIENTS; ++i) {
		thread_pool.create_thread(run);
	}
	
    thread_pool.join_all();
    return 0;
}
