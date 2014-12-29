/*  =========================================================================
    zpgutil_datasource - Datasource: embedding the connection parameters to the database

    Copyright (c) the Contributors as noted in the AUTHORS file.                                                             
                                                                                                                             
    This Source Code Form is subject to the terms of the GNU General Public License as published by the Free Software Foundat
    ion; either version 3 of the License, or (at your option) any later version.                                             
    See the GNU General Public License for more details                                                                      
    =========================================================================
*/

#ifndef __ZPGUTIL_DATASOURCE_H_INCLUDED__
#define __ZPGUTIL_DATASOURCE_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif


//  @interface
//  Create a new zpgutil_datasource
ZPGUTIL_EXPORT zpgutil_datasource_t *
    zpgutil_datasource_new (zconfig_t *config);

// Export to the Postgres connextion string
// The string has to be destroyed by the client of this method
ZPGUTIL_EXPORT char* 
    zpgutil_datasource_connStr (zpgutil_datasource_t *self);


//  Destroy the zpgutil_datasource
ZPGUTIL_EXPORT void
    zpgutil_datasource_destroy (zpgutil_datasource_t **self_p);

//  Print properties of object
ZPGUTIL_EXPORT void
    zpgutil_datasource_print (zpgutil_datasource_t *self);

//  Self test of this class
ZPGUTIL_EXPORT int
    zpgutil_datasource_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
