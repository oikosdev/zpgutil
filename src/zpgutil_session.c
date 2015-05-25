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
    free (cs);

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
        zsys_info ("Opened connection to Postgres.\n");
    }
    assert (self->conn);
    self->pars = zlist_new ();
    zlist_autofree (self->pars);

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
    zsys_debug ("sql query is now: %s \n",self->sql);
}

void
zpgutil_session_set (zpgutil_session_t *self, char *par)
{
    char* str = (char *)zmalloc(300);
    strcpy(str,par);
    zlist_append (self->pars,str);
    zsys_debug ("set the parameter to:%s\n",str);
    free (str);
}

int
zpgutil_session_execute (zpgutil_session_t *self) 
{
    assert(self->conn);
    PGresult *res = PQexec (self->conn, "BEGIN");
    if (PQresultStatus (res)!=PGRES_COMMAND_OK)
    {
    zsys_error ("Failing to open the transaction: %s\n", PQerrorMessage(self->conn));
    }
    else
    {    
    zsys_info ("Beginning Transaction");
    }
    PQclear (res);
    assert(self->sql);
    if(strchr(self->sql,'$')!=NULL)
    {
      zsys_debug ("$ found\n");
      int size = zlist_size(self->pars);
      zsys_debug ("number of parameters = %i\n",size);
      // init a table of parameters
      char **paramValues = (char **)zmalloc(size * sizeof(char *));
      int stringsize = 300; 
      for (int j = 0; j < size; ++j) {
         paramValues[j] = (char *)zmalloc(stringsize+1);
      }
      //----------------------
      zlist_first (self->pars);
      int i=0;
      while(i<size)
      {
        zsys_debug ("param? %s\n",(char*)(zlist_item(self->pars)));
        paramValues[i] = (char*)(zlist_item(self->pars));
        zsys_debug ("set param for %i value=%s\n",i,paramValues[i]);
        i++;
        zlist_next(self->pars);
      }
      res = PQexecParams(self->conn,
                    self->sql,
                    size,
                    NULL, // datatypes inferred
                    (const char * const *)paramValues,
                    NULL, // not needed (for binary)
                    NULL, // not needed (for binary)
                    0     // returns in text format
                   ); 
      assert(res); 
      // release the memory hold by paramValues
      for (int k = 0; k < size; ++k) {
        free(paramValues[k]);
      }
      free(paramValues);
   } 
    else
    { 
        res = PQexec(self->conn,self->sql);
    }
    if(PQresultStatus(res)!=PGRES_COMMAND_OK)
    {
       zsys_error ("EXECUTE failed: %s\n", PQerrorMessage(self->conn));
       PQclear (res);
       return 1;
    }
    else
    {
      zsys_info ("EXECUTE succeeded");
      PQclear (res);
      assert (res);
    }
    return 0;
}

PGresult*
zpgutil_session_select (zpgutil_session_t *self)
{
    assert(self->conn);
    PGresult *res = NULL;
    assert(self->sql);
    if(strchr(self->sql,'$')!=NULL)
    {
      zsys_debug ("$ found\n");
      int size = zlist_size(self->pars);
      const char *paramValues[size];
      for(int i=0;i<size;i++)
      {
       char *par = (char *)zlist_next(self->pars);
       zsys_debug ("set parameter value=%s\n",par);
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
       zsys_error ("SELECT failed: %s\n", PQerrorMessage(self->conn));
    }
    else
    {
      zsys_info ("SELECT succeeded");
      assert (res);
    }
    return res;
}


// Select query with a char* return type
// char* to be freed by client
char*
zpgutil_session_select_one (zpgutil_session_t *self)
{
    PGresult *res = zpgutil_session_select (self);
    if(PQntuples(res)==0) 
    {
    zsys_debug ("No tuple returned !");
    return NULL;
    }
    else 
    {
    char* resStr = (char *)zmalloc(300);
    strcpy(resStr,PQgetvalue(res,0,0));
    PQclear(res);
    return resStr;
    }
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
          zsys_info ("Finished connection to Postgres.\n");
        }
        zpgutil_session_t *self = *self_p;

        //  Free class properties
        zlist_destroy (&self->pars);
        free (self->sql); 
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

int 
zpgutil_session_transac_command (zpgutil_session_t *self, char* command) 
{
    assert (self);
    PGresult *res = PQexec (self->conn, command);
    if(PQresultStatus(res)!=PGRES_COMMAND_OK)
    {
    zsys_error ("Command %s failed: %s", command, PQerrorMessage(self->conn));
    // \todo use constant for error codes
    PQclear (res);
    return 1;
    }
    else
    {
    zsys_info ("%s succeeded",command);
    PQclear (res);
    }
    return 0;
}


// -----------------------------------------------------------------------------
// Commit whatever is prepared in the current Postgres connection
int 
zpgutil_session_commit (zpgutil_session_t *self) 
{
    return zpgutil_session_transac_command (self,"COMMIT");
}

int 
zpgutil_session_rollback (zpgutil_session_t *self) 
{
    return zpgutil_session_transac_command (self,"ROLLBACK");
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
    char* res0 = zpgutil_session_select_one (self);
    free (res0);
    //----------------------------------------------------------
    zpgutil_session_sql (self,"SELECT name FROM company WHERE code=$1");
    zpgutil_session_set (self,"XXX");
    char* res = zpgutil_session_select_one (self);
    assert (res);
    printf ("resultat=%s\n", res);
    assert (streq(res,"Lambda Inc."));
    free (res);
    //------------------------------------------------------------ 
    zpgutil_session_sql (self, "SELECT code, name FROM company");
    PGresult* r = zpgutil_session_select (self); 
    assert (r);
    PQclear (r);
    //-------------------------------------------------------------
    int no_transac = zpgutil_session_commit (self);
    // does nothing but suceeds anyway
    assert (!no_transac);
    //------------------------------------------------------------ 
    zpgutil_session_sql (self, "INSERT INTO ACCOUNT(name) VALUES('FOO')");
    int e = zpgutil_session_execute (self);
    assert (!e);
    int transac = zpgutil_session_commit (self);
    assert (!transac);
    zpgutil_session_sql (self, "DELETE FROM ACCOUNT WHERE NAME='FOO'");
    zpgutil_session_execute (self);
    zpgutil_session_commit (self);
    zpgutil_session_sql (self, "INSERT INTO ACCOUNT(name) VALUES('FOO')");
    int e2 = zpgutil_session_execute (self);
    assert (!e2);
    zpgutil_session_rollback (self); 
    zpgutil_session_destroy (&self);
    zpgutil_datasource_destroy (&datasource);
    zconfig_destroy (&config);
    //  @end
    printf ("OK\n");
    return 0;
}
