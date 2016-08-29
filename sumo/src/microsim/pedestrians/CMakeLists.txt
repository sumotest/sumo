message("entering pedestrians")

set(microsimpeds_files
	MSPerson.cpp MSPerson.h
	MSPModel.cpp MSPModel.h 
	MSPModel_Striping.cpp MSPModel_Striping.h 
	MSPModel_NonInteracting.cpp MSPModel_NonInteracting.h)	

add_library(mspedestrians STATIC ${microsimpeds_files})
