message("entering pedestrians")

set(microsimpeds_files
	MSPModel.cpp MSPModel.h 
	MSPModel_Striping.cpp MSPModel_Striping.h 
	MSPModel_NonInteracting.cpp MSPModel_NonInteracting.h 
	MSGRPCClient.cpp MSGRPCClient.h 
	MSPRCPState.cpp MSPRCPState.h)
add_library(mspedestrians STATIC ${microsimpeds_files})
