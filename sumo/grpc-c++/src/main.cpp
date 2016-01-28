#include <iostream>

#include <grpc++/grpc++.h>
#include "noninteracting.grpc.pb.h"



#define UNDF 0
#define FWD 1
#define BWD -1

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using noninteracting::PBPState;
using noninteracting::CMPWlkgTm;
using noninteracting::CMPWlkgTmRpl;
using noninteracting::GetEdgePos;
using noninteracting::PBEdge;
using noninteracting::PBMSPersonStage_Walking;
using noninteracting::PBSUMOReal;
using noninteracting::PBSUMOTime;

class PStateImpl final : public PBPState::Service {

    Status computeWalkingTime(ServerContext *context,
                              const CMPWlkgTm *req,
                              CMPWlkgTmRpl *rpl) override {

        PBEdge prev  = req->prev();
        PBMSPersonStage_Walking st = req->stage();
        PBEdge curr = st.edge();
        PBEdge nxt = st.nextrouteedge();
        double mxSpd = st.maxspeed();
        double depPos = st.departpos();
        double arrPos = st.arrivalpos();
        double time = req->sumotime().sumotime();

        std::cout << time << std::endl;
        rpl->set_mylastentrytime(time);

        int dir = UNDF;

//        if (prev == 0) {
//
//        }

        return Status::OK;

    }

    Status getEdgePost(::grpc::ServerContext *context,
                       const GetEdgePos *request,
                       PBSUMOReal *response) override {
        return Status::OK;
    }

    Status getWaitingTime(ServerContext *context,
                          const noninteracting::PBSUMOTime *request,
                          PBSUMOTime *response) override {

        std::cout << request->sumotime() << std::endl;
        response->set_sumotime(0.);
        return Status::OK;
    }

};


int main(int argc, char** argv) {
    std::cout << "Hello, World!" << std::endl;
    std::string server_address("0.0.0.0:50051");
    PStateImpl service;


    ServerBuilder builder;

    builder.AddListeningPort(server_address,grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());

    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();

//    builder.BuildAndStart();

//    std::cout << grpc_test_VERSION_MAJOR << " " << grpc_test_VERSION_MINOR << std::endl;
    return 0;
}