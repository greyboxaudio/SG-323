# SG-323
## Ursa Major Stargate 323

// This personal project is in no way affilliated or associated with Ursa Major Inc / Christopher Moore / Seven Woods Audio Inc / AKG

This plugin is an authentic emulation of my Ursa Major Stargate 323 Reverb. It is based on over four years of work, which included a detailed analysis of the original circuit logic and filters, as well as eprom data.

## Special thanks
Dr. Albert Gräf and his team at the Computer Music Research Group http://www.musikinformatik.uni-mainz.de/e_ag.php  
Joshua Krosenbrink https://www.electroacousticlabs.com/  
Xenakios https://xenakios.wordpress.com/  
William Light https://github.com/wrl  
Florian Mrugalla https://github.com/Mrugalla  
Joshua Hodge and the whole Audio Programmer Community https://www.theaudioprogrammer.com/  

## Build It Yourself!

### Ubuntu 24.04
```
apt install git build-essential cmake libasound2-dev libcurl4-openssl-dev libfontconfig1-dev libfreetype-dev libwebkit2gtk-4.1-dev
git clone --recurse-submodules https://github.com/greyboxaudio/SG-323.git
cd SG-323/
cmake -B build
cmake --build build --target all --config Release
```

You can find the latest JUCE dependencies on linux here: https://github.com/juce-framework/JUCE/blob/master/docs/Linux%20Dependencies.md

### Windows 11

### macOS 14
