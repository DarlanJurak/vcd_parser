/***
*   @Author: Augusto Moraes and Darlan Alves Jurak
*   @Date: September, 7 - 2016
*   @Brief: 
*       ".vcd" file interpreter
*
*/

#include <fstream> // Read .vcd file
#include <iostream> // cout
#include <vector> // Store signals
#include <string>
#include <sstream>
#include "../include/Signal.h"
#include "../include/Module.h"

using namespace std;

enum states { idle_state, timescale_state, module_state, newtime_state, starttime_state, update_state};
char p_eriod[2];

vector<string> split(const string &s, char delim);
void frequency(string);

int main()
{
    states current_state, next_state = idle_state; // Main state machine
    ifstream vcd_file ("vcd/fpu_double.vcd");
    string line, module_name, timescale, bus;
    vector<Signal*> signals;
    vector<Module*> modules;
    Signal* signal_pointer = NULL;
    Module* module_pointer;
    int transitions_most = 0, transitions_less = 0;
    vector<Signal*> mostIdle;
    vector<Signal*> mostTransitions;
    vector<Signal*> lessTransitions;
    bool alreadyRegistered = false;
    int idleTime_longer = 0;
    int zero = 0;

    if( vcd_file.is_open() ){
        
        while( getline(vcd_file, line) ){ // Read line by line

            // IDLE STATE - Reading file line by line
            if( current_state == idle_state ){

                if( line.find("$timescale") != std::string::npos ){

                    next_state = timescale_state;

                }else if( line.find("$scope") != std::string::npos ){

                    next_state = module_state;

                    // Splits line in words
                    vector<string> line_splitted = split(line, ' ');

                    module_pointer = new Module(line_splitted.at(2));
                    modules.push_back(module_pointer);


                }else if( line[0] == '#' ){

                    if( line[1] != '0'){ // #50, #100, #200, ...

                        for(vector<Signal*>::iterator it = signals.begin(); it != signals.end(); ++it) {

                            (*it)->idleCounting();

                        }

                        if (zero == 0) {

                            zero = 1;

                        }

                        next_state = update_state;

                    }else{ // #0

                        next_state = starttime_state;

                    }

                }


            // TIME SCALE STATE - Reading timescale value
            }else if( current_state == timescale_state ){

                timescale = line; // Store value
                next_state = idle_state;

            // MODULE STATE - Reading functional module details
            }else if( current_state == module_state ){

                // New wire
                if( line.find("$var") != std::string::npos ){

                    next_state = module_state;

                    //Registering wire
                    vector<string> line_splitted = split(line, ' ');
                    if( (line_splitted.at(5)).compare("$end") == 0 ){ // 1-bit signal

                        // Contructor that receives symbol, name and module
                        signal_pointer = new Signal(line_splitted.at(3), line_splitted.at(4), module_name);

                    }else{ // Bus

                        bus.append(line_splitted.at(4)); // name
                        bus.append(line_splitted.at(5)); // vector`s index
                        signal_pointer = new Signal(line_splitted.at(3), bus, module_name);
                        bus.clear();

                    }

                    signals.push_back(signal_pointer);
                    module_pointer->addSignal(signal_pointer);


                // New module
                }else if( line.find("$scope") != std::string::npos ){

                    next_state = module_state;

                    // Splits line in words
                    vector<string> line_splitted = split(line, ' ');

                    // Saves module_state name
                    module_name = line_splitted.at(2);

                    module_pointer = new Module(module_name);
                    modules.push_back(module_pointer);


                }else{

                    next_state = idle_state;

                }

            // START TIME STATE - Initializing signals
            }else if( current_state == starttime_state){

                if( line[0] == '#'){

                    for(vector<Signal*>::iterator it = signals.begin(); it != signals.end(); ++it) {
                            (*it)->idleCounting();

                    }

                    if (zero == 0) {

                            zero = 1;
                            line.copy(p_eriod,2,1);

                    }

                    next_state = update_state;

                }else{

                    // Initialize signal`s state
                    for(vector<Signal*>::iterator it = signals.begin(); it != signals.end(); ++it) {

                        if( (*it)->getSymbol().at(0) == line[1] ){

                            (*it)->setState(line[0]);

                        }

                    }

                    next_state = starttime_state;

                }

            // UPDATE STATE - Get transitions of the signals state
            }else if( current_state == update_state){
    
                if( line[0] == '#'){

                    for(vector<Signal*>::iterator it = signals.begin(); it != signals.end(); ++it) {

                            (*it)->idleCounting();

                    }

                    next_state = update_state;

                }else{ 

                    // Initialize signal`s state
                    for(vector<Signal*>::iterator it = signals.begin(); it != signals.end(); ++it) {

                        if( (*it)->getSymbol().at(0) == line[1] ){

                            (*it)->update(line[0]);

                        }

                    }

                    next_state = update_state;

                }

            }

            current_state = next_state;
        }
        vcd_file.close();
        for(vector<Signal*>::iterator it = signals.begin(); it != signals.end(); ++it) {

            (*it)->idleCounting();

        }

    }else{

        cout << "Unable to open file" << endl;

    }

    frequency(timescale);
    

    // Shows modules names and theirs signals
    for(vector<Module*>::iterator module = modules.begin(); module != modules.end(); ++module) {
        cout << "\nIn module " << (*module)->getName() << " there are: " << (*module)->signalsAmount() << " signals."<< endl;

        // Preparing control signals and structures
        idleTime_longer = 0;
        transitions_most = (*module)->signals.at(2)->getTransitions();
        transitions_less = (*module)->signals.at(2)->getTransitions();
        mostIdle.clear();
        mostTransitions.clear();
        lessTransitions.clear();
        mostIdle.push_back((*module)->signals.at(2));
        mostTransitions.push_back((*module)->signals.at(2));
        lessTransitions.push_back((*module)->signals.at(2));

        // Signals of current module
        for(vector<Signal*>::iterator signal = (*module)->signals.begin(); signal != (*module)->signals.end(); ++signal) {

            /*cout    << "   * "<< (*signal)->getName() 
                    << " " << (*signal)->getSymbol() << " " 
                    << endl;*/

            if( (*signal)->getName().compare("clock") == 0){

                continue;

            }

            if( (*signal)->getName().compare("reset") == 0){

                continue;

            }

            // Most idle time
            if( (*signal)->getIdleTime_longer() == idleTime_longer ){ // Current signal has same longer idle time period

                alreadyRegistered = false;
                for(vector<Signal*>::iterator signal2 = mostIdle.begin(); signal2 != mostIdle.end(); ++signal2) {

                    if( (*signal)->getName().compare((*signal2)->getName()) == 0){

                        alreadyRegistered = true;
                        break;

                    }

                }

                if(alreadyRegistered == false){

                    mostIdle.push_back((*signal));

                }

            }else if( (*signal)->getIdleTime_longer() > idleTime_longer ){

                mostIdle.clear();
                idleTime_longer = (*signal)->getIdleTime_longer();
                mostIdle.push_back((*signal));


            }

            if( (*signal)->getName().compare("reset") == 0){

                continue;

            }

            // Most transitions
            if( (*signal)->getTransitions() == transitions_most ){

                alreadyRegistered = false;
                for(vector<Signal*>::iterator signal2 = mostTransitions.begin(); signal2 != mostTransitions.end(); ++signal2) {

                    if((*signal)->getName().compare((*signal2)->getName()) == 0){

                        alreadyRegistered = true;
                        break;

                    }

                }

                if(alreadyRegistered == false){

                    mostTransitions.push_back((*signal));

                }

            }else if( (*signal)->getTransitions() > transitions_most ){

                mostTransitions.clear();
                transitions_most = (*signal)->getTransitions();
                mostTransitions.push_back((*signal));


            }

            // Less transitions
            if( (*signal)->getTransitions() == transitions_less ){

                alreadyRegistered = false;
                for(vector<Signal*>::iterator signal2 = lessTransitions.begin(); signal2 != lessTransitions.end(); ++signal2) {

                    if((*signal)->getName().compare((*signal2)->getName()) == 0){

                        alreadyRegistered = true;
                        break;

                    }

                }

                if(alreadyRegistered == false){

                    lessTransitions.push_back((*signal));

                }

            }else if( (*signal)->getTransitions() < transitions_less ){

                lessTransitions.clear();
                transitions_less = (*signal)->getTransitions();
                lessTransitions.push_back((*signal));


            }
        }

        cout << "> Longer idle time (do not counting reset): " << endl;
        for(vector<Signal*>::iterator it = mostIdle.begin(); it != mostIdle.end(); ++it) {

            cout    << "   * " 
                    << (*it)->getName() << ": " 
                    << (*it)->getIdleTime_longer() << " ticks."<< endl;

        }

        cout << "> Most transitions (do not counting clock): " << endl;
        for(vector<Signal*>::iterator it = mostTransitions.begin(); it != mostTransitions.end(); ++it) {

            cout    << "   * " 
                    << (*it)->getName() << ": " 
                    << (*it)->getTransitions() << " transitions."<< endl;

        }

        cout << "> Less transitions: " << endl;
        for(vector<Signal*>::iterator it = lessTransitions.begin(); it != lessTransitions.end(); ++it) {

            cout    << "   * " 
                    << (*it)->getName() << ": " 
                    << (*it)->getTransitions() << " transitions."<< endl;

        }

    }


    /*for(vector<Signal*>::iterator it = signals.begin(); it != signals.end(); ++it) {
        cout << (*it)->getSymbol() << " " << (*it)->getName() << " " << (*it)->getIdleTime_longer() << (*it)->getTransitions() << endl;
    }*/

    /*for(vector<Signal*>::iterator it = signals.begin(); it != signals.end(); ++it) {

        cout    << (*it)->getName() << " " 
                //<< (*it)->getSymbol() << " " 
                //<< (*it)->getTransitions() << " transitions " 
                << "idle: "<< (*it)->getIdleTime_longer() << endl;

    }*/

    /*for(vector<Module*>::iterator it = modules.begin(); it != modules.end(); ++it) {

        cout << "Module " << (*it)->getName() << " has " << (*it)->signalsAmount() <<  " signals" << endl;

    }*/

    return 0;
}

vector<string> split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

void frequency(string period){

    string  period_str;
    char    scale;
    string  multiplier;
    int     period_int = 0;

    for ( std::string::iterator it = period.begin(); it != period.end(); ++it){

        if( (*it >= '0') && (*it <= '9') ){

            period_str.append(&(*it));

        }else if( (*it != ' ') && (*it != '\t') ){

            scale = *it;
            break;

        }

    }

    switch (scale){

        case 'm':
            multiplier = "K Hz";
        break;

        case 'u':
            multiplier = "M Hz";
        break;

        case 'n':
            multiplier = "G Hz";
        break;

        case 'p':
            multiplier = "T Hz";
        break;

        default:
        break;

    }

    period_int = (atoi(period_str.c_str()))*(atoi(p_eriod))*2;
    cout << "Clock period: " << period_int << " " << scale << "s" << endl;
    cout << "Clock Frequency: " << (double)1/period_int << " " << multiplier << endl;

}
