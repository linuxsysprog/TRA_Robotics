#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <string>
#include <map>
#include <boost/regex.hpp>

using namespace std;

class Msg {
public:
    Msg(const string msg);
    bool malformed() { return _malformed; }
    int wait_secs() { return _wait_secs; }
    string body() { return _body; }
    string reverse_body();
    string reverse_words();
    map<char, int> char_freq();
    static string print_freq(map<char, int> freq);
    
private:
    bool _malformed;
    int _wait_secs;
    string _body;
};

#endif
