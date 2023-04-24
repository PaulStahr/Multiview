#include "gl_util.h"

void setEnabled(GLuint option, bool activated)
{
    if (activated)
    {
        glEnable(option);
    }
    else
    {
        glDisable(option);
    }
}
