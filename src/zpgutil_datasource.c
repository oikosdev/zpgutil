/*  =========================================================================
    zpgutil_datasource - Datasource: embedding the connection parameters to the database

    Copyright (c) the Contributors as noted in the AUTHORS file.                                                             
                                                                                                                             
    This Source Code Form is subject to the terms of the GNU General Public License as published by the Free Software Foundat
    ion; either version 3 of the License, or (at your option) any later version.                                             
    See the GNU General Public License for more details                                                                      
    =========================================================================
*/

/*
@header
    zpgutil_datasource - Datasource: embedding the connection parameters to the database
@discuss
@end
*/

//  Structure of our class

struct _zpgutil_datasource_t {
  char* user;
  char* password;
  char* host;
  char* port;
  char* db;
};

#include "../include/zpgutil.h"

//  --------------------------------------------------------------------------
//  Create a new zpgutil_datasource

zpgutil_datasource_t *
zpgutil_datasource_new (zconfig_t *config)
{
    zpgutil_datasource_t *self = (zpgutil_datasource_t *) zmalloc (sizeof (zpgutil_datasource_t));
    self->user = (char*)zmalloc(100);
    self->password = (char*)zmalloc(100);
    self->host= (char*)zmalloc(100);
    self->port = (char*)zmalloc(100);
    self->db = (char*)zmalloc(100);
    assert (self);
    char* db =  zconfig_get (config, "/database/db", NULL);
    assert( strlen(db)<100 );
    strcpy(self->db, db);
    char* user =  zconfig_get (config, "/database/user", NULL);
    assert(strlen(user)<100);
    strcpy(self->user,user);
    char* password =  zconfig_get (config, "/database/password", NULL);
    assert(strlen(password)<100);
    strcpy(self->password,password);
    char* host =  zconfig_get (config, "/database/host", NULL);
    assert(strlen(host)<100);
    strcpy(self->host,host);
    char* port = zconfig_get (config, "/database/port", NULL);
    assert(strlen(port)<100);
    strcpy(self->port, port);
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zpgutil_datasource

void
zpgutil_datasource_destroy (zpgutil_datasource_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zpgutil_datasource_t *self = *self_p;

        //  Free class properties
        free (self->db);
        free (self->password);
        free (self->host);
        free (self->port);
        free (self->user);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

char* 
zpgutil_datasource_connStr (zpgutil_datasource_t *self)
{
    char* connStr = (char*) zmalloc(200);
    sprintf(connStr,"dbname=%s user=%s password=%s hostaddr=%s port=%s",
                     self->db,
                     self->user,
                     self->password,
                     self->host,
                     self->port);
    zsys_debug ("connection string: %s\n",connStr);
    return connStr;
} 

//  --------------------------------------------------------------------------
//  Print properties of object

void
zpgutil_datasource_print (zpgutil_datasource_t *self)
{
    assert (self);
}


//  --------------------------------------------------------------------------
//  Selftest

int
zpgutil_datasource_test (bool verbose)
{
    printf (" * zpgutil_datasource: ");

    //  @selftest
    //  Simple create/destroy test
    zconfig_t* config = zconfig_load(".testdir/test.cfg");
    assert(config); 
    char *user = zconfig_get (config, "/database/user", NULL);
    assert(user);
    printf("user=%s\n",user);
    zpgutil_datasource_t *self = zpgutil_datasource_new (config);
    assert (self);
    printf("port in datasource =%s\n",self->port);
    //  @end
    char * str = zpgutil_datasource_connStr (self);
    assert (str);
    free (str);
    printf ("OK\n");
    zpgutil_datasource_destroy (&self);
    zconfig_destroy (&config);
    return 0;
}
