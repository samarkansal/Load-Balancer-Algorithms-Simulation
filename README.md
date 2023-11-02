# Load-Balancing Algorithm Simulation

EECE 5643 Load-balancing algorithms simulation.

Created by Hannah Khafif, Tyler Royere, Fizzah Shaikh, & Nicolas Tedori

## Contents

1. [Background](#background)
3. [Data](#data)
4. [Model](#model)
5. [Scripts](#scripts)
2. [Files](#files)
7. [Compiling](#compiling)
8. [Running](#running)
6. [Additional Info](#additional-info)

### Background

This project aims to evaluate multiple load-balancing algorithms on the effect of server utilization of multiple different server models.

### Data

This contains all the data files used for the simulation.

Source for the real world data come from NREL's Eagle supercomputer. Link to the data [here](https://data.nrel.gov/submissions/152).

Additionally, data from Northeastern University's Discovery cluster was provided to us. This data is not publicly available. More info available [here](https://rc.northeastern.edu/). This is the `discovery-data.csv.bz2` file, which is a trimmed down version of the original dataset featuring only relevant information necessary for this project.

### Model

These are the C++ files for the simulation to run. Currently (2020-03-12) these files are mostly just a skeleton for a basic implementation.

### Scripts

Currently there are two utility scripts. `json-data-parser.py` is used to parse the NREL data (as that is in a JSON-like format) and build a file that can be parsed in a simpler way via C++. This script is currently not fully functional and should not be used under that assumption. See the code for more info.

Similarly, `read-discovery-data.py` is designed specifically to remove unnecessary features of the data, simplifying the file. The original dataset (not included in this repo), was given as a pickled dataframe, so this needed to be read and reformatted (which is what this script does).

### Files

The files in this repo are outlined below

1. [data](./data) (See [here](#data).)
   1. [anon_jobs_2020-08.json.bz2](./data/anon_jobs_2020-08.json.bz2) - this is one month of data from the Eagle supercomputer.
   2. [discovery-data.csv.bz2](./data/discovery-data.csv.bz2) - this is the Discovery cluster data for about 8 months of submitted jobs.
   3. [test_data.json](./data/test_data.json) - primarily used to test the [JSON data parser utility](./util/json-data-parser.py).
2. [model](./model) (See [here](#model).)
   1. [Job.cpp](./model/Job.cpp) and [Job.h](./model/Job.h) - this is a class for the model's representation of a Job.
   2. [LoadBalancing.cpp](./model/LoadBalancing.cpp) and [LoadBalancing.h](./model/LoadBalancing.h) - all the load-balancing algorithms can be implemented here.
   3. [Makefile](./model/Makefile) - this will compile the code into a temporary test program.
   4. [Node.cpp](./model/Node.cpp) and [Node.h](./model/Node.h) - this represents a service node in the model.
   5. [main.cpp](./model/main.cpp) - this runs the all the top-level simulation control.
   6. rng(s)/rvgs/rvms - These files are from the class c_lib and perform all RNG needed for the program.
3. [util](./util) (See [Scripts](#scripts).)
     1. [json-data-parser.py](./util/json-data-parser.py) - this parses a JSON file with multiple JSON entries, formatting data.
     2. [plot-dist.py](./util/plot-dist.py) - This will plot the data specifically output by `serverDistribution()` in [main.cpp](./model/main.cpp).
     3. [read-discovery-data.py](./util/read-discovery-data.py) - this converts the discovery data to a useable format.

## Compiling

Compiling this code is done with the [Makefile](./model/Makefile). Running `make` will compile an executable `make.out` which can be run (see [Running](#running)). There is likely to be linker errors if the previous compilation object files aren't removed first. This can be done with `make clean` and then running `make` or `make main.out`.

Because of the above, running `make main` will run `make clean && make main.out`, completing both processes at once.

Additionally, `make run` will compile and run the program. However, so command-line input is provided, so this will not run. See [Running](#running) for more information.

## Running

Running this code can be done two ways: (1) running `make run` which will compile and run the code<sup>1</sup> or (2) executing `./main.out <nNodes> <lba_alg> <qSize> <nJobs>` once the compilation is complete. Input to the program via command-line is required. This inputs are outlined below.

| Argument | Values |
|-|-|
| `nNodes` | This is the number of nodes to be used in the simulation. This must be provided as an integer. Any reasonable number greater than 0 will run a simulation with 1 or more nodes (provided the other arguments are provided with reasonable values as well). |
| `lba_alg` | This is the algorithm used in the dispatcher in the simulation. This can be one of "rand", "round", "util", or "least", verbatim. Anything else will cause and error. |
| `qSize` | The size of the queue for the nodes. This only matters for an MQMS simulation. This must be an integer, and any value in the range [0, `INT_MAX`], provided your system has the memory for `INT_MAX` Jobs. |  
| `nJobs` | This is the total number of jobs to use in the simulation. This should also be an integer greater than 0. |

<sup>1</sup>This won't run as the command in the Makefile does not currently provide CLI to the program.

## Additional Info 

### Searching files:

There are special places listed throughout the files with specific notes. Usually in ALL CAPS, these notes will tpyically provide extra information about details of the function pertaining to its (future) design. Below is a key/legend of these notes.

| Value | Meaning |
|-|-|
| `NOTE` | This is just something to keep in mind when dealing with the code around this point. Typically may describe errors or future changes if a spec of the implementation changes |
| `TODO` | Something needs to be implemented here. Details of what to do will follow. |

### Unzipping `.bz2`

Some files are compressed using `bzip2`. See [here](https://linux.die.net/man/1/bzip2) for more info.

The quick and dirty of it is:

To compress a file
```
$ bzip2 -z filename.csv
```

To uncompress a file with `.bz2`
```
$ bzip2 -d filename.csv.bz2
```
# Load-Balancer-Algorithms-Simulation
# Load-Balancer-Algorithms-Simulation
