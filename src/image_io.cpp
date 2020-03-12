#include "image_io.h"

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

void image_io_init(){
    OPENEXR_IMF_INTERNAL_NAMESPACE::setGlobalThreadCount (14);
}

using namespace OPENEXR_IMF_INTERNAL_NAMESPACE;
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


 void    writeRgba1 (const char fileName[],
                     const Rgba *pixels,                
                     int width,                
                     int height)    
 {        
     RgbaOutputFile file (fileName, width, height, WRITE_RGBA);      // 1
     file.setFrameBuffer (pixels, 1, width);                         // 2        
     file.writePixels (height);                                      // 3    
}

void    writeGZ1 (const char fileName[],           
                  const half *gPixels,          
                  const float *zPixels,           
                  int width,        
                  int height)   
{      
    Header header (width, height);    
                                // 1        
    header.channels().insert ("G", Channel (HALF));                   // 2   
    header.channels().insert ("Z", Channel (FLOAT));                  // 3    
    OutputFile file (fileName, header);                               // 4   
    FrameBuffer frameBuffer;                                          // 5     
    frameBuffer.insert ("G",                                // name   // 6     
                        Slice (HALF,                        // type   // 7      
                               (char *) gPixels,            // base   // 8        
                               sizeof (*gPixels) * 1,       // xStride// 9         
                               sizeof (*gPixels) * width)); // yStride// 10       
    frameBuffer.insert ("Z",                                // name   // 11      
                        Slice (FLOAT,                       // type   // 12      
                               (char *) zPixels,            // base   // 13     
                               sizeof (*zPixels) * 1,       // xStride// 14      
                               sizeof (*zPixels) * width)); // yStride// 15      
    file.setFrameBuffer (frameBuffer);                                // 16     
    file.writePixels (height);                                        // 17   
    
}

void writeGZ1 (std::string const & fileName,          
                  const float *red,           
                  size_t width,        
                  size_t height)   
{      
    Header header (width, height);    
                                // 1        
    header.channels().insert ("R", Channel (FLOAT));                   // 2    
    OutputFile file (fileName.c_str(), header);                               // 4   
    FrameBuffer frameBuffer;                                          // 5     
    frameBuffer.insert ("R",                                // name   // 6     
                        Slice (FLOAT,                        // type   // 7      
                               (char *) red,            // base   // 8        
                               sizeof (*red) * 1,       // xStride// 9         
                               sizeof (*red) * width)); // yStride// 10        
    file.setFrameBuffer (frameBuffer);                                // 16     
    file.writePixels (height);                                        // 17   
}

void writeGZ1 (std::string const & fileName,          
                  const float *red,          
                  const float *green,          
                  size_t width,        
                  size_t height)   
{      
    Header header (width, height);    
                                // 1        
    header.channels().insert ("R", Channel (FLOAT));                   // 2   
    header.channels().insert ("G", Channel (FLOAT));                   // 2   
    OutputFile file (fileName.c_str(), header);                               // 4   
    FrameBuffer frameBuffer;                                          // 5     
    frameBuffer.insert ("R",                                // name   // 6     
                        Slice (FLOAT,                        // type   // 7      
                               (char *) red,            // base   // 8        
                               sizeof (*red) * 1,       // xStride// 9         
                               sizeof (*red) * width)); // yStride// 10       
    frameBuffer.insert ("G",                                // name   // 11      
                        Slice (FLOAT,                       // type   // 12      
                               (char *) green,            // base   // 13     
                               sizeof (*green) * 1,       // xStride// 14      
                               sizeof (*green) * width)); // yStride// 15      
    file.setFrameBuffer (frameBuffer);                                // 16     
    file.writePixels (height);                                        // 17   
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
    header.channels().insert ("R", Channel (FLOAT));                   // 2   
    header.channels().insert ("G", Channel (FLOAT));                   // 2   
    header.channels().insert ("B", Channel (FLOAT));                   // 2   
    OutputFile file (fileName.c_str(), header);                               // 4   
    FrameBuffer frameBuffer;                                          // 5     
    frameBuffer.insert ("R",                                // name   // 6     
                        Slice (FLOAT,                        // type   // 7      
                               (char *) red,            // base   // 8        
                               sizeof (*red) * 1,       // xStride// 9         
                               sizeof (*red) * width)); // yStride// 10       
    frameBuffer.insert ("G",                                // name   // 11      
                        Slice (FLOAT,                       // type   // 12      
                               (char *) green,            // base   // 13     
                               sizeof (*green) * 1,       // xStride// 14      
                               sizeof (*green) * width)); // yStride// 15      
    frameBuffer.insert ("B",                                // name   // 11      
                        Slice (FLOAT,                       // type   // 12      
                               (char *) blue,            // base   // 13     
                               sizeof (*blue) * 1,       // xStride// 14      
                               sizeof (*blue) * width)); // yStride// 15      
    file.setFrameBuffer (frameBuffer);                                // 16     
    file.writePixels (height);                                        // 17   
}
