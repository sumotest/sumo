export CPPFLAGS="$CPPFLAGS -I/opt/X11/include/"
export LDFLAGS="-L/opt/X11/lib"

./configure CXX=clang++ CXXFLAGS="-stdlib=libc++ -std=gnu++11" --with-xerces=/usr/local --with-proj-gdal=/usr/local
