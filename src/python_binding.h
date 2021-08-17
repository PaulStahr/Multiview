#ifndef PYTHON_BINDING
#define PYTHON_BINDING

#include <iostream>
#include "session.h"

void api_call();

void diffrot();

int run_testscript (session_t *session);

namespace PYTHON{
void run(std::string const & file, session_t *session);
}

#endif
