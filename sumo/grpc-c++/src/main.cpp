#include <iostream>

#include "DummyJPS.h"


int main(int argc, char** argv) {

    std::string server_address("0.0.0.0:50051");


    DummyJPS  service;

    ServerBuilder builder;

    builder.AddListeningPort(server_address,grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());

    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();


    return 0;
}
