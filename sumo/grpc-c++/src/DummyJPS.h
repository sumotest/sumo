//
// Created by laemmel on 08.02.16.
//

#ifndef GRPC_TEST_DUMMYJPS_H
#define GRPC_TEST_DUMMYJPS_H
#include <grpc++/grpc++.h>
#include <vector>
#include <math.h>


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

class DummyJPS final : public HybridSimulation::Service {

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



    struct Link {
        double x0;
        double y0;
        double x1;
        double y1;
        std::string id;
    };

    class DummyAgent {
    public:

        DummyAgent(double x, double y, const std::string id, std::vector<Link> links): id(id),x(x),y(y),links(links){
            lIt = links.begin();

            updateDeltas();
        };

        void updateDeltas();

        void proceed();//{ x+= 1; y+= 1;}

        double getX(){return x;}
        double getY(){return y;}
        const std::string getID(){return id;}
        std::string getCurrentLinkID(){
            if (current == 0) {
                return "";
            }
            return current->id;
        }

    private:
        double x;
        double y;
        double dx;
        double dy;
        const std::string id;
        std::vector<Link> links;

        std::vector<DummyJPS::Link>::iterator lIt;

        double sqLength;
        const double spd = 1.34;
        Link * current;
        double cY = 0;
        double cX = 0;

    };


private:
    std::map<const std::string,DummyAgent*> agents;
};


#endif //GRPC_TEST_DUMMYJPS_H
