zpgutil
=======

Utility library to easily use PostgreSQL libpq from a czmq program, in a (pure C) object-oriented way.

Status 
------

[![Build Status](https://travis-ci.org/oikosdev/zpgutil.svg?branch=master)](https://travis-ci.org/oikosdev/zpgutil)

Memory leaks checked, code secured for buffer-overflow. Contributions all welcome !

Overview 
--------

 * Pure C
 * Developed following  [the CLASS language] (http://rfc.zeromq.org/spec:21)
 * A very thin and simple layer on top of libpq.

Content 
-------

The first class provided by zpgutil is the datasource: zpgutil_datasource.

A datasource is a simple data structure holding all the basics parameters to access a PostgreSQL. It is constructed from a [zconfig_t](http://czmq.zeromq.org/manual:zconfig): this provides a standard config file format from which you can set-up your program.

In your datasource, you will typically set up: db, user, password, host and port. 

The second class provided by this library is the session: zpgutil_session.

A session is a class holding a connection to the postgresql database. A session is constructed from a zpgutil_datasource.
Then, you can use it to set a sql query, set parameters, query for data, execute sql queries, commit, rollback.

Let's see some code example:

    zpgutil_datasource_t *datasource = zpgutil_datasource_new (config);
    assert (datasource);
    zpgutil_session_t *self = zpgutil_session_new (datasource);
    assert (self);
    zpgutil_session_sql (self,"SELECT * FROM company");
    zpgutil_session_select_one (self);
    //------------------------------------------------------------ 
    zpgutil_session_sql (self, "SELECT code, name FROM company");
    PGresult* r = zpgutil_session_select (self); 
    assert (r);
    PQclear (r);
    //-------------------------------------------------------------
    zpgutil_session_sql (self, "INSERT INTO ACCOUNT(name) VALUES('FOO')");
    int e = zpgutil_session_execute (self);
    assert (!e);
    int transac = zpgutil_session_commit (self);
    assert (!transac);

Dependencies   
------------

 * libzmq, version > 4.2.0
 * czmq, version > 3.0
 * PostgreSQL (libpq)

Building 
--------

Before building, make sure all the dependencies are known to pkg-config. You may need to adjust your PKG_CONFIG_PATH.
You may also need to adjust the following environment variables: zmq_CFLAGS, zmq_LIBS.

    ./autogen.sh
    ./configure
    make 

Tests    
-----

Only tested on Linux Ubuntu 12, gcc 4.6.3.
 
