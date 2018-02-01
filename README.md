## What is this repository for?

FIMTrack is a larval tracking program to acquire locomotion trajectories
and conformation information of Drosophila melanogaster larvae. It is
optimized for FIM images. FIM is an acronym for FTIR-based Imaging Method,
whereby FTIR is the short form for Frustrated Total Internal Reflection.
Details can be found in

- B. Risse, S. Thomas, N. Otto, T. Löpmeier, D. Valkov, X. Jiang, and
  C. Klämbt, "FIM, a Novel FTIR-Based Imaging Method for High
  Throughput Locomotion Analysis," PLoS ONE, vol. 8, no. 1, p. e53963,
  Jan. 2013.

- Risse, B., Jiang, X., & Klämbt, C. (2013). FIM: Frustrated Total
  Internal Reflection Based Imaging for Biomedical Applications.
  ERCIM News, 95(Image Understanding), 11-12.

- B. Risse, N. Otto, D. Berh, X. Jiang, and C. Klämbt, "FIM Imaging
  and FIMTrack: Two New Tools Allowing High-throughput and Cost
  Effective Locomotion Analysis," Journal of Visualized Experiments:
  JoVE, no. 94, p. e52207, 2014.

- B. Risse, D. Berh, N. Otto, X. Jiang, and C. Klämbt, "Quantifying subtle
  locomotion phenotypes of Drosophila larvae using internal structures
  based on FIM images," Computers in Biology and Medicine, vol. 63,
  pp. 269-276, 2015.

For more informations contact fim[AT]uni-muenster[DOT]de


## Building FIMTrack

First download [QCustomPlot](http://www.qcustomplot.com/) (ver. 1.3.2) and place the header file `qcustomplot.h` and the source files `qcustomplot.cpp` in the `Utility` folder of FIMTrack.

### Linux
These are instructions for Ubuntu/Debian systems, the packages should be similar on other distributions.

#### Dependencies
        sudo apt-get install git libopencv-dev qt5-default g++

#### Building
        git clone https://github.com/i-git/FIMTrack.git FIMTrack
        cd FIMTrack
        qmake FIMTrack.pro
        make
        cd build/release/bin
        ./FIMTrack

### OS X El Capitan 10.11 and Yosemite 10.10
We suggest to use Xcode and [Homebrew](http://brew.sh/) for building FIMTrack on Mac OS X.

#### Dependencies
* Git
    * `sudo brew install git`
* OpenCV
    * `sudo brew tap homebrew/science`
    * `sudo brew install opencv --c++11`
* Qt
    * `sudo brew tap homebrew/versions`
    * `sudo brew install qt5`
    * `sudo brew link --force qt5`

#### Building
        git clone https://github.com/i-git/FIMTrack.git FIMTrack
        cd FIMTrack
        qmake FIMTrack.pro
        make
        cd build/release/bin
        ./FIMTrack
