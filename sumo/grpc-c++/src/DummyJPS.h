//
// Created by laemmel on 08.02.16.
//

#ifndef GRPC_TEST_DUMMYJPS_H
#define GRPC_TEST_DUMMYJPS_H
#include <grpc++/grpc++.h>

#include "hybridsim.grpc.pb.h"
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using hybridsim::HybridSimulation;
using hybridsim::Agent;
using hybridsim::Agents;
using hybridsim::Boolean;
using hybridsim::Empty;
using hybridsim::LeftClosedRightOpenTimeInterval;
using hybridsim::Trajectories;
using hybridsim::Trajectory;

class DummyJPS : public HybridSimulation::Service {

public:
    DummyJPS();

    virtual ~DummyJPS();

    virtual ::grpc::Status simulatedTimeInerval(::grpc::ServerContext *context,
                                                const ::hybridsim::LeftClosedRightOpenTimeInterval *request,
                                                ::hybridsim::Empty *response) override;

    virtual ::grpc::Status transferAgent(::grpc::ServerContext *context, const ::hybridsim::Agent *request,
                                         ::hybridsim::Boolean *response) override;

    virtual ::grpc::Status receiveTrajectories(::grpc::ServerContext *context, const ::hybridsim::Empty *request,
                                               ::hybridsim::Trajectories *response) override;

    virtual ::grpc::Status retrieveAgents(::grpc::ServerContext *context, const ::hybridsim::Empty *request,
                                          ::hybridsim::Agents *response) override;

private:
    std::map<const std::string,DummyAgent*> agents;



    class DummyAgent {
    public:

        DummyAgent(double x, double y): x(x),y(y){};

        void proceed(){ x+= 1; y+= 1;}

        double getX(){return x;}
        double getY(){return y;}

    private:
        double x;
        double y;
    };
};


#endif //GRPC_TEST_DUMMYJPS_H