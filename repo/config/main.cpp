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
// #include "src/request.hpp"
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
//#include "parse_confile.hpp"
//#include "servers.hpp"

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

 int main(int argc, char **argv)
 {
 	parse_config conf;
   
 	std::vector<server> test ;
    std::ifstream file;
    if (argc > 2)
        return (basic_error("Usage: ./parse_confile <path_to_config_file>", NULL, ""));
    if (argc == 2)
    {
	    file.open(argv[1]);
        if (!file.is_open())
            return (basic_error("Error: file " , argv[1] , " not found"));
    }
    else
    {
        file.open("./conf/conf.conf");
        if (!file.is_open())
            return (basic_error("Error: default conf not found", NULL, ""));
    }
	std::string line;
	std::vector<std::string> lines;
    std::cout << "BEFORE PARSE" << std::endl;
	while (std::getline(file, line))
	{
		std::replace( line.begin(), line.end(), '\t', ' ');
		// if (line.size() > 0 && line[0] != '#')
		// 	lines.push_back(line);
        size_t i = 0;
		while (i < line.size() && line[i] == ' ')
        {
            // std::cout << "i = " << i << " line[i] = [" << line[i] << "]" << std::endl;
			i++;
        }
		if (line != " " && line != "" && (i != line.size()))
			lines.push_back(line);	
	}
    file.close();
 	if(!conf.parsing_conf(argc, argv ,lines))
 		return (0);
	
 	conf.read_server();
 	//test = conf.get_server_vect()  ;
    // system("leeks webserver");  
 	return (0);
 }
