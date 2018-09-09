/*
 * This is Client.
 * Build: make clean; make
 * Run: ./client.sh
 **/
#include <iostream>
#include <vector>
#include <unistd.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#define BIND2(x,y) boost::bind(&Client::x, shared_from_this(),y)
#define BIND3(x,y,z) boost::bind(&Client::x, shared_from_this(),y,z)

using namespace std;
using namespace boost::asio;

typedef boost::system::error_code error_code;

io_service service;

class Client : public boost::enable_shared_from_this<Client>, boost::noncopyable {
public:
    static const int PORT = 10000;
    static const int MAX_MSG = 4096;

    Client(const string &msg) : _sock(service), _started(true), _msg(msg) {}
    
    static boost::shared_ptr<Client>
    start(ip::tcp::endpoint ep, const string &msg) {
        boost::shared_ptr<Client> client(new Client(msg));
        client->start(ep);
        return client;
    }
    
    void stop() {
        if (! _started) return;
        _started = false;
        _sock.close();
    }
    
    bool started() { return _started; }
private:
    void start(ip::tcp::endpoint ep) {
        _sock.async_connect(ep, BIND2(connect_handler, _1));
    }

    void connect_handler(const error_code &err) {
        if (err) {
            stop();
        }
        else {
            write_msg(_msg + '\0');
        }
    }
    
    void read_msg() {
        async_read(_sock, buffer(_read_buf),
            BIND3(completion_cond, _1, _2), BIND3(read_handler, _1, _2));
    }

    void read_handler(const error_code &err, size_t bytes) {
         if (! err) {
             string msg(_read_buf, bytes);
             cout << msg << endl;
         }
         stop();
    }
    
    void write_msg(const string &msg) {
        if (! started()) return;
        copy(msg.begin(), msg.end(), _write_buf);
        _sock.async_write_some(buffer(_write_buf, msg.size()), 
                                BIND3(write_handler, _1, _2));
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
    string _msg;
    char _read_buf[MAX_MSG];
    char _write_buf[MAX_MSG];
};

int main(int argc, char* argv[]) {
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), Client::PORT);
	Client::start(ep, argv[1]);
    service.run();
    return 0;
}
