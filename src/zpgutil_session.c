/*  =========================================================================
    zpgutil_session - Session with the acual connection to the Postgres database

    Copyright (c) the Contributors as noted in the AUTHORS file.                                                             
                                                                                                                             
    This Source Code Form is subject to the terms of the GNU General Public License as published by the Free Software Foundat
    ion; either version 3 of the License, or (at your option) any later version.                                             
    See the GNU General Public License for more details                                                                      
    =========================================================================
*/

/*
@header
    zpgutil_session - Session with the acual connection to the Postgres database
@discuss
@end
*/

//  Structure of our class

#include "../include/zpgutil.h"

struct _zpgutil_session_t {
  PGconn *conn;
  char *sql; 
  zlist_t *pars;
};


//  --------------------------------------------------------------------------
//  Create a new zpgutil_session

zpgutil_session_t *
zpgutil_session_new (zpgutil_datasource_t *datasource)
{
    zpgutil_session_t *self = (zpgutil_session_t *) zmalloc (sizeof (zpgutil_session_t));
    assert (self);
    char *cs = zpgutil_datasource_connStr(datasource);
    self->conn = PQconnectdb(cs);
    //-------
    //TODO
    self->sql = (char *)zmalloc(300); 
    /* Check to see that the backend connection was successfully made */
    if (PQstatus(self->conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(self->conn));
        // TODO exit_nicely(self->conn);
    } 
    else
    {
        printf("Opened connection to Postgres.\n");
    }
    assert (self->conn);
    free (cs);
    self->pars = zlist_new ();
    assert (self->pars);
    return self;
}

// Set the current SQL query
// Returns self
void
zpgutil_session_sql (zpgutil_session_t *self, char *sql)
{
    // necessary to reset the params cache
    zlist_purge (self->pars);
    strcpy(self->sql,sql);
    printf("sql query is now: %s \n",self->sql);
}

void
zpgutil_session_set (zpgutil_session_t *self, char *par)
{
    zlist_append (self->pars,par);
    printf("set the parameter to:%s\n",par);
}


PGresult*
zpgutil_session_select (zpgutil_session_t *self)
{
    assert(self->conn);
    PGresult *res = NULL;
    assert(self->sql);
    if(strchr(self->sql,'$')!=NULL)
    {
      printf("? found\n");
      int size = zlist_size(self->pars);
      printf("size=%i\n",size);
      const char *paramValues[size];
      for(int i=0;i<size;i++)
      {
       char *par = (char *)zlist_next(self->pars);
       printf("value=%s\n",par);
       paramValues[i]=par;
      }
      res = PQexecParams(self->conn,
                    self->sql,
                    size,
                    NULL, // datatypes inferred
                    paramValues,
                    NULL, // not needed (for binary)
                    NULL, // not needed (for binary)
                    0     // returns in text format
                   ); 
      assert(res); 
    }
    else
    { 
        res = PQexec(self->conn,self->sql);
    }
    if(PQresultStatus(res)!=PGRES_TUPLES_OK)
    {
       printf("SELECT failed: %s\n", PQerrorMessage(self->conn));
    }
    else
    {
      printf("SELECT succeeded !\n");
      assert(res);
      printf ("result returned\n");
    }
    return res;
}


// Select query with a char* return type
// char* to be freed by client
char*
zpgutil_session_select_one (zpgutil_session_t *self)
{
    PGresult *res = zpgutil_session_select (self);
    char* resStr = (char *)zmalloc(300);
    strcpy(resStr,PQgetvalue(res,0,0));
    printf("%s\n",resStr);
    PQclear(res);
    return resStr;
}

//  --------------------------------------------------------------------------
//  Destroy the zpgutil_session

void
zpgutil_session_destroy (zpgutil_session_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zpgutil_session_t *session = *self_p;
        if(session->conn) {
          PQfinish(session->conn);
          printf("Finished connection to Postgres.\n");
        }
        zpgutil_session_t *self = *self_p;

        //  Free class properties
        zlist_destroy (&self->pars);
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Print properties of object

void
zpgutil_session_print (zpgutil_session_t *self)
{
    assert (self);
}


//  --------------------------------------------------------------------------
//  Selftest

int
zpgutil_session_test (bool verbose)
{
    printf (" * zpgutil_session: ");

    //  @selftest
    zconfig_t* config = zconfig_load(".testdir/test.cfg");
    assert(config); 
    char *user = zconfig_resolve (config, "/database/user", NULL);
    assert(user);
    printf("user=%s\n",user);
    zpgutil_datasource_t *datasource = zpgutil_datasource_new (config);
    assert (datasource);
    //---------------------------------------------------------- 
    zpgutil_session_t *self = zpgutil_session_new (datasource);
    assert (self);
    zpgutil_session_sql (self,"SELECT * FROM company");
    zpgutil_session_select_one (self);
    //----------------------------------------------------------
    zpgutil_session_sql (self,"SELECT name FROM company WHERE code=$1");
    zpgutil_session_set (self,"XXX");
    char* res = zpgutil_session_select_one (self);
    assert (res);
    printf ("resultat=%s\n", res);
    assert (streq(res,"Lambda Inc."));
    //------------------------------------------------------------ 
    zpgutil_session_sql (self, "SELECT code, name FROM company");
    PGresult* r = zpgutil_session_select (self); 
    assert (r);
    PQclear (r);
    zpgutil_session_destroy (&self);
    //  @end

    printf ("OK\n");
    return 0;
}