#include <boost/lexical_cast.hpp>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <boost/algorithm/string/join.hpp>
#include "common.hpp"

Msg::Msg(const string msg) {
		boost::regex expr("^(\\d+)(.*)$");
		boost::smatch match;
		_malformed = true;
		if (! boost::regex_search(msg, match, expr)) {
			cerr << "message malformed\n";
			return;
		}
		
		try {
			_wait_secs = boost::lexical_cast<int>(match[1]);
		} catch (const boost::bad_lexical_cast &) {
			cerr << "error parsing wait interval\n";
			return;
		}
		_malformed = false;
		_body = match[2];
}

string Msg::reverse_body() {
	string tmp(_body);
	reverse(tmp.begin(), tmp.end());
	return tmp;
}

string Msg::reverse_words() {
	istringstream iss(_body);
	vector<string> words;
	copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(words));
	reverse(words.begin(), words.end());
	return boost::algorithm::join(words, " ");
}

map<char, int> Msg::char_freq() {
	map<char, int> freq;
	for (string::const_iterator it = _body.begin(); it != _body.end(); ++it) {
		if( freq.find(*it) == freq.end()) freq[*it] = 1;
		else ++freq[*it];
	}
	return freq;
}

string Msg::print_freq(map<char, int> freq) {
	stringstream ss;
	for (map<char, int>::const_iterator it = freq.begin(); it != freq.end(); ++it) {
		ss << it->first << it->second << " ";
	}
	return ss.str();
}
