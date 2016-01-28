#!/usr/bin/env bash
echo "works with Mac and 64bit Linux only"
echo "for other OSs you need to get, build, and install grpc-java by yourself"
case "$OSTYPE" in
  darwin*)  echo "OSX detected"; PLUGIN_JAVA="protoc-gen-grpc-java-osx" ;;
  linux*)   echo "LINUX detected" ; PLUGIN_JAVA="protoc-gen-grpc-java-linux-x86-64"; PLUGIN_CPP="protoc-gen-grpc-cpp-linux-x86-64";;
   *)        echo "unsupported: $OSTYPE" && exit -2;;
esac
protoc -I ../src/main/proto --grpc_out=../src/main/java/ --plugin=protoc-gen-grpc=$PLUGIN_JAVA ../src/main/proto/noninteracting.proto
protoc -I ../src/main/proto --java_out=../src/main/java/ ../src/main/proto/noninteracting.proto 
protoc -I ../src/main/proto --grpc_out=../../src/microsim/pedestrians  --plugin=protoc-gen-grpc=$PLUGIN_CPP ../src/main/proto/noninteracting.proto
protoc -I ../src/main/proto --cpp_out=../../src/microsim/pedestrians ../src/main/proto/noninteracting.proto

protoc -I ../src/main/proto --grpc_out=../../grpc-c++/src/  --plugin=protoc-gen-grpc=$PLUGIN_CPP ../src/main/proto/noninteracting.proto
protoc -I ../src/main/proto --cpp_out=../../grpc-c++/src/ ../src/main/proto/noninteracting.proto
echo "done"
