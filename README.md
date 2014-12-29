zpgutil
=======

Utility library to easily use PostgreSQL from a czmq program

Status 
------

This is still a draft work. Not ready to be used yet, but contributors are welcome.

Overview 
--------

 * Pure C
 * Developed following  [the CLASS language] (http://rfc.zeromq.org/spec:21)
 * A very thin and simple layer on top of libpq.

Content 
-------

The first class provided by zpgutil is the datasource: zpgutil_datasource_t.

A datasource is a simple data structure holding all the basics parameters to access a PostgreSQL. It is constructed from a [zconfig_t](http://czmq.zeromq.org/manual:zconfig): this provides a standard config file format from which you can set-up your program.

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

Only tested with Linux Ubuntu 12, gcc 4.6.3.
 
