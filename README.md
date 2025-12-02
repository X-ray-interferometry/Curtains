# XRI instrument simulator

This package provides a simulator for X-ray interferometric observations in space. It contains an end-to-end pipeline starting from mock images of compact X-ray sources.

## XRI model simulator

Contained within this package is a model simulator which allows a user to create images via simple and complex shapes with attached spectra per shape. The model simulator has a dedicated GUI, and (WIP) interfaces directly with xspec models. As this model simulator is written in C++ (and many models written in fortran), it requires the user to compile themselves.

### Building

To build the model simulator, you can perform the following commands (for linux and mac) in the terminal. We recommend having a seperate build folder to prevent issues in building contaminating the source code.

```
cd /path/to/XRImulator
mkdir build
cd build
cmake ../ModelSim
cmake --build .
```

This will build the model simulator in the Build folder. Simply run the model simulator with

```
./ModelSim
```

### Using ModelSim

ModelSim is currently designed to be mostly used for the generation of simple toy-models. We include a variation of the reltrans model of X-ray reverberation/reflection around a black hole which can be found at https://github.com/X-ray-interferometry/XRI-image-gen. This is an example of a more complex model, custom built to generate more realistic model images.

