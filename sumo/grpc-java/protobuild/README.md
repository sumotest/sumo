#The following is based on https://github.com/matsim-org/matsim/tree/master/playgrounds/gregor/protobuild and adapted and updated to the current needs


# Setup of protobuf (tested on Mac and Linux)

One needs the correct version of the `protoc` compiler for `build.sh` to work.  Currently, this seems to be the version 
v3.0.0-beta-2.  I had success with the following sequence of steps:

* `git clone https://github.com/google/protobuf.git`

* `git checkout d5fb408ddc281ffcadeb08699e65bb694656d0bd`

* compile protobuf according to instructions on [github](https://github.com/google/protobuf) .

# Setup of grpc

grpc is the piece that supports remote procedure calls.  

The MATSim guys had success with the following:

* go to [search.maven.org](http://search.maven.org) and search for `grpc-java`.

* Download the corresponding `*.exe` for your platform (it is also `*.exe` for Mac) and put it in the directory where 
`build.sh` resides.

* Replace `protoc-gen-grpc-java` in `build.sh` by the name of the file you just downloaded.

The grpc documentation implies that this could also be included via maven.  This, however, needs maven3, and matsim 
currently does not have that, so it would have to be done somehow "on the side".  The above path seems easier for the
time being ... in particular since one does not need these files to *run* the code, just to compile it.  



# Build sequence (on Mac)

* edit the *.proto files in `src.main.resources/proto/...` according to needs

* run `sh build.sh` in `protobuild` directory.  This will generate the necessary java classes.  The class name is the
same as the name of the `*.proto` file.  The package is given inside the `*.proto` file.

* One can now program against those java classes.  For example, `VisServer.initScenario()` is copying the network part of the
matsim scenario into the network part of the proto scenario.  That scenario is public, thus it can be used from the outside; 
it seems to be used in `BlockingVisServiceImpl`.



