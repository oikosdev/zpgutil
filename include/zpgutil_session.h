/*  =========================================================================
    zpgutil_session - Session with the acual connection to the Postgres database

    Copyright (c) the Contributors as noted in the AUTHORS file.                                                             
                                                                                                                             
    This Source Code Form is subject to the terms of the GNU General Public License as published by the Free Software Foundat
    ion; either version 3 of the License, or (at your option) any later version.                                             
    See the GNU General Public License for more details                                                                      
    =========================================================================
*/

#ifndef __ZPGUTIL_SESSION_H_INCLUDED__
#define __ZPGUTIL_SESSION_H_INCLUDED__


#ifdef __cplusplus
extern "C" {
#endif

extern const int MAX_QUERY_SIZE;
extern const int MAX_PARAM_SIZE;

//  @interface
//  Create a new zpgutil_session
ZPGUTIL_EXPORT zpgutil_session_t *
    zpgutil_session_new (zpgutil_datasource_t *datasource);

//  Destroy the zpgutil_session
ZPGUTIL_EXPORT void
    zpgutil_session_destroy (zpgutil_session_t **self_p);

// Set the current SQL query
ZPGUTIL_EXPORT void
    zpgutil_session_sql (zpgutil_session_t *self, const char *sql);

// Prepare a statement
ZPGUTIL_EXPORT int
    zpgutil_session_prepare (zpgutil_session_t *self, const char *stmtName, const char *sql);

// Set a query parameter
ZPGUTIL_EXPORT void
    zpgutil_session_set (zpgutil_session_t *self, const char *par);

// Select query with a char* return type
// char* to be freed by client
ZPGUTIL_EXPORT char* 
    zpgutil_session_select_one (zpgutil_session_t *self);

// Select query returning the PGresult*
// result to be cleared by client
ZPGUTIL_EXPORT PGresult* 
    zpgutil_session_select (zpgutil_session_t *self);

// -----------------------------------------------------------------------------
// Execute but without commiting (opening a transaction) 
ZPGUTIL_EXPORT int 
    zpgutil_session_execute (zpgutil_session_t *self);

//Execute a prepared statement identified by its name as used at the creation
ZPGUTIL_EXPORT int
    zpgutil_session_execute_prepared (zpgutil_session_t *self, const char *stmtName);

// -----------------------------------------------------------------------------
// Commit whatever is prepared in the current Postgres connection
ZPGUTIL_EXPORT int 
    zpgutil_session_commit (zpgutil_session_t *self);

// -----------------------------------------------------------------------------
// Rollback whatever is prepared in the current Postgres connection
ZPGUTIL_EXPORT int 
    zpgutil_session_rollback (zpgutil_session_t *self);


//  Print properties of object
ZPGUTIL_EXPORT void
    zpgutil_session_print (zpgutil_session_t *self);

//  Self test of this class
ZPGUTIL_EXPORT int
    zpgutil_session_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
