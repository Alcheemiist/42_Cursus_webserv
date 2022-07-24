/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_confile.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: isaadi <isaadi@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/27 23:27:53 by nabboudi          #+#    #+#             */
/*   Updated: 2022/07/24 18:08:46 by isaadi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_CONFILE_HPP
#define PARSE_CONFILE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "location.hpp"
#include "cgi.hpp"

class cgi;
class location;
class server;

class parse_config
{
    protected:
        std::vector<std::string> _lines;
        std::vector<std::string> _words;
        std::vector<server> _servers;
        
    public:
        parse_config():_lines(), _words(), _servers(){};
        ~parse_config(){};
        void                start_parsing();
        void                split_by_space();
        void                accolade_error();
        void                syntax_error();
        void                check_host_server_names_error();
        void                specified_words(std::string&);
        unsigned int        server_parsing(unsigned int&);
        size_t              get_lines_size() const;
        const std::vector<server> &get_server_vect() const;
        std::vector<server> set_server_vect(std::vector<server>);
        void                set_lines(std::vector<std::string> lines);
        void                read_lines();
        void                read_server();
        std::string         get_lines(int i)const;
        int                 parsing_conf(int ac, char **av, std::vector<std::string> lines);
        int                 basic_error(std::string , char const *, std::string);
        unsigned int        fill_index(std::vector<std::string>, unsigned int);
};
int parse_main(int argc, char **argv, parse_config &conf);


#endif