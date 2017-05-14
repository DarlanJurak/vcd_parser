/***
*   @Author: Augusto Moraes and Darlan Alves Jurak
*   @Date: September, 3 - 2016
*   @Brief:
*       Signal`s methods implementation.
*/
#include <iostream>
#include <string>
#include "../include/Signal.h"

using namespace std;

Signal::Signal()
{
    //ctor
}

Signal::Signal(string symbol, string name, string module){

    this->symbol = symbol;
    this->name = name;
    this->module = module;

	idleTime_current = 0;
	idleTime_longer = 0;

    transitions = 0;

}

Signal::~Signal()
{
    //ctor
}

void Signal::idleCounting(){

	if( state == '0' ){

		idleTime_current++;
		//cout << ": " << idleTime_current << endl;
		if( idleTime_current >  idleTime_longer ){ // Update current longer period in idle

	        idleTime_longer = idleTime_current;

	    }

	}else{

		//cout << " NO IDLE" << endl;

	}

}
void Signal::update(char state){

	if( state != this->state){

		transitions++;
		//cout << this->name << " " << this->symbol << " " << transitions << endl;

	}

	if( this->state == '0' ) { // Signal was in idle

	    if( idleTime_current >  idleTime_longer ){ // Update current longer period in idle

	        idleTime_longer = idleTime_current;

	    }

	}

	idleTime_current = 0;
	this->state = state; // Update state

}