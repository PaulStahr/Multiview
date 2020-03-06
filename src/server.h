#ifndef COMMAND_SERVER
#define COMMAND_SERVER

#include <string>
#include <iostream>
#include <functional>

class CommandServer{
    public:
        void start();
        void stop();
        void run();
        void setCommandExecutor (std::function<void(std::string, std::ostream &)>);
    private:
        volatile bool running;
        void dostuff(int);
        void error(const char *msg);
        int portno;
        std::function<void(std::string, std::ostream &)> _command_execute;
};

#endif
