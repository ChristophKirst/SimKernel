# SimKernel

SimKernel controls parameter scans and iterations for any given C++ code / scientific simulation. Parameters, iteration ranges, settings and dependencies are all specified in Mathematica syntax.

Features:

* Powerful control for parameter scans of scientific software
* Works with any C++ program in minutes
* Mathematica syntax input & manipulation of parameter lists
* MPI parallel version
* Nested iterations - exploit symmetries in parameter space
* Handles any parameter dependencies during iteration 
* Function / module definitions possible in input file
* Soon to come: result dependent parameter selection

## Tutorial

This Tutorial will walk you through adding SimKernel functionality to a simple program.

Say you have program like this:

```c++
#include <stdio>

using namespace std;

int main(void) {
  cout <<"Chunky bacon!"<<endl;
  
  return 0;
}
```

The output is, of course: `Chunky bacon!`

But now you decide that you need a more flexible version of this program, so you wrap it as a SimKernel class like this:

```c++
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
  void initialize(Sim& sim) {
  };
  
  void execute(Sim& sim) {
    cout <<"Hello World!"<<endl;
  };
  
  void finalize(Sim& sim) {
  };
};
```

Note that this file is included in the repository as `tutorial/hello_world_sim.cpp`, which you can build by calling `make tutorial_sim`. When executing, it requires a control file as parameter, but for now, any empty file will do. Just create one, and run `./tutorial_sim empty_control_file.txt`.

The output is, slightly more verbose than our original version:

```
Hello World!
Sim: Simulation iteration: 1/1 done!
Sim: Simulation done!
Sim: 
Sim: Error Summary:
Sim: No errors!
Sim: Bye!
```

Now say that we want to call this with different names as parameters, and would also allow for external control of the greeting, maybe because we want to use different languages later. To do this, we simply add the strings `greeting` and `name` as a variable to our SimKernel object and have SimKernel read it in during initialization, like this:

```c++
class Kernel
{
public:
  string greeting, name;
  void initialize(Sim& sim) {
    sim.get("greeting",greeting,"Hello");
    sim.get("name",name,"World");
  };
  
  void execute(Sim& sim) {
    cout <<greeting<<" "<<name<<"!"<<endl;
  };
  
  void finalize(Sim& sim) {
  };
};
```

Note: The third parameter of `sim.get`, where we supply default values in case a variable is not listed in the respective control file. If we do not supply a default value and the variable is not found in the control file, the program will exit with an error message.

Then, we add a control file that list several names, and define an iterator. Each value of this iterator corresponds to an iteration of our main SimKernel program. For each iteration, `greeting` and `name` are read in from the control file.

```c++
greeting = "Hello";

people = {"Alice", "Bob", "Peter"};

whichone = Iterator[j,{j,0,Length[people]-1,1}];
name = people[[whichone]];
```

That's it! Now if we run `./tutorial_sim tutorial/control_file.txt`, we get the following result:

```
Sim: Starting Simulation iteration: 1/3
Hello Alice!
Sim: Simulation iteration: 1/3 done!
Sim: Starting Simulation iteration: 2/3
Hello Bob!
Sim: Simulation iteration: 2/3 done!
Sim: Starting Simulation iteration: 3/3
Hello Peter!
Sim: Simulation iteration: 3/3 done!
Sim: Simulation done!
Sim: 
Sim: Error Summary:
Sim: No errors!
Sim: Bye!
```

We believe you can see where this is going. Just paste the code you originally had into the `execute` method. Then use the medod `intialize` to load parameters from the control file, and use `finalize` to save results or to deallocate memory.

If you have ever worked with simulations or any other program that has lots of parameters, is is enough to keep track of your control files to remember or repeat what you have been doing.

If you have any questions, feature requests or bug reports, feel free to use the Issues tab above.
