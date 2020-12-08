#include "image_io.h"

#ifdef OPENEXR

#include <OpenEXR/ImfFrameBuffer.h>
#include <OpenEXR/ImfRgba.h>
#include <OpenEXR/ImathVec.h>
#include <OpenEXR/ImathBox.h>
#include <OpenEXR/ImfThreading.h>
#include <OpenEXR/ImfNamespace.h>
#include <OpenEXR/ImfForward.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfStringAttribute.h>
#include <OpenEXR/ImfIntAttribute.h>
#include <OpenEXR/ImfFloatAttribute.h>
#include <OpenEXR/ImfVecAttribute.h>
#include <OpenEXR/ImfBoxAttribute.h>
#include <OpenEXR/ImfStringVectorAttribute.h>
#include <OpenEXR/ImfTimeCodeAttribute.h>
#include <OpenEXR/ImfStandardAttributes.h>
#include <OpenEXR/ImfMatrixAttribute.h>
#include <OpenEXR/ImfFramesPerSecond.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/half.h>

#endif

void image_io_init(){
#ifdef OPENEXR
    OPENEXR_IMF_INTERNAL_NAMESPACE::setGlobalThreadCount (14);
#endif
}

#ifdef OPENEXR
using namespace OPENEXR_IMF_INTERNAL_NAMESPACE;
#endif
/*int awx_ScreenShot(std::string const & filename) {
    unsigned char *pixels;
    FILE *image;
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    pixels = new unsigned char[viewport[2]*viewport[3]*3];
    glReadPixels(0, 0, viewport[2], viewport[3], GL_BGR, GL_UNSIGNED_BYTE, pixels);
    char tempstring[50];
    sprintf(tempstring,filename.c_str(),1);
    if((image=fopen(tempstring, "wb"))==NULL)
    {
        return 1;
    }
    uint8_t TGAheader[12]= {0,0,2,0,0,0,0,0,0,0,0,0};
    uint8_t header[6]= {((uint8_t)(viewport[2]%256)), ((uint8_t)(viewport[2]/256)), ((uint8_t)(viewport[3]%256)), ((uint8_t)(viewport[3]/256)),24,0}; // TGA header schreiben
    fwrite(TGAheader, sizeof(unsigned char), 12, image);  // Header schreiben
    fwrite(header, sizeof(unsigned char), 6, image);
    fwrite(pixels, sizeof(unsigned char), viewport[2]*viewport[3]*3, image);
    fclose(image);
    delete [] pixels;
    return 0;
}*/

#ifdef OPENEXR
 void    writeRgba1 (const char fileName[],
                     const Rgba *pixels,                
                     int width,                
                     int height)    
 {        
     RgbaOutputFile file (fileName, width, height, WRITE_RGBA);  
     file.setFrameBuffer (pixels, 1, width);                      
     file.writePixels (height);                                  
}

void    writeGZ1 (const char fileName[],           
                  const half *gPixels,          
                  const float *zPixels,           
                  int width,        
                  int height)   
{      
    Header header (width, height);    
                                // 1        
    header.channels().insert ("G", Channel (HALF));        
    header.channels().insert ("Z", Channel (FLOAT));       
    OutputFile file (fileName, header);                    
    FrameBuffer frameBuffer;                               
    frameBuffer.insert ("G",                               
                        Slice (HALF,                       
                               (char *) gPixels,           
                               sizeof (*gPixels) * 1,      
                               sizeof (*gPixels) * width));
    frameBuffer.insert ("Z",                               
                        Slice (FLOAT,                      
                               (char *) zPixels,           
                               sizeof (*zPixels) * 1,      
                               sizeof (*zPixels) * width));
    file.setFrameBuffer (frameBuffer);                     
    file.writePixels (height);                             
    
}

void writeGZ1 (std::string const & fileName,          
                  const float *red,           
                  size_t width,        
                  size_t height)   
{      
    Header header (width, height);    
                                // 1        
    header.channels().insert ("R", Channel (FLOAT));  
    OutputFile file (fileName.c_str(), header);          
    FrameBuffer frameBuffer;                          
    frameBuffer.insert ("R",                          
                        Slice (FLOAT,                 
                               (char *) red,          
                               sizeof (*red) * 1,     
                               sizeof (*red) * width));
    file.setFrameBuffer (frameBuffer);                
    file.writePixels (height);                        
}

void writeGZ1 (std::string const & fileName,          
                  const float *red,          
                  const float *green,          
                  size_t width,        
                  size_t height)   
{      
    Header header (width, height);    
                                // 1        
    header.channels().insert ("R", Channel (FLOAT));    
    header.channels().insert ("G", Channel (FLOAT));    
    OutputFile file (fileName.c_str(), header);         
    FrameBuffer frameBuffer;                            
    frameBuffer.insert ("R",                            
                        Slice (FLOAT,                   
                               (char *) red,            
                               sizeof (*red) * 1,       
                               sizeof (*red) * width)); 
    frameBuffer.insert ("G",                            
                        Slice (FLOAT,                   
                               (char *) green,          
                               sizeof (*green) * 1,     
                               sizeof (*green) * width));
    file.setFrameBuffer (frameBuffer);                   
    file.writePixels (height);                           
}

void writeGZ1 (std::string const & fileName,          
                  const float *red,          
                  const float *green,           
                  const float *blue,           
                  size_t width,        
                  size_t height)   
{      
    Header header (width, height);    
                                // 1        
    header.channels().insert ("R", Channel (FLOAT));       
    header.channels().insert ("G", Channel (FLOAT));       
    header.channels().insert ("B", Channel (FLOAT));       
    OutputFile file (fileName.c_str(), header);        
    FrameBuffer frameBuffer;                           
    frameBuffer.insert ("R",                           
                        Slice (FLOAT,                  
                               (char *) red,           
                               sizeof (*red) * 1,      
                               sizeof (*red) * width));
    frameBuffer.insert ("G",                           
                        Slice (FLOAT,                         
                               (char *) green,             
                               sizeof (*green) * 1,         
                               sizeof (*green) * width));   
    frameBuffer.insert ("B",                                  
                        Slice (FLOAT,                         
                               (char *) blue,             
                               sizeof (*blue) * 1,         
                               sizeof (*blue) * width));   
    file.setFrameBuffer (frameBuffer);                       
    file.writePixels (height);                             
}

#endif
