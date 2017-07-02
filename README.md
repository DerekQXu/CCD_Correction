# CCD_Correction

This is code that demonstrates image calibration in CCD cameras, by 
exaggerating the effects of dark current, offset, and sensitivity.
The program creates an artificial 'broken' version and a 'corrected'
(using CCD correction algorithms) version of .png images.

## Usage

Simply put a .png image in the "CCD_Correction/main" folder, name it
"rawImage.png", and run the "main.cpp" file.

## Additional

For a more in-depth correction program refer to: [engr96b-16f]
(https://github.com/cliau/engr96b-16f), which shows the varying histograms
and correction values between used and correct images.

## Libraries Used

* C++ STL
* opencv
