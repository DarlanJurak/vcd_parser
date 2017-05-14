/***
*   @Author: Augusto Moraes and Darlan Alves Jurak
*   @Date: September, 7 - 2016
*   @Brief:
*       Hardware block
*/
#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector> // Store signals
#include "Signal.h"

using namespace std;

class Module
{
    public:

        Module(); //ctor
        Module(string name); //ctor
        virtual ~Module(); //dtor

        // gets
        string getName(){return name;};

        // sets

        // 
        void addSignal(Signal* signal);
        int signalsAmount();

        vector<Signal*> signals;

    protected:
    private:

        string name;

};

#endif // MODULE_H
