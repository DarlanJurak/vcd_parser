/***
*   @Author: Augusto Moraes and Darlan Alves Jurak
*   @Date: September, 7 - 2016
*   @Brief:
*       Signal`s methods implementation.
*/
#include <iostream>
#include <string>
#include "../include/Module.h"

using namespace std;

Module::Module(){ //ctor


} 

Module::Module(string name){ //ctor

	this->name = name;

}

void Module::addSignal(Signal* signal){

	bool alreadyRegistered = false;

	//Verify if signal is already registered
	for(vector<Signal*>::iterator it = signals.begin(); it != signals.end(); ++it) {

        if( (*it)->getName() == signal->getName() ){

            alreadyRegistered = true;
            break;

        }

    }

    // 
    if( alreadyRegistered == false ){

    	signals.push_back(signal);

    }

}

int Module::signalsAmount(){

    return signals.size();

}

Module::~Module(){ //dtor


}
