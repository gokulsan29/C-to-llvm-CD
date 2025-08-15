#pragma once
#include <cassert>
#include <iostream>

using namespace std;

#define NOT_REACHED() do { cout << "not-reached" << endl; assert(false); } while(0)
#define NOT_IMPLEMENTED() do { cout << "not-implemented" << endl; assert(false); } while(0)

