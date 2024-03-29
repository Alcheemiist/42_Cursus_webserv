
#include "parse_confile.hpp"
#include "server.hpp"
#include "../networking/elements.hpp"

//--------methods--------

size_t ParseConfig::get_lines_size() const
{
    return (_lines.size());
}

void ParseConfig::read_lines()
{
    for (std::vector<std::string>::iterator it = _lines.begin(); it != _lines.end(); ++it)
    {
        std::cout << *it << std::endl;
    }
}

void ParseConfig::accolade_error()
{
    size_t i = 0;
    int accolade = 0;
    while (i < _words.size())
    {
        if (_words[i] == "{")
            ++accolade;
        else if (_words[i] == "}")
            --accolade;
        ++i;
    }
    if (accolade != 0)
    {
        std::cout << "Error: File Not Well Formated(accolade error)" << std::endl;
        exit(1);
    }
}

void ParseConfig::split_by_space()
{
    for (std::vector<std::string>::iterator it = _lines.begin(); it != _lines.end(); ++it)
    {
        std::string line = *it;
        std::stringstream ss(line);
        std::string word;
        while (std::getline(ss, word, ' '))
        {
            if (word == "")
                continue;
            _words.push_back(word);
        }
    }
}

unsigned int ParseConfig::server_parsing(unsigned int &i)
{
    Server s;
    bool location_flag = false;
    bool cgi_flag = false;
    while (1)
    {
        if (i >= _words.size() || (_words[i] == "}" && !location_flag && !cgi_flag))
            break;
        if (_words[i] == "server_names")
            i = s.fill_name(_words, i);
        else if (_words[i] == "listen")
            /*s.set_listen(_words[i + 1]);//*/ i = s.fill_listen(_words, i);
        else if (_words[i] == "root")
            s.set_root(_words[i + 1]);
        else if (_words[i] == "allow_methods")
            i = s.fill_allowed_methods(_words, i);
        else if (_words[i] == "upload_path")
            s.set_upload_path(_words[i + 1]);
        else if (_words[i] == "index")
            i = s.fill_index(_words, i);
        else if (_words[i] == "error_page")
            s.set_error_pages(_words[i + 1], _words[i + 2]);
        else if (_words[i] == "autoindex")
            i = s.fill_autoindex(_words, i);
        else if (_words[i] == "redirection")
            s.set_redirections(_words[i + 1], _words[i + 2], "");
        else if (_words[i] == "client_max_body_size")
            s.set_client_max_body_size(_words[i + 1]);
        else if (_words[i] == "location")
            i = s.fill_location(_words, i, location_flag);
        else if (_words[i] == "cgi")
            i = s.fill_cgi(_words, i, cgi_flag);
        i++;
    }
    s.set_to_default();
    this->_servers.push_back(s);
    return i;
}

void ParseConfig::specified_words(std::string &tmp)
{
    std::string err;
    err = "Error: [";
    err += tmp;
    err += "] is not a valid word";
    err += "undefined";
    if (tmp != "server_names" && tmp != "server" && tmp != "cgi_path" && tmp != "root" &&
        tmp != "allow_methods" && tmp != "upload_path" && tmp != "index" && tmp != "error_page" && tmp != "autoindex" && tmp != "redirection" && tmp != "client_max_body_size" && tmp != "location" && tmp != "cgi" && tmp != "{" && tmp != "}" && tmp != "listen")
    {
        std::cout << err << std::endl;
        exit(1);
    }
}

void ParseConfig::syntax_error()
{
    for (std::vector<std::string>::iterator it = _lines.begin(); it != _lines.end(); ++it)
    {
        std::string line = *it;
        std::stringstream ss(line);
        std::string tmp;
        if (!line.empty())
        {
            while (std::getline(ss, tmp, ' '))
            {
                if (tmp == "")
                    continue;
                break;
            }
            specified_words(tmp);
        }
    }
}

void ParseConfig::check_host_server_names_error()
{
    size_t i = 0;
    while (i < _servers.size())
    {
        size_t k = i + 1;
        while (k < _servers.size())
        {
            if ((_servers[i].get_listen_port() == _servers[k].get_listen_port()))
            {
                size_t t = 0;
                while (t < _servers[i].get_name_size())
                {
                    size_t y = 0;
                    while (y < _servers[k].get_name_size())
                    {
                        if (_servers[i].get_name(t).compare(_servers[k].get_name(y)) == 0)
                        {
                            std::cout << "Error: there is two server have same server_name and port" << std::endl;
                            exit(1);
                        }
                        y++;
                    }
                    t++;
                }
            }
            k++;
        }
        i++;
    }
}

void ParseConfig::start_parsing()
{
    split_by_space();
    if (_words.size() == 0)
    {
        std::cout << "Error: File Not Well Formated(no word)" << std::endl;
        exit(1);
    }
    accolade_error();
    syntax_error();
    for (unsigned int i = 0; i < _words.size(); i++)
    {
        if (_words[i] == "server" && ((i + 1) < _words.size()))
        {
            i++;
            if (_words[i] == "{")
                i = ParseConfig::server_parsing(i);
        }
        else
        {
            std::cout << "Error: every server configuration must be startes by server" << std::endl;
            exit(1);
        }
    }
    check_host_server_names_error();
}

