# SG-323
## Ursa Major Stargate 323 Emulation

This plugin is an authentic emulation of my Ursa Major Stargate 323 Reverb. It is based on over four years of work, which included a detailed analysis of the original circuit logic and filters, as well as eprom data.

This personal project is in no way affilliated or associated with Ursa Major Inc / Christopher Moore / Seven Woods Audio Inc / AKG.  
VST® is a registered trademark of Steinberg Media Technologies GmbH.

## Build It Yourself!
You can build the VST3 and CLAP releases yourself!  
*AAX requires digital signing from PACE as well as Apple & Microsoft and can not be loaded into Pro Tools. If Pro Tools is your main DAW  you can use a third party plugin wrapper like the free Waves StudioVerse Audio Effects to load the VST3.*  

### Windows 11
Prerequisites:  
Download and install Git for Windows. You can leave all the settings at default. https://git-scm.com/downloads/win  
Download and install CMake. Make sure to check the "Add CMake to the PATH environment variable" during installation. https://cmake.org/download/#latest  
Download and install Visual Studio 2022 Community and install the "Desktop Development with C++" workload. https://learn.microsoft.com/en-us/cpp/build/vscpp-step-0-installation?view=msvc-170  

Build it:  
Open a new command prompt or PowerShell window:
```
git clone --recurse-submodules https://github.com/greyboxaudio/SG-323.git
cd .\SG-323\
cmake -B build
cmake --build build --target ALL_BUILD --config Release
```
The compiled plugins will be created in `.\build\SG323_artefacts\Release\` and should be copied to your system folders automatically.  

### macOS 14
Prerequisites:  
Download and install CMake. https://cmake.org/download/#latest  
Open a terminal and add CMake to your path: `PATH="/Applications/CMake.app/Contents/bin":"$PATH"`. While you're in the terminal you also need to install the Xcode command line tools: `xcode-select --install`  

Build it:  
Open a new terminal and run the following commands  
```
git clone --recurse-submodules https://github.com/greyboxaudio/SG-323.git
cd SG-323/
cmake -B build -D CMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build --target all --config Release
```

### Ubuntu 24.04
Prerequisites:  
Open a terminal and install basic development tools as well as dependencies for JUCE
```
apt install git build-essential cmake libasound2-dev libcurl4-openssl-dev libfontconfig1-dev libfreetype-dev libwebkit2gtk-4.1-dev
```
(You can find info about the JUCE dependencies on linux here: https://github.com/juce-framework/JUCE/blob/master/docs/Linux%20Dependencies.md)  

Build it:  
Open a new terminal and run the following commands  
```
git clone --recurse-submodules https://github.com/greyboxaudio/SG-323.git
cd SG-323/
cmake -B build
cmake --build build --target all --config Release
```

## Special thanks
Dr. Albert Gräf and his team at the Computer Music Research Group http://www.musikinformatik.uni-mainz.de/e_ag.php  
Joshua Krosenbrink https://www.electroacousticlabs.com/  
Xenakios https://xenakios.wordpress.com/  
William Light https://github.com/wrl  
Florian Mrugalla https://github.com/Mrugalla  
Joshua Hodge and the whole Audio Programmer Community https://www.theaudioprogrammer.com/  
