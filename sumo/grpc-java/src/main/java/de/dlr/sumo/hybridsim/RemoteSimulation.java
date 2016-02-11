/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

package de.dlr.sumo.hybridsim;


import io.grpc.Server;
import io.grpc.ServerBuilder;
import io.grpc.stub.StreamObserver;

import java.util.logging.Logger;

/**
 * Created by laemmel on 11.02.16.
 */
public class RemoteSimulation {

    private static Logger log = Logger.getLogger(RemoteSimulation.class.getName());
    private Server server;
    private int port = 50051;

    private void start() throws Exception {
        server = ServerBuilder.forPort(port)
                .addService(HybridSimulationGrpc.bindService(new HybridSimImpl()))
                .build()
                .start();
        log.info("Server started, listening on " + port);
        Runtime.getRuntime().addShutdownHook(new Thread() {
            @Override
            public void run() {
                // Use stderr here since the logger may have been reset by its JVM shutdown hook.
                System.err.println("*** shutting down gRPC server since JVM is shutting down");
                this.stop();
                System.err.println("*** server shut down");
            }
        });


    }
    private void stop() {
        if (server != null) {
            server.shutdown();
        }
    }

    /**
     * Await termination on the main thread since the grpc library uses daemon threads.
     */
    private void blockUntilShutdown() throws InterruptedException {
        if (server != null) {
            server.awaitTermination();
        }
    }


    public static void main(String [] args) throws Exception {
        RemoteSimulation server = new RemoteSimulation();
        server.start();
        server.blockUntilShutdown();
    }

    private static final class HybridSimImpl implements HybridSimulationGrpc.HybridSimulation {

        @Override
        public void simulatedTimeInerval(HybridSimProto.LeftClosedRightOpenTimeInterval request, StreamObserver<HybridSimProto.Empty> responseObserver) {
            throw new RuntimeException("implement me!");
        }

        @Override
        public void transferAgent(HybridSimProto.Agent request, StreamObserver<HybridSimProto.Boolean> responseObserver) {
            throw new RuntimeException("implement me!");
        }

        @Override
        public void receiveTrajectories(HybridSimProto.Empty request, StreamObserver<HybridSimProto.Trajectories> responseObserver) {
            throw new RuntimeException("implement me!");
        }

        @Override
        public void retrieveAgents(HybridSimProto.Empty request, StreamObserver<HybridSimProto.Agents> responseObserver) {
            throw new RuntimeException("implement me!");
        }
    }
}
