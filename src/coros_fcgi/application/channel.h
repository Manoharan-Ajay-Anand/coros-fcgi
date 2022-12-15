#ifndef COROS_FCGI_RECORD_CHANNEL_H
#define COROS_FCGI_RECORD_CHANNEL_H

#include <string>
#include <unordered_map>

namespace coros::base {
    class Socket;
}

namespace coros::fcgi {
    class ApplicationChannel {
        private:
            std::unordered_map<std::string, std::string> variables;
        public:
            void set_variable(std::string& name, std::string& value);
            void set_variable(std::string&& name, std::string&& value);
    };
}

#endif
