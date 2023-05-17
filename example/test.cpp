#include "hs_matcher.h"
int main()
{
    Echidna::HsMatcher matcher;
    matcher.push_back("tian.*?xia");
    matcher.SafeMatch("tianzhisuoxia");
    return 0;
}