void ParseConfig::read_server()
{
    size_t i = 0;

   

    while (i < _servers.size())
    {
        std::cout << red << "--------------SERVER" << i << "---------------" << def << std::endl;
        unsigned int j = 0;
        std::cout << "server_names: ";
        while (j < this->_servers[i].get_name_size())
        {
            std::cout << this->_servers[i].get_name(j) << " ";
            j++;
        }
        std::cout << std::endl;
        std::cout << "listen_host: " << this->_servers[i].get_listen_host() << std::endl;
        std::cout << "listen_port: " << this->_servers[i].get_listen_port() << std::endl;
        std::cout << "root: " << this->_servers[i].get_root() << std::endl;
        j = 0;
        std::cout << "allowed_methods: ";
        while (j < this->_servers[i].get_allowed_methods_size())
        {
            std::cout << this->_servers[i].get_allowed_methods(j) << " ";
            j++;
        }
        std::cout << std::endl;
        std::cout << "upload_path: " << this->_servers[i].get_upload_path() << std::endl;
        j = 0;
        std::cout << "index: ";
        while (j < this->_servers[i].get_index_size())
        {
            std::cout << this->_servers[i].get_index(j) << " ";
            j++;
        }
        std::cout << std::endl;
        if (this->_servers[i].get_autoindex())
            std::cout << "auto index: on" << std::endl;
        else
            std::cout << "auto index: off" << std::endl;
        j = 0;
        while (j < _servers[i].get_error_pages_size())
        {
            std::cout << "error_pages: ";
            std::vector<std::string> tmp;
            tmp = _servers[i].get_error_pages(j);
            std::cout << tmp[0];
            std::cout << "   ";
            std::cout << tmp[1] << std::endl;
            j++;
        }
        j = 0;
        while (j < _servers[i].get_redirections_size())
        {
            std::cout << "redirections: ";
            std::vector<std::string> tmp;
            tmp = _servers[i].get_redirections(j);
            std::cout << tmp[0];
            std::cout << "   ";
            std::cout << tmp[1] << std::endl;
            j++;
        }
        std::cout << "client_max_body_size: " << this->_servers[i].get_client_max_body_size() << std::endl;
        j = 0;
        std::cout << "********location*********" << std::endl;
        while (j < _servers[i].get_location_size())
        {
            Location t = this->_servers[i].get_location(j);
            std::cout << "location_path: " << t.get_locations_path() << std::endl;
            std::cout << "upload_path: " << t.get_upload_path() << std::endl;
            std::cout << "max_body_size: " << t.get_client_max_body_size() << std::endl;
            unsigned int k = 0;
            std::cout << "root: " << t.get_root() << std::endl;
            if (t.get_autoindex())
                std::cout << "auto index: on" << std::endl;
            else
                std::cout << "auto index: off" << std::endl;
            std::cout << "index: ";
            while (k < t.get_index_size())
            {
                std::cout << t.get_index(k) << " ";
                k++;
            }
            std::cout << std::endl;
            k = 0;
            std::cout << "allowed_methods: ";
            while (k < t.get_methods_size())
            {
                std::cout << t.get_methods(k) << " ";
                k++;
            }
            // if (t.get_cgi_size() != 0)
            // {
            //     size_t m = 0;
            // std::cout << std::endl;
            // std::cout << "********CGI_LOCATION*********" << std::endl;
            // while (m < t.get_cgi_size())
            // {
            //     cgi p = t.get_cgi(m);
            //     std::cout << "cgi name:" << p.get_cgi_name() << std::endl;
            //     std::cout << "cgi_path: "<< p.get_cgi_path() << std::endl;
            //     unsigned int k = 0;
            //     k = 0;
            //     std::cout << "cgi_methods: ";
            //     while (k < p.get_cgi_methods_size())
            //     {
            //         std::cout << p.get_cgi_methods(k) << " ";
            //         k++;
            //     }
            //     std::cout << std::endl;
            //     std::cout << "***********************" << std::endl;
            //     m++;
            // }
            // }
            std::cout << std::endl;
            std::cout << green << "***********************" << def << std::endl;
            j++;
        }
        j = 0;
        std::cout << "********CGI*********" << std::endl;
        while (j < _servers[i].get_cgi_size())
        {
            Cgi t = this->_servers[i].get_cgi(j);
            std::cout << "cgi name:" << t.get_cgi_name() << std::endl;
            std::cout << "cgi_path: " << t.get_cgi_path() << std::endl;
            unsigned int k = 0;
            k = 0;
            std::cout << "cgi_methods: ";
            while (k < t.get_cgi_methods_size())
            {
                std::cout << t.get_cgi_methods(k) << " ";
                k++;
            }
            std::cout << std::endl;
            std::cout << green << "***********************" << def << std::endl;
            j++;
        }
        i++;
    }
}

int ParseConfig::basic_error(std::string error_message, char const *av, std::string error_message2)
{
    if (av && error_message2 != "" && error_message != "")
        std::cout << error_message << av << error_message2 << std::endl;
    else if (av && error_message != "")
        std::cout << error_message << av << std::endl;
    else if (error_message != "")
        std::cout << error_message << std::endl;
    return (0);
}

int ParseConfig::parsing_conf(int ac, char **av, std::vector<std::string> lines)
{
    (void)ac;
    (void)av;
    set_lines(lines);
    try
    {
        start_parsing();
    }
    catch (std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
        return (0);
    }
    return (1);
}

//--------getters-------------

const std::vector<Server> &ParseConfig::get_server_vect() const { return _servers; };

std::string ParseConfig::get_lines(int i) const
{
    return (_lines[i]);
}

char **ParseConfig::getEnv() {
	return env;
}

//--------setters-------------

void ParseConfig::setEnv(char **_env) {
	env = _env;
}

void ParseConfig::set_server_vect(std::vector<Server> servers) {
	_servers = servers;
}

void ParseConfig::set_lines(std::vector<std::string> lines)
{
    _lines = lines;
}

//--------operators--------