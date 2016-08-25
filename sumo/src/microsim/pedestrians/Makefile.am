noinst_LIBRARIES = libmicrosimpeds.a

libmicrosimpeds_a_SOURCES = MSPerson.cpp MSPerson.h \
MSPModel.cpp MSPModel.h \
MSPModel_Striping.cpp MSPModel_Striping.h \
MSPModel_NonInteracting.cpp MSPModel_NonInteracting.h \
MSGRPCClient.cpp MSGRPCClient.h \
MSPRCPState.cpp MSPRCPState.h \
hybridsim.grpc.pb.cc hybridsim.grpc.pb.h \
hybridsim.pb.cc hybridsim.pb.h

all: protoc
hybridsim.pb.h: protoc
hybridsim.pb.cc: protoc
hybridsim.grpc.pb.h: protoc
hybridsim.grpc.pb.cc: protoc
protoc: hybridsim.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` hybridsim.proto
	protoc --cpp_out=. hybridsim.proto

