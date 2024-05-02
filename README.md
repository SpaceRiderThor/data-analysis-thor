# Data Analysis Software for the THOR-SR Mission

This repository compiles all the necessary files to both convert to the required format and analyze the data gathered by the MiniPIX TPX3 CdTe detectors. It contains the MEGAlib geometry files for a single "finger", the quad detector and the whole THOR instrument, as well as the C++ program that performs the conversion from the *.t3pa* to the *.evta* format. 


## Requirements

- C++ Compiler
    - [GCC](https://gcc.gnu.org/) (Linux)
    - [MinGW](https://www.mingw-w64.org/) (Windows)
- [MEGAlib](https://megalibtoolkit.com/home.html) - Required in order to analyze the converted data.


## How to run

### Finger

Navigate to the *finger_THOR* directory: 
```
cd finger_THOR
```

Then compile and run the C++ code. On Windows:
```
g++ -o output .\data_analysis_finger.cpp
.\output.exe
```

On Linux:
```
cc data_analysis_finger.cpp -o output -std=c++17 -lstdc++ -lm
./output
```

A file named *dados.evta* will be created, which can then be loaded onto *Revan* using:
```
revan -g THOR_finger.geo.setup -f dados.evta -c THOR.cfg
```


### Quad
Navigate to the *quad_THOR* directory:
```
cd quad_THOR
```

Compile and run the C++ code. On Windows:
```
g++ -o output .\data_analysis_quad.cpp
.\output.exe
```

On Linux:
```
cc data_analysis_quad.cpp -o output -std=c++17 -lstdc++ -lm
./output
```

This will create a file named *dados.evta*, which can be loaded onto *Revan* using the same command as before:
```
revan -g THOR_finger.geo.setup -f dados.evta -c THOR.cfg
```


### Complete THOR Instrument
Go to the instrument directory:
```
cd instrument_THOR
```
Compile and run the C++ code. On Windows:
```
g++ -o output .\data_analysis_instrument.cpp
.\output.exe
```

On Linux:
```
cc data_analysis_instrument.cpp -o output -std=c++17 -lstdc++ -lm
./output
```

This will create a file named *dados.evta*, which can be loaded onto *Revan* using the same command as before:
```
revan -g THOR.geo.setup -f dados.evta -c THOR.cfg
```


### Energy conversion from ToT to KeV
if your *.t3pa* file's first line looks like this:

![Alt text](images/t3pa_tot.png ".t3pa file ToT")

Instead of like this:

![Alt text](images/t3pa_energy.png ".t3pa file Energy")

It means that your file's Energy hasn't been converted yet (Fourth column, from *ToT* to *Energy*). To do this, when you run the program using the flag *-c*:
```
.\output.exe -c 
```
or

```
./output -c
```

Whether you're on Windows or Linux. This flag will make sure the energy conversion is done and that the correct value is used.