#pragma once

#define PY_SSIZE_T_CLEAN
#ifdef __APPLE__
#include <Python/Python.h>
#elif __linux__
#include <Python.h>
#endif
#include <iostream>

PyObject * sys, * path, * ModuleString, * Module, * Dict;
PyObject * Coach_start_func, * Clock_func, * Set_motor_func, * Set_LED_func;

void coach_start() {
    Py_Initialize();
    sys = PyImport_ImportModule("sys");
    path = PyObject_GetAttrString(sys, "path");
    PyList_Append(path, PyString_FromString("../../Driver/Coachbot/"));
    ModuleString = PyString_FromString((char*) "coach_kernel_dummy");
    printf("ha?\n");

    Module = PyImport_Import(ModuleString);
    Dict = PyModule_GetDict(Module);
    printf("ha?\n");

    Coach_start_func = PyDict_GetItemString(Dict, "coach_start");
    Clock_func = PyDict_GetItemString(Dict, "clock");
    Set_motor_func = PyDict_GetItemString(Dict, "set_motor");
    Set_LED_func = PyDict_GetItemString(Dict, "set_LED");
    printf("ho?\n");
    PyObject_CallObject(Coach_start_func, NULL);
}

unsigned int python_get_clock() {
    PyObject * Result = PyObject_CallObject(Clock_func, NULL);
    return (unsigned int) PyInt_AsLong(Result);
}

void python_set_color(int red, int green, int blue) {

}

void python_set_motors(int left, int right) {

}