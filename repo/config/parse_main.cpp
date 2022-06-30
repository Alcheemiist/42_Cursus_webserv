/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nabboudi <nabboudi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/28 02:15:42 by nabboudi          #+#    #+#             */
/*   Updated: 2022/06/29 00:35:01 by nabboudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <streambuf>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include "server.hpp"
#include "parse_confile.hpp"


int basic_error(std::string error_message, char const *av, std::string error_message2)
{
    if (av && error_message2 != "" && error_message != "")
        std::cout << error_message << av << error_message2 << std::endl;
    else if (av && error_message != "")
        std::cout << error_message << av << std::endl;
    else if (error_message != "")
        std::cout << error_message << std::endl;    
    return (0);
}

int parse_main(int argc, char **argv, parse_config *conf)
 {
    std::ifstream           file;
    
    if (argc > 2)
        return (basic_error("Usage: ./parse_confile <path_to_config_file>", NULL, ""));
    if (argc == 2)
    {
	    file.open(argv[1]);
        if (!file.is_open())
        {
            basic_error("Error: file " , argv[1] , " not found");
            exit(1);
        }
    }
    else
    {
        file.open("./config/conf/conf.conf");
        if (!file.is_open())
            return (basic_error("Error: default conf not found", NULL, ""));
    }
	std::string line;
	std::vector<std::string> lines;
	while (std::getline(file, line))
	{
		std::replace( line.begin(), line.end(), '\t', ' ');
        size_t i = 0;
		while (i < line.size() && line[i] == ' ')
        {
			i++;
        }
		if (line != " " && line != "" && (i != line.size()))
			lines.push_back(line);	
	}
    file.close();
 	if(!conf->parsing_conf(argc, argv ,lines))
 		return (0);
        
 	// conf->read_server();

 	return (0);
 }
