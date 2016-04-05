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

DummyJPS::DummyJPS() { }


DummyJPS::~DummyJPS() { }

::grpc::Status DummyJPS::simulatedTimeInerval(::grpc::ServerContext *context,
                                              const ::hybridsim::LeftClosedRightOpenTimeInterval *request,
                                              ::hybridsim::Empty *response) {
    std::cout << "simulateing from: " << request->fromtimeincluding() << " to: " << request->totimeexcluding() << "\n";
    std::map<const std::string,DummyAgent*>::iterator it = agents.begin();
    std::cout << "number of agents: " << agents.size() << std::endl;
    while(it != agents.end()) {
        (*it).second->proceed();
        it++;
    }
    std::cout << "sim step done." << std::endl;

    return Status::OK;
}

::grpc::Status DummyJPS::transferAgent(::grpc::ServerContext *context, const ::hybridsim::Agent *request,
                                       ::hybridsim::Boolean *response) {



    std::cout << "transfer" << std::endl;
    std::vector<Link> links;
    auto it = request->leg().link().begin();
    while (it != request->leg().link().end()) {
        const hybridsim::Link &l = *it;
        Link * al = new Link();
        al->x0 = l.x0();
        al->y0 = l.y0();
        al->x1 = l.x1();
        al->y1 = l.y1();
        al->id = l.id();
        links.push_back(*al);
        it++;

    }
    DummyAgent * a = new DummyAgent(request->x(),request->y(),request->id(),links);

    agents[request->id()] = a;

    response->set_val(true);

    std::cout << "transfer done." << std::endl;

    return Status::OK;
}

::grpc::Status DummyJPS::receiveTrajectories(::grpc::ServerContext *context, const ::hybridsim::Empty *request,
                                             ::hybridsim::Trajectories *response) {


    std::cout << "trajectories" << std::endl;
    std::map<const std::string,DummyAgent*>::iterator it = agents.begin();
    std::cout << "map" << std::endl;
    while(it != agents.end()) {
        // (*it).second->proceed();
        DummyAgent * a = (*it).second;

        std::cout << "agent:" << a->getID() << std::endl;


        Trajectory * tr = response->add_trajectories();

        std::cout << "tr created" << std::endl;
        tr->set_id(a->getID());
        tr->set_x(a->getX());
        tr->set_y(a->getY());
        tr->set_linkid(a->getCurrentLinkID());
        it++;
    }

    std::cout << "trajectories done." << std::endl;

    return Status::OK;
}

::grpc::Status DummyJPS::retrieveAgents(::grpc::ServerContext *context, const ::hybridsim::Empty *request,
                                        ::hybridsim::Agents *response) {


    std::cout << "retrieve" << std::endl;
    std::map<const std::string,DummyAgent*>::iterator it = agents.begin();
    while(it != agents.end()) {
        if ((*it).second->getX() > 30.) {
            Agent * agent  = response->add_agents();
            agent->set_id((*it).second->getID());
            it = agents.erase(it);
            std::cout << "erasing agent: " << agent->id() << "\n";
        } else {
            it++;
        }

    }

    std::cout << "retrieve done." << std::endl;
    return Status::OK;
}

void DummyJPS::DummyAgent::updateDeltas() {

        current = &*lIt;
        double xL = current->x1-current->x0;
        double yL = current->x1-current->x0;
        sqLength = xL*xL + yL * yL;

        double length = sqrt(sqLength);

        dx = xL/length;
        dy = yL/length;

        cX = 0;
        cY = 0;

}

void DummyJPS::DummyAgent::proceed() {
    x += dx * spd;
    y += dy * spd;
    cX += dx * spd;
    cY += dy * spd;

    if ((cX*cX+cY*cY) > sqLength) {
        if (lIt == links.end()-1) {
            current = 0;

        } else {
            lIt++;
            updateDeltas();
        }
    }

}
