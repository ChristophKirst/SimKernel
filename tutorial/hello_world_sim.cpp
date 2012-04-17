#include <iostream>
#include "../sim_main.h"

using namespace std;

class Kernel;

int main(int argc, char* argv[])
{
  SimControl<Kernel> simc;
  simc.simulate(argc, argv);
  return 0;
};

class Kernel
{
public:
  string greeting, name;
  void initialize(Sim& sim) {
    sim.get("greeting",greeting,"Hello");
    sim.get("name",name,"World");
  };
  
  void execute(Sim& sim) {
    // cout <<"Hello World!"<<endl;
    cout <<greeting<<" "<<name<<"!"<<endl;
  };
  
  void finalize(Sim& sim) {
  };
};
