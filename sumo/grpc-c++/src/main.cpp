#include <iostream>

#include <grpc++/grpc++.h>
#include <math.h>
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

//        std::cout << time << std::endl;
        rpl->set_mylastentrytime(time);

        int dir = UNDF;
        double beg, end, dur;
        if (prev.length() == 0) {
            beg = st.departpos();
        } else {
            dir = (curr.tojunctionid() == prev.tojunctionid() || curr.tojunctionid() == prev.fromjunctionid()) ? BWD : FWD;
            beg = dir == FWD ? 0 : curr.length();
        }
        if (nxt.length() == 0) {
            end = arrPos;
        } else {
            if (dir == UNDF) {
                dir = (curr.fromjunctionid() == nxt.fromjunctionid() || curr.fromjunctionid() == nxt.tojunctionid()) ? BWD : FWD;
            }
            end = dir == FWD ? curr.length() : 0;
        }

        dur = fabs(beg-end)/mxSpd;

        rpl->set_mycurrentbeginpos(beg);
        rpl->set_mycurrentendpos(end);
        rpl->mutable_duration()->set_sumotime(dur);

        return Status::OK;

    }

    Status getEdgePost(::grpc::ServerContext *context,
                       const GetEdgePos *req,
                       PBSUMOReal *rpl) override {

        double beg = req->mycurrentbeginpos();
        double end = req->mycurrentendpos();
        double dur = req->mycurrentduration();
        double last = req->mylastentrytime().sumotime();
        double now = req->time().sumotime();

        double pos = beg + (end-beg)/dur*(now-last);
        rpl->set_sumoreal(pos);

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