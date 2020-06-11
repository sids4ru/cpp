//Author: Siddharth Bagai
//Email: sids4ru@gmail.com

#include<iostream>
#include <vector>
#include <string>

//state is a entry in state table
/*
* Class Name :	state
* Description : State is a entry in state table
* Parameters :	name: state name,
*				next: state that can be navigated to,
*				event: event that can happen with current state
*				visible: is this state system operated state or user operated
*/
class state {
private:
	std::string name, next, event;
	bool visible;
public:
	state(std::string name, std::string next, std::string event, bool visible = true) :name(name), next(next), event(event), visible(visible) {};
	std::string getName() { return name; }
	std::string getNext() { return next; }
	std::string getEvent() { return event; }
	bool isVisible() { return visible; }

};
/*
* Class Name :	stateTable
* Description : Inspired from boost meta state machine
*				Table based state machine is most scalable as states can enter as table entry,
*				Transitions are also table entries.
*				Each tupple entry in table contains a state object.
*				A state object defines state name,
*				Name of state it can move to,
*				An event thats possible on this state and visibility
*				There can be multiple entries with same state name, this implies an 'X' state can switch to these 'y,z,a'
*
* Further enhancements: State table(vec) is public, this needs to be handled in better way
*						Input table can be implemented to be given as XML or other formats
*						Exception handling mechanism should be implemented instead of current boolean method
*/
class stateTable {
public:
	std::vector<state> vec{
		//car states
		//current state
		state("car drive","car drive","car is driving"),
		state("man walk", "man walk", "man is walking"),
		state("signal green", "signal orange", "signal is green"),

		//car Transition
		state("car drive","car stop","car stopping from driving"),
		state("car stop","car drive","car starting to drive"),
		state("car drive","car signalStop","car stopping from driving at signal",false),
		state("car stop","car signalStop","car is contining to stop at signal",false),
		state("car signalStop","car drive","car is starting at signal",false),
		//new state
		state("car stop","car stop","car is stopped"),
		state("car signalStop","car signalStop","car is waiting at red signal",false),

		//signal states
		state("signal red", "signal green", "signal is red"),
		state("signal orange", "signal red", "signal is orange"),
		//man states
		//transition states
		state("man walk", "man stop", "man will be stopping"),
		state("man stop", "man walk", "man will be walking"),
		state("man stop", "man signalStop", "man is stoping at signal from waiting",false),
		state("man walk", "man signalStop", "man is stoping at signal from walking",false),
		state("man signalStop", "man walk", "man is walking after waiting for signal",false),
		//state
		state("man stop", "man stop", "man is not walking"),
		state("man signalStop", "man signalStop", "man is waiting at signal",false),
	};
	/*
	* Function Name :	checkStatePossible
	* Description : Check if it is possible to shift from 'X' state to 'Y'
	*/
	int checkStatePossible(std::string name, std::string next, bool visible = true) {
		for (unsigned int i = 0; i < vec.size(); i++) {
			if (vec[i].getName().compare(name) == 0 && vec[i].getNext().compare(next) == 0) {
				if (vec[i].isVisible())
					return i;
				else if (!visible)
				{
					return i;
				}
			}
		}
		return -1;
	}
	/*
	* Function Name :	setState
	* Description : returns location of current state in state table
	*/
	int setState(std::string name, std::string next, bool visible = true) {
		for (unsigned int i = 0; i < vec.size(); i++) {
			if (vec[i].getName().compare(name) == 0 && vec[i].getNext().compare(next) == 0) {
				if (vec[i].isVisible())
					return i;
				else if (!visible)
				{
					return i;
				}
			}
		}
		return -1;
	}
	/*
	* Function Name :	setState
	* Description : returns location of current state in state table
	*/
	int setState(std::string name, bool visible = true) {
		for (unsigned int i = 0; i < vec.size(); i++) {
			if (vec[i].getName().compare(name) == 0 && visible == vec[i].isVisible())
				return i;
		}
		return -1;
	}

};
/*
* Class Name :	stateMachine
* Description : Manager class for managing state machines.
*
* Parameters : stateTable, position: initial state position in state table.
* Further enhancements: Interface implementation.
*						Exception handling mechanism should be implemented instead of current boolean method.
*/
class stateMachine {
public:
	stateMachine(int position, stateTable &table) :current(position), table(table) {
		std::cout << table.vec[current].getEvent() << std::endl;
	};
	virtual ~stateMachine() {};
	/*
	* Function Name :	setState
	* Description : Sets next state
	* Parameters :	next: next state
	*				visible: If this is user state or system state
	*/

	virtual bool setState(std::string next, bool visible = true) {
		int state_position = table.checkStatePossible(table.vec[current].getName(), next, visible);
		if (state_position == -1) {
			std::cout << "Cannot switch to this state" << std::endl;
			return false;
		}
		std::cout << table.vec[state_position].getEvent() << std::endl;
		state_position = table.setState(next, next, visible);
		if (state_position == -1) {
			std::cout << "cannot find final state, input table has error";
			return false;
		}
		current = state_position;
		std::cout << table.vec[current].getEvent() << std::endl;
		return true;
	}

	std::string getState() { return table.vec[current].getName(); }
	std::string getEvent() { return table.vec[current].getEvent(); }
protected:
	int current;
	stateTable &table;
};

/*
* Class Name :	signalMachine
* Description : Manager for signal state which is a more complex state that manages triggers man and car as well
*
* Parameters : stateTable, position: initial state position in state table.
* Further enhancements: Interface implementation.
*						Exception handling mechanism should be implemented instead of current boolean method.
*/
class signalMachine :public stateMachine {
public:
	signalMachine(int position, stateTable &table) :stateMachine(position, table) {};
	~signalMachine() {};
	bool setState(std::string next) {
		std::cout << "Not impemented, use switchNext instead" << std::endl;
		return false;
	}
	/*
	* Function Name :	switchSignalToNext
	* Description : switches signal light and triggers other agents
	* Parameters :	car, man state machines
	*/
	void switchSignalToNext(stateMachine& car, stateMachine& man) {
		switchNext();
		if (getState().compare("signal red") == 0) {
			car.setState("car signalStop", false);
			man.setState("man walk", false);
		}
		else if (getState().compare("signal green") == 0)
		{
			car.setState("car drive", false);
			man.setState("man signalStop", false);
		}
	}
private:
	bool switchNext() {
		std::string next = table.vec[current].getNext();
		int state_position = table.setState(next);
		if (state_position == -1) {
			std::cout << "Cannot find next signal state input table is wrong" << std::endl;
			return false;
		}
		current = state_position;
		std::cout << table.vec[current].getEvent() << std::endl;
		return true;
	}

};

int main() {
	stateTable table;
	signalMachine signal(2, table);
	stateMachine car(0, table);
	stateMachine man(18, table);
	std::string input;

	while (input.compare("exit") != 0) {
		std::cout << "Enter agent as 'signal', 'man' or 'car' : ";
		std::cin >> input;

		if (input.compare("man") == 0) {
			std::cout << "Enter commands 'walk' or 'stop' on man: ";
			std::cin >> input;
			input = "man " + input;
			man.setState(input);
		}
		else if (input.compare("car") == 0) {
			std::cout << "Enter command 'drive' or 'stop' on car: ";
			std::cin >> input;
			input = "car " + input;
			car.setState(input);

		}
		else if (input.compare("signal") == 0) {
			std::cout << "switching light" << std::endl;
			signal.switchSignalToNext(car, man);
		}
		else
		{
			std::cout << "Agent not supported" << std::endl;
		}
		std::cout << "press any key to continue or type 'exit' to terminate: ";
		std::cin >> input;
	}
	return 0;
}
