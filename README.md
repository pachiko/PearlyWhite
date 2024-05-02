# PearlyWhite
## Description
A Windows C++ CLI application that displays a dental x-ray image file from local storage 
while also computing and displaying the histogram of the image.

If there is an indicator, the image is rotated so that the indicator is positioned on the top-left corner.
## Requirements
### `OpenCV >= 4.9.0`

Please follow the instructions here to install OpenCV:
https://docs.opencv.org/4.x/d3/d52/tutorial_windows_install.html#tutorial_windows_install_prebuilt

Please ensure that the environment variable `OpenCV_DIR` is pointing to `<path to OpenCV dir>/build/x64/vc16`,
as CMake will depend on that variable to configure and generate the solution.

Most of the functionality, algorithms and UI are done with OpenCV. 
 
### `Visual Studio 2019, MSVC 14.29.30133`

Please use the following link to install Visual Studio:
https://visualstudio.microsoft.com/downloads/

PearlyWhite has only been developed and tested on Windows and MSVC compiler.

### `CMake >= 3.29.1`

Please use the following link to install CMake:
https://cmake.org/download/

CMake will be used to generate the build tools (eg. Visual Studio solution file on Windows).

To generate the build for PearlyWhite, start up `CMake-GUI`, select the root folder (ie. the folder containing `CMakeLists.txt`) as your `source` directory.

Next, specify the `build` directory which is usually just `<source directory>/build`.

Click on `Configure` and a window pops up. Use `Visual Studio` as the generator and `x64` as the platform. Click on `Finish`.

![alt text](https://github.com/pachiko/PearlyWhite/blob/main/media/CMake-config.png?raw=true)

Finally, click on `Generate` to create the build files. If successful, you should see something similar to the outputs below.

![alt text](https://github.com/pachiko/PearlyWhite/blob/main/media/CMake-gen.png?raw=true)

## Compile
Open Visual Studio, right-click on the `PearlyWhite` project in `Solution Explorer`. Press `Set as Startup Project`.
This ensures that PearlyWhite will be the project being compiled and run.

![alt text](https://github.com/pachiko/PearlyWhite/blob/main/media/VS-startup-proj.png?raw=true)

Alternatively, you can go to `Build` drop-down menu and press `Build PearlyWhite`. This builds the executable without running it.

![alt text](https://github.com/pachiko/PearlyWhite/blob/main/media/VS-build.png?raw=true)

## Usage
You can run the compiled executable from `PowerShell`:
```
Usage: PearlyWhite.exe [params] image

        -?, -h, --help, --usage
                prints this message
        --calibrate
                use trackbar to calibrate image thresholding

        image (value:<none>)
                path to image file
```
If `calibrate` option is passed in as argument, the application allows you to set the thresholding value and the thresholding type.
This can be useful if the dental image or indicator cannot be detected by the program using default thresholding values.

![alt text](https://github.com/pachiko/PearlyWhite/blob/main/media/PearlyWhite-calibrate.png?raw=true)

The final output and histogram should look like the following:

![alt text](https://github.com/pachiko/PearlyWhite/blob/main/media/PearlyWhite-output.png?raw=true)

![alt text](https://github.com/pachiko/PearlyWhite/blob/main/media/PearlyWhite-histo.png?raw=true)
