#The following is based on https://github.com/matsim-org/matsim/tree/master/playgrounds/gregor/protobuild and adapted and updated to the current needs


# Setup of protobuf (tested on Mac and Linux)

One needs the correct version of the `protoc` compiler for `build.sh` to work.  Currently, this seems to be the version 
v3.0.0-beta-2.  I had success with the following sequence of steps:

    git clone https://github.com/google/protobuf.git
    cd protobuf
    git checkout d5fb408
    ./autogen.sh
    ./configure
    make -j4
    make -j4 check
    make install
    ldconfig #for linux only, for Mac lib paths should be updated automatically 

# Setup of grpc

    git clone https://github.com/grpc/grpc.git
    cd grpc
    git checkout 78e04bb
    git submodule update --init
    make -j4
    make install
    ldconfig #for linux only, for Mac lib paths should be updated automatically 


