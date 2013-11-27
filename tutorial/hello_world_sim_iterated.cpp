#include <string>
#include "../sim_main.h"

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
    sim.get("name",name);
  };
  
  void execute(Sim& sim) {
    sim.io <<greeting<<" "<<name<<"!"<<Endl;
  };
  
  void finalize(Sim& sim) {};
};
