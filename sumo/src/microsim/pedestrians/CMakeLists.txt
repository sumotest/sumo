message("entering pedestrians")

set(microsimpeds_files
	MSPerson.cpp MSPerson.h
	MSPModel.cpp MSPModel.h 
	MSPModel_Striping.cpp MSPModel_Striping.h 
	MSPModel_NonInteracting.cpp MSPModel_NonInteracting.h)	



if(GRPC_FOUND)

    set(microsimpeds_files ${microsimpeds_files} hybridsim.grpc.pb.cc hybridsim.grpc.pb.h hybridsim.pb.cc hybridsim.pb.h RemoteModel.cpp RemoteModel.h)

endif()


add_library(mspedestrians STATIC ${microsimpeds_files})
