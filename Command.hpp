#ifndef COMMAND_HPP
# define COMMAND_HPP

//#include "Server.hpp"
//#include "Client.hpp"
#include <array>
#include <iostream>

class Server;
class Client;

class Command{
	public:
		Command(void);
		virtual ~Command(void);
 		Command(Server *s, std::string type) : server(s), _type(type) {};
		virtual void exec(std::string params, Client &c) = 0;


	protected:
		Server			*server;
		std::string		_type;
		virtual std::array<std::string, 2>	separateParamsAndMessage(std::string params);
};
/*
 * buscar nombre para comandos que necesitan que el cliente esté registrado
 * QualifiedCommands, abstracta. implementaría exec().
 * en exec() se ejecuta c.isRegistered(). si no estás registrado, o le echamos o 
 * le respondemos con ERR_NOTREGISTERED(). ahí finaliza la ejecución de ese comando
 *
 * si está registrado, exec llama a otra función abstracta execQualified, 
 * que va a ser abstracta dentro de QualifiedCommands. Todos los comandos que 
 * hereden de QualifiedCommands van a implementar ese execQualified (equivalente
 * al exec() actual).
 *
 * NotQualifiedCommand
*/

#endif
