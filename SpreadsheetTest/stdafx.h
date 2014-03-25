// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#define TEST_DEFINITION

#ifndef TEST_DEFINITION
#define VIRTUAL_FOR_TEST
#define PUBLIC_FOR_TEST
#else
#define VIRTUAL_FOR_TEST virtual
#define PUBLIC_FOR_TEST public:
#endif

#define NUMBER double
// boost... lexical_cast...
#define STR_TO_NUMBER std::stod

// TODO: reference additional headers your program requires here
