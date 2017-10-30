#pragma once

#include <ostream>
#include <boost/noncopyable.hpp>

class Logger : boost::noncopyable {
public:
	explicit Logger(std::ostream *ostream = nullptr);

	std::ostream* ostream() const { return ostream_; }
	std::ostream* ostream(std::ostream *ostream) { return ostream_ = ostream; }

	template<class T> Logger& operator<<(const T &param);
	Logger& operator<<(std::ostream& (*f)(std::ostream&));
	Logger& put(char ch);
	Logger& write(const char *s, std::streamsize count);
	Logger& flush();
	
private:
	std::ostream *ostream_;
};

inline Logger::Logger(std::ostream *ostream)
	: ostream_(ostream)
{}

template<class T>
Logger& Logger::operator<<(const T &param)
{
	if (ostream_) *ostream_ << param;
	return *this;
}

inline Logger & Logger::operator<<(std::ostream &(*f)(std::ostream &)) {
	if (ostream_) *ostream_ << f;
	return *this;
}

inline Logger& Logger::put(char ch) {
	if (ostream_) ostream_->put(ch);
	return *this;
}

inline Logger& Logger::write(const char *s, std::streamsize count) {
	if (ostream_) ostream_->write(s, count);
	return *this;
}

inline Logger& Logger::flush() {
	if (ostream_) ostream_->flush();
	return *this;
}
