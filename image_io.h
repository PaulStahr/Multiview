#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include <string>

void image_io_init();

void writeGZ1 (std::string const & fileName,          
                  const float *red,           
                  size_t width,        
                  size_t height);

void writeGZ1 (std::string const & fileName,          
                  const float *red,          
                  const float *green,          
                  size_t width,        
                  size_t height);

void writeGZ1 (std::string const & fileName,          
                  const float *red,          
                  const float *green,           
                  const float *blue,           
                  size_t width,        
                  size_t height);


#endif
