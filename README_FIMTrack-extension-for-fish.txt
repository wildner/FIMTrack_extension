Table of Contents
---------------------------------------------
1. Used License
2. Used Development Setup
3. Getting the Source Code Running
4. How to Use The Tool for Fish Tracking
5. TODO-List for Developers
---------------------------------------------


1. Used License
---------------------------------------------
This FIMTrack extension for fish was developed by Manuel Wildner (master student at the University of Konstanz, Germany), by modifying the source code of FIMTrack under the conditions of the non-commercial academic licence. See "LICENSE-academic.txt" for details.
---------------------------------------------


2. Used Development Setup
---------------------------------------------
The following setup was used for development:
- Windows 7
- Visual Studio 2013
- OpenCV 2.4.11
- Qt 5.5.1
- QCustomPlot 1.3.2 (copy qcustomplot.h and qcustomplot.cpp into the folder "Utility" of this FIMTrack project)
---------------------------------------------


3. Getting the Source Code Running
---------------------------------------------
To get the source code running on the mentioned system, the following steps are needed:
- add the OpenCV path to the environment variables - for example: C:\opencv\2.4.11\build\x86\vc12\bin
- add the Qt path to the environment variables - for example: C:\Qt\Qt5.5.1\5.5\msvc2013\bin

- open the project file (.pro) and change all the paths in section "win32" to the corresponding paths of your system
- if the file structure of subdirectories should be kept, add the following command to the .pro file:
CONFIG-=flat

- open cmd in windows.
- go to the project directory.
- build the Qt project for Visual Studio 2013 using the following command:
qmake -r -spec win32-msvc2013 -tp vc FIMTrack.pro
---------------------------------------------


4. How to Use The Tool for Fish Tracking
---------------------------------------------
The basic interaction with the tool is described in the user manual provided by the developers of FIMTrack (see download section of http://fim.uni-muenster.de, last visited on February 1, 2018).

A good image quality is the basis for all further steps. So make sure the background does not change over time and the contrast is high between foreground (fish) and background.

A critical part of the tracking is the correct identification of single objects, multiple (touching) objects and noise. Most of the noise should be removed by the background subtraction, small remaining noise can be filtered out by the parameter "Min Fish Area".
The identification of the fish objects is more complicated. If the objects are expected to have the same size, the parameters "Min Fish Area" and "Max Fish Area" can be helpful. Since this is often not sufficient, more parameters can be found in the preference menu. The "Contour Filtering Parameters" currently offer three important values to adjust:
- "Max Length to Thickness Threshold" defines the maximum length of an object (determined from the spine) in comparison to its average thickness (along the spine).
- "Max Spine Length Threshold" defines the maximum length of an object in pixels (determined from the spine)
- "Max to Mean Thickness Threshold" defines the relation between the maximum thickness (along the spine) to the average thickness (along the spine) of an object
All objects which fulfill the requirements are detected as single objects and are used for further tracking. All other objects seem to be multiple touching objects and need to be processed further. Currently, the implemented methods only consider the information of a single image. More sophisticated methods need to be implemented like motion estimation or machine learning.
---------------------------------------------


5. TODO-List for Developers
---------------------------------------------
- Provide video support. Currently only single .tiff or .png files are supported. Notice that both the GUI of the preview window and the GUI of the results viewer have to be changed as well.
- Provide a more sophisticated method to solve collided contours. Consider motion extimation techniques and machine learning approaches.
- Provide the ability to take a look at the computed background image and the image after background subtraction. This facilitates the adjustment of the background subtraction parameters.

Minor Improvements:
- Improve the storage and reuse of already calculated background images. Use one background image for each image input folder (needs to be updated every time the folder list changes) - currently only one background image is used which is recalculated once a different image folder is selected. Recalculate the background image only if the parameters of the background calculation have changed - currently a click on the preferences menu already triggers the deletion of the stored background image.
- Improve the image averaging method. Currently the background image is computed by summing up all selected images and dividing them by the amount of images. This needs a big image to store. Think about adapting the background image over time to overcome slight changes of the background.
- The GUI elements of the former "coiled recognition parameters" are currently used as parameters for the contour filtering. If the coiled parameters are needed again, own GUI elements have to be added for filtering. Otherwise the GUI elements have to be renamed so avoid confusion. Remove "isCoiled = false" in class "RawLarva.cpp".
- Add more parameters for the user to filter contours. Depending on the used method to solve collided contours, add more parameters for the user to adjust.
- Valley enhancement is broken currently. If valley enhancement should be used again, the errors have to be resolved. After that, the spin box of the GUI have to be made visible again (see "hide valley threshold" in class "MainGUI.cpp").
---------------------------------------------