#pragma once

#include "coach_driver.h"

#define PY_SSIZE_T_CLEAN
#ifdef __APPLE__
#include <Python/Python.h>
#elif __linux__
#include <Python.h>
#endif

#include <stdio.h>

extern "C" {
    int call_cpp_loop() {
        printf("Hey I am in c calling loop!\n");
        driver_loop();
    }
}