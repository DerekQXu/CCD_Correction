# CCD_Correction

This code is used for demonstrating image calibration in CCD cameras, by 
exaggerating the effects of dark current, offset, and sensitivity. Given
a .png image, the program creates an artificial 'broken' version of the
image, and a 'corrected' version after applying correction algorithms.

##Usage

Simply put a .png image in the "CCD_Correction/main" folder, name it
"rawImage.png", and run the "main.cpp" file.

##Additional

For a more in-depth correction program refer to: [engr96b-16f]
(https://github.com/cliau/engr96b-16f), which shows the varying histograms
and correction values used to correct images.

##Libraries Used

*C++ STL
*opencv
