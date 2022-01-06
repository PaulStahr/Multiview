/*
Copyright (c) 2018 Paul Stahr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITYglas,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "io_util.h"
#include <fstream>
#include <unistd.h>
#include <boost/dll/runtime_symbol_info.hpp>

bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

namespace IO_UTIL
{
void find_and_replace_all(std::string & data, std::string const & toSearch, std::string const & replaceStr)
{
	size_t pos = data.find(toSearch);
    while( pos != std::string::npos)
	{
		data.replace(pos, toSearch.size(), replaceStr);
		pos =data.find(toSearch, pos + replaceStr.size());
	}
}
    
std::string do_readlink(std::string const& path) {
    char buff[4096];
    ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    }
    throw std::runtime_error("error, can't read link");
}

std::string get_programpath()
{
    auto tmp = boost::dll::program_location().parent_path();
    if (tmp.filename() == "built")
    {
        tmp = tmp.parent_path();
    }
    return tmp.native();
}

std::string get_selfpath() {
    char buff[4096];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    }
    throw std::runtime_error("error, can't read selfpath");
}

std::string read_file(std::string const & file)
{
    std::ifstream t(file);
    if (!t)
    {
        throw std::runtime_error("Bad input stream for file " + file);
    }
    std::string str;

    t.seekg(0, std::ios::end);   
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());
    t.close();
    return str;
}
    
bool string_to_struct< bool >::operator()(std::string const & str) const
{
    if (str == "true")
        return true;
    if (str == "false")
        return false;
    bool res;
    std::stringstream ss(str);
    if (!(ss >> res))
    {
        throw std::invalid_argument("\"" + str + "\" not castable to " + typeid(bool).name());
    }
    return res;
}

std::vector<std::vector<float> > parse_csv(std::istream & stream)
{
    std::vector<std::vector<float> > res;
    std::string line;
    size_t iline = 0;
    auto split_iter = IO_UTIL::make_split_iterator("", [](char c){return c == ' ' || c == '\t';});
    size_t last_size = 0;
    while(std::getline(stream, line))
    {
        split_iter.str(line);
        res.push_back(std::vector<float>());
        std::vector<float> & back = res.back();
        back.reserve(last_size);
        try
        {
            while (split_iter.valid())
            {
                float f = std::numeric_limits<float>::quiet_NaN();
                if (*split_iter != "NaN")
                {
                    split_iter.parse(f);
                }
                back.push_back(f);
                ++split_iter;
            }
            last_size = back.size();
        }catch(std::invalid_argument const & e)
        {
            if (iline == 0)
            {
                res.pop_back();
                continue;
            }
        }
        ++iline;
    }
    return res;
}

std::vector<size_t> parse_framelist(std::istream & stream)
{
    std::vector<size_t> res;
    std::string line;
    while(std::getline(stream, line))
    {
        res.push_back(std::stoi(line));
    }
    return res;
}

/*void split_in_args(std::vector<std::string>& qargs, std::string const & command){
    size_t len = command.length();
    bool qot = false, sqot = false;
    size_t arglen;
    for(size_t i = 0; i < len; i++) {
        size_t start = i;
        if(command[i] == '\"')      {qot = true;}
        else if(command[i] == '\'') {sqot = true;}

        if(qot)
        {
            i++;
            start++;
            while(i<len && command[i] != '\"') ++i;
            qot &= i >= len;
            arglen = i-start;
            i++;
        }
        else if(sqot)
        {
            i++;
            while(i<len && command[i] != '\'') ++i;
            sqot &= i >= len;
            arglen = i-start;
            i++;
        }
        else
        {
            while(i<len && command[i]!=' ') ++i;
            arglen = i-start;
        }
        qargs.emplace_back(command.begin() + start, command.begin() +start + arglen);
    }
    //std::cout<<qargs.size();
    if(qot || sqot) std::cout<<"One of the quotes is open\n";
}*/

void split_in_args(std::vector<std::string>& qargs, std::string const & command){
    bool quote = false;
    bool place_next = true;
    for(auto iter = command.begin(); iter != command.end(); ++iter) {
        if (*iter == ' ' && !quote)
        {
            place_next = true;
        }
        else if (*iter == '\\')
        {
            ++iter;
            if (iter == command.end())
            {
                throw std::runtime_error("Command ends with escape character");
            }
            if (place_next)
            {
                qargs.emplace_back();
                place_next = false;
            }
            qargs.back().push_back(*iter);
        }
        else if (*iter == '"')
        {
            quote = !quote;
        }
        else
        {
            if (place_next)
            {
                qargs.emplace_back();
                place_next = false;
            }
            qargs.back().push_back(*iter);
        }
    }
    if(quote){
        throw std::runtime_error("Quote was left unclosed");
    }
}
}


//print_as_struct::print_as_struct(){}

print_struct::print_struct(){}

printer_struct::printer_struct(){}
