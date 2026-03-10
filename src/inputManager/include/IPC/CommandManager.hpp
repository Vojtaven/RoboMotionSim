#ifndef COMMAND_MANAGER_HPP
#define COMMAND_MANAGER_HPP
#include "Commands.hpp"
#include <queue>
#include "RobotState.hpp"
class CommandManager
{
	public :
		CommandManager();
		void AddCommand(const Command& cmd);
		void ClearCommands();
		void Update(RobotState state);
private:
	std::queue<Command> _commandQueue;
	Command _currentCommand = Command{};


};

#endif // !COMMAND_MANAGER_HPP
