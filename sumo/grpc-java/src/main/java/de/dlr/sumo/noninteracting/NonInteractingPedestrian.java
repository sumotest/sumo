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

package de.dlr.sumo.noninteracting;

import io.grpc.Server;
import io.grpc.ServerBuilder;
import io.grpc.stub.StreamObserver;

import java.util.logging.Logger;

/**
 * Created by laemmel on 14.01.16.
 */
public class NonInteractingPedestrian {

    private static final Logger logger = Logger.getLogger(NonInteractingPedestrian.class.getName());

    private int port = 8081;
    private Server server;

    private void start() throws Exception {
        server = ServerBuilder.forPort(port)
                .addService(PStateGrpc.bindService(new PStateImpl()))
                .build()
                .start();
        logger.info("Server started, listening on " + port);
        Runtime.getRuntime().addShutdownHook(new Thread() {
            @Override
            public void run() {
                // Use stderr here since the logger may have been reset by its JVM shutdown hook.
                System.err.println("*** shutting down gRPC server since JVM is shutting down");
                NonInteractingPedestrian.this.stop();
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


    public static void main(String[] args) throws Exception{
    final NonInteractingPedestrian server = new NonInteractingPedestrian();
    server.start();
    server.blockUntilShutdown();

    }

    private class PStateImpl implements PStateGrpc.PState {

        @Override
        public void computeWalkingTime(NonInteractingProto.CMPWlkgTm request, StreamObserver<NonInteractingProto.SUMOTime> responseObserver) {

        }

        @Override
        public void getPosition(NonInteractingProto.GetPos request, StreamObserver<NonInteractingProto.Position> responseObserver) {

        }

        @Override
        public void getAngle(NonInteractingProto.GetAngle request, StreamObserver<NonInteractingProto.SUMOReal> responseObserver) {

        }

        @Override
        public void getWaitingTime(NonInteractingProto.SUMOTime request, StreamObserver<NonInteractingProto.SUMOTime> responseObserver) {
            NonInteractingProto.SUMOTime replay = NonInteractingProto.SUMOTime.newBuilder().setSumoTime(0).build();
            responseObserver.onNext(replay);
            responseObserver.onCompleted();

        }

        @Override
        public void getSpeed(NonInteractingProto.MSPersonStage_Walking request, StreamObserver<NonInteractingProto.SUMOReal> responseObserver) {

        }

        @Override
        public void getNextEdge(NonInteractingProto.MSPersonStage_Walking request, StreamObserver<NonInteractingProto.Edge> responseObserver) {

        }
    }
}
