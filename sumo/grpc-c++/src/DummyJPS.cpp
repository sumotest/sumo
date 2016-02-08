//
// Created by laemmel on 08.02.16.
//

#include "DummyJPS.h"



#include <iostream>



//int main(int argc, char** argv) {
//    std::cout << "Hello, World!" << std::endl;
//    std::string server_address("0.0.0.0:50051");
//    DummyJPS service;
//
//
//    ServerBuilder builder;
//
//    builder.AddListeningPort(server_address,grpc::InsecureServerCredentials());
//    builder.RegisterService(&service);
//    std::unique_ptr<Server> server(builder.BuildAndStart());
//
//    std::cout << "Server listening on " << server_address << std::endl;
//    server->Wait();
//
////    builder.BuildAndStart();
//
////    std::cout << grpc_test_VERSION_MAJOR << " " << grpc_test_VERSION_MINOR << std::endl;
//    return 0;
//}



::grpc::Status DummyJPS::simulatedTimeInerval(::grpc::ServerContext *context,
                                              const ::hybridsim::LeftClosedRightOpenTimeInterval *request,
                                              ::hybridsim::Empty *response) {
    std::cout << "simulateing from: " << request->fromtimeincluding() << " to: " << request->totimeexcluding() << "\n";
    std::map<const std::string,DummyAgent*>::iterator it = agents.begin();

    while(it != agents.end()) {
        (*it).second->proceed();
        it++;
    }


    return Status::OK;
}

::grpc::Status DummyJPS::transferAgent(::grpc::ServerContext *context, const ::hybridsim::Agent *request,
                                       ::hybridsim::Boolean *response) {

    DummyAgent * a = new DummyAgent(request->x(),request->y());

    agents[request->id()] = a;

    response->set_val(true);


    return Status::OK;
}

::grpc::Status DummyJPS::receiveTrajectories(::grpc::ServerContext *context, const ::hybridsim::Empty *request,
                                             ::hybridsim::Trajectories *response) {

    std::map<const std::string,DummyAgent*>::iterator it = agents.begin();

    while(it != agents.end()) {
      // (*it).second->proceed();
        Trajectory * tr = response->add_trajectories();
        tr->set_x((*it).second->getX());
        tr->set_y((*it).second->getY());


        it++;
    }


    return Status::OK;
}

::grpc::Status DummyJPS::retrieveAgents(::grpc::ServerContext *context, const ::hybridsim::Empty *request,
                                        ::hybridsim::Agents *response) {


    return Status::OK;
}

