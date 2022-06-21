#ifndef PYTHON_BINDING
#define PYTHON_BINDING

#include <iostream>
#include <string>
#include <vector>
class session_t;

void api_call();

void diffrot();

int run_testscript (session_t *session);

namespace PYTHON{
void run(std::string const & file, exec_env & env, session_t *session, std::vector<std::string> const & argv);
}

#endif
