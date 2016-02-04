#!/usr/bin/env bash

if [ "$1" == "" ]; then
	echo "usage: ./build.sh path-to-proto-file"
	exit -1
fi

echo "works with Mac and 64bit Linux only"
echo "for other OSs you need to get, build, and install grpc-java by yourself"
case "$OSTYPE" in
  darwin*)  echo "OSX detected"; PLUGIN_JAVA="protoc-gen-grpc-java-osx"; PLUGIN_CPP="protoc-gen-grpc-cpp-osx";;
  linux*)   echo "LINUX detected" ; PLUGIN_JAVA="protoc-gen-grpc-java-linux-x86-64"; PLUGIN_CPP="protoc-gen-grpc-cpp-linux-x86-64";;
   *)        echo "unsupported: $OSTYPE" && exit -2;;
esac

PROTO=$1
echo "Proto file: $PROTO"

protoc -I ../src/main/proto --grpc_out=../src/main/java/ --plugin=protoc-gen-grpc=$PLUGIN_JAVA $PROTO
protoc -I ../src/main/proto --java_out=../src/main/java/ $PROTO
protoc -I ../src/main/proto --grpc_out=../../src/microsim/pedestrians  --plugin=protoc-gen-grpc=$PLUGIN_CPP $PROTO
protoc -I ../src/main/proto --cpp_out=../../src/microsim/pedestrians $PROTO

protoc -I ../src/main/proto --grpc_out=../../grpc-c++/src/  --plugin=protoc-gen-grpc=$PLUGIN_CPP ../src/main/proto/noninteracting.proto
protoc -I ../src/main/proto --cpp_out=../../grpc-c++/src/ $PROTO
echo "done"
