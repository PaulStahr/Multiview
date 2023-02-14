#include <deque>

#ifndef TEXTURE_H
#define TEXTURE_H

struct texture_t
{
    std::string _name;
    size_t _width;
    size_t _height;
    size_t _channels;
    GLuint _datatype;
    size_t _id;
    bool _defined;
    std::shared_ptr<gl_texture_id> _tex;
    
    texture_t() : _defined(false), _tex(nullptr){}
};

class texture_cache
{
private:
    std::deque<std::unique_ptr<texture_t> > _textures;
public:
    void add (std::unique_ptr<texture_t> ){
        
    }
};

#endif
