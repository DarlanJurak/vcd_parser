/***
*   @Author: Augusto Moraes and Darlan Alves Jurak
*   @Date: September, 3 - 2016
*   @Brief:
*       This class is used to store data about the system`s signal modeling.
*/
#ifndef SIGNAL_H
#define SIGNAL_H

#include <string>

using namespace std;

class Signal
{
    public:

        Signal(); //ctor
        Signal(string symbol, string name, string module); //ctor
        virtual ~Signal(); //dtor

        // gets
        string  getSymbol(){return symbol;};
        char    getState(){return state;};
        int     getIdleTime_longer(){return idleTime_longer;};
        string  getName(){return name;};
        int     getTransitions(){return transitions;};

        // sets
        void setState(char state){this->state = state;};

        // 
        void idleCounting();
        void update(char state);
        void inc_idleTime_current(){idleTime_current++;};

    protected:
    private:

        string symbol;
        string name;
        string module;
        char state;
        int idleTime_current;
        int idleTime_longer;
        int transitions;

};

#endif // SIGNAL_H
