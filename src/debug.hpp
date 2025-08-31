#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>

#ifdef DEBUG 
#define DEBUG_PRINT(x) (std::cout << (x))
#else 
#define DEBUG_PRINT(x)
#endif

#endif // DEBUG_HPP
