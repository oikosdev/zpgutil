/*  =========================================================================
    zpgutil - ZPGUTIL wrapper

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

#ifndef __zpgutil_library_H_INCLUDED__
#define __zpgutil_library_H_INCLUDED__

//  External dependencies
#include <zmq.h>
#include <czmq.h>
#include <libpq-fe.h>
//  ZPGUTIL version macros for compile-time API detection

#define ZPGUTIL_VERSION_MAJOR 0
#define ZPGUTIL_VERSION_MINOR 1
#define ZPGUTIL_VERSION_PATCH 0

#define ZPGUTIL_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define ZPGUTIL_VERSION \
    ZPGUTIL_MAKE_VERSION(ZPGUTIL_VERSION_MAJOR, ZPGUTIL_VERSION_MINOR, ZPGUTIL_VERSION_PATCH)

#if defined (__WINDOWS__)
#   if defined LIBZPGUTIL_STATIC
#       define ZPGUTIL_EXPORT
#   elif defined LIBZPGUTIL_EXPORTS
#       define ZPGUTIL_EXPORT __declspec(dllexport)
#   else
#       define ZPGUTIL_EXPORT __declspec(dllimport)
#   endif
#else
#   define ZPGUTIL_EXPORT
#endif

//  Opaque class structures to allow forward references
typedef struct _zpgutil_datasource_t zpgutil_datasource_t;
#define ZPGUTIL_DATASOURCE_T_DEFINED
typedef struct _zpgutil_session_t zpgutil_session_t;
#define ZPGUTIL_SESSION_T_DEFINED


//  Public API classes
#include "zpgutil_datasource.h"
#include "zpgutil_session.h"

#endif
/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
