/*  =========================================================================
    zpgutil_selftest.c - run selftests

    Runs all selftests.

    -------------------------------------------------------------------------
    Copyright (c) the Contributors as noted in the AUTHORS file.                                                             
                                                                                                                             
    This Source Code Form is subject to the terms of the GNU General Public License as published by the Free Software Foundat
    ion; either version 3 of the License, or (at your option) any later version.                                             
    See the GNU General Public License for more details                                                                      

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
    =========================================================================
*/

#include "zpgutil_classes.h"

int
main (int argc, char *argv [])
{
    bool verbose;
    if (argc == 2 && streq (argv [1], "-v"))
        verbose = true;
    else
        verbose = false;

    printf ("Running zpgutil selftests...\n");

    zpgutil_datasource_test (verbose); 
    zpgutil_session_test (verbose); 

    printf ("Tests passed OK\n");
    return 0;
}
/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
