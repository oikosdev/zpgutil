#!/usr/bin/env bash

echo "STARTING BUILD"
if [ $BUILD_TYPE == "default" ]; then
    # Build, check, and install libsodium if WITH_LIBSODIUM is set
    if [ -n "$WITH_LIBSODIUM" ]; then
        git clone git://github.com/jedisct1/libsodium.git &&
        ( cd libsodium; ./autogen.sh && ./configure &&
            make check && sudo make install && sudo ldconfig ) || exit 1
    fi

    # Build, check, and install the version of ZeroMQ given by ZMQ_REPO
    git clone git://github.com/zeromq/${ZMQ_REPO}.git &&
    ( cd ${ZMQ_REPO}; ./autogen.sh && ./configure &&
        make check && sudo make install && sudo ldconfig ) || exit 1

    # Build, check, and install czmq 
    git clone git://github.com/zeromq/czmq.git &&
    ( cd czmq; ./autogen.sh && ./configure &&
        make check && sudo make install && sudo ldconfig ) || exit 1

    # Build, check, and install zpgutil from local source
    ./configure && make check-verbose VERBOSE=1 && sudo make install
else
    cd ./builds/${BUILD_TYPE} && ./ci_build.sh
fi

echo "END OF BUILD"