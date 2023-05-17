# Hscpp
an user-friendly C++ interface of Hyperscan

## get start
assuming that you have installed hyperscan, 
```
git clone https://github.com/Echidna-sxh/Hscpp.git
cd Hscpp
mkdir build && cd build
cmake ..
make
make install
```
now, you have a static library of hscpp

we have some cmake variables that may be useful, please refer to the top CMakeLists.txt 

## usage
```
#include "hs_matcher.h"
int main()
{
    Echidna::HsMatcher matcher;
    matcher.push_back("tian.*?xia");
    matcher.Match("tianzhisuoxia");
    return 0;
}
```
just this esay way.