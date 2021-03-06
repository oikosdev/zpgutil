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

const int MAX_QUERY_SIZE=1000000; 
const int MAX_PARAM_SIZE=1000000;

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
    // note: sql is not set yet (waiting for the query)
    assert (self->pars);
    return self;
}

// Set the current SQL query
// Returns self
void
zpgutil_session_sql (zpgutil_session_t *self, const char *sql)
{
    // purge if a sql query is already set
    if(self->sql) 
    {
      free(self->sql);
      self->sql = NULL;
    }
    // necessary to reset the params cache
    zlist_purge (self->pars);
    assert(strlen(sql)<MAX_QUERY_SIZE);
    self->sql = (char *)zmalloc((strlen(sql)+1));
    strcpy(self->sql,sql);
    zsys_debug ("sql query is now: %s \n",self->sql);
}

void
zpgutil_session_set (zpgutil_session_t *self, const char *par)
{
    assert(strlen(par)<MAX_PARAM_SIZE);
    char* str = (char *)zmalloc((strlen(par)+1));
    strcpy(str,par);
    zlist_append (self->pars,str);
    zsys_debug ("set the parameter to:%s\n",str);
    free (str);
}

int 
zpgutil_session_prepare (zpgutil_session_t *self, const char *stmtName, const char *sql)
{
    assert(self->conn);
    zsys_info ("Beginning Transaction");
    assert(sql);
    assert(stmtName);
    PGresult *res = PQprepare(self->conn,
		    stmtName,
                    sql,
                    0,
                    NULL
                   ); 
    assert(res); 
    if(PQresultStatus(res)!=PGRES_COMMAND_OK)
    {
       zsys_error ("PREPARED failed: %s\n", PQerrorMessage(self->conn));
       PQclear (res);
       return 1;
    }
    else
    {
      zsys_info ("PREPARED succeeded");
      PQclear (res);
      assert (res);
    }
    return 0;
}

/*
 * Util function (not in API) to get an arrray of strings from a zlist
 */ 
const char* *zlist_to_array (zlist_t *list)
{
 int size = zlist_size (list);
 const char* *stringArray = (const char**)zmalloc (size);
 zlist_first (list);
 for (int i=0;i<size;++i) 
 {
    stringArray[i] = (const char*)(zlist_item(list));
    zlist_next (list);
    zsys_debug ("in Array set param for %i value=%s\n",i,stringArray[i]);
 }
 return stringArray;
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
      int size = zlist_size(self->pars);
      zsys_debug ("number of parameters = %i\n",size);
      // init a table of parameters
      const char **paramValues = zlist_to_array (self->pars);
      //----------------------
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
      free(paramValues);
    if(PQresultStatus(res)!=PGRES_COMMAND_OK)
    {
       zsys_error ("EXECUTE failed: %s\n", PQerrorMessage(self->conn));
       PQclear (res);
       zlist_purge (self->pars);
       return 1;
    }
    else
    {
      zsys_info ("EXECUTE succeeded");
      PQclear (res);
      zlist_purge (self->pars);
      assert (res);
    }
    return 0;
}

int
zpgutil_session_execute_prepared (zpgutil_session_t *self, const char *stmtName) 
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
    assert (stmtName);
    int size = zlist_size (self->pars);
    const char **paramValues = zlist_to_array (self->pars);
    res = PQexecPrepared(self->conn,
                    stmtName,
                    size,
                    (const char * const *)paramValues,
                    NULL, // not needed (for binary)
                    NULL, // not needed (for binary)
                    0     // returns in text format
                   ); 
   assert(res); 
   free(paramValues);
   if(PQresultStatus(res)!=PGRES_COMMAND_OK)
   {
      zsys_error ("EXECUTE failed: %s\n", PQerrorMessage(self->conn));
      zlist_purge (self->pars);
      PQclear (res);
      return 1;
   }
   else
   {
     zsys_info ("EXECUTE succeeded");
     zlist_purge (self->pars);
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
        if(self->sql)
        {
        free (self->sql); 
        }
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
    char *user = zconfig_get (config, "/database/user", NULL);
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
    zpgutil_session_sql (self, "INSERT INTO ACCOUNT(name) VALUES($1)");
    zpgutil_session_set (self, "My'FOO");
    int e = zpgutil_session_execute (self);
    assert (!e);
    zpgutil_session_sql (self, "SELECT name FROM ACCOUNT");
    char * res2 = zpgutil_session_select_one (self);
    assert (streq(res2,"My'FOO"));
    int transac = zpgutil_session_commit (self);
    assert (!transac);
    // testing here without parameters
    zpgutil_session_sql (self, "DELETE FROM ACCOUNT WHERE NAME LIKE 'My%'");
    zpgutil_session_execute (self);
    zpgutil_session_commit (self);
    // ------------------
    zpgutil_session_sql (self, "INSERT INTO ACCOUNT(name) VALUES($1)");
    zpgutil_session_set (self, "My'FOO");
    int e2 = zpgutil_session_execute (self);
    assert (e2==0);
    zpgutil_session_rollback (self); 
    // Prepqred statements
    int pr = zpgutil_session_prepare (self, "my_stmt", "INSERT INTO ACCOUNT(name) VALUES($1)");
    assert (pr==0);
    zpgutil_session_set (self, "Peter");
    int ep = zpgutil_session_execute_prepared (self, "my_stmt");
    assert (ep==0);
    zpgutil_session_sql (self, "SELECT name FROM ACCOUNT WHERE name LIKE 'Pet%'");
    char * res3 = zpgutil_session_select_one (self);
    assert (streq(res3,"Peter"));
 
    zpgutil_session_destroy (&self);
    zpgutil_datasource_destroy (&datasource);
    zconfig_destroy (&config);
    //  @end
    printf ("OK\n");
    return 0;
}
