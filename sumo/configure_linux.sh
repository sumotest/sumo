export CPPFLAGS="$CPPFLAGS -I$PWD/../include/"
export LDFLAGS="-L$PWD/../lib64 -L$PWD/../lib"
export PATH="$PWD/../bin:$PATH"
./configure && make -j16
