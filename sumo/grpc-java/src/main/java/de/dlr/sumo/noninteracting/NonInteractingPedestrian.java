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

    private int port = 50051;
    private Server server;

    private static int FWD = 1;
    private static int BCKWD = -1;
    private static int UNDF = 0;

    private void start() throws Exception {
        server = ServerBuilder.forPort(port)
                .addService(PBPStateGrpc.bindService(new PStateImpl()))
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

    private class PStateImpl implements PBPStateGrpc.PBPState {

        @Override
        public void computeWalkingTime(NonInteractingProto.CMPWlkgTm request, StreamObserver<NonInteractingProto.PBSUMOTime> responseObserver) {
            double currentTime = request.getSumoTime().getSumoTime();
            NonInteractingProto.PBEdge prev = request.getPrev();
            NonInteractingProto.PBEdge edge = request.getStage().getEdge();
            NonInteractingProto.PBEdge nxt = request.getStage().getNextRouteEdge();
            int dir = UNDF;
            double beginPos;
            double endPos;

            if (prev != null) {
                beginPos = request.getStage().getDepartPos();
            } else {
                dir = (edge.getToJunctionId() == prev.getToJunctionId() || edge.getToJunctionId() == prev.getFromJunctionId()) ? BCKWD : FWD;
                beginPos = dir == FWD ? 0 : edge.getLength();
            }
            if (nxt == null) {
                endPos = request.getStage().getArrivalPos();
            } else {
                if (dir == UNDF) {
                    dir = (edge.getToJunctionId() == prev.getToJunctionId() || edge.getToJunctionId() == prev.getFromJunctionId()) ? BCKWD : FWD;
                }
                endPos = dir == FWD ? edge.getLength() : 0;
            }

            double duration = MAX2(1, Math.abs(endPos-beginPos)/request.getStage().getMaxSpeed());

            System.out.println("duration:" + duration);

            NonInteractingProto.PBSUMOTime t = NonInteractingProto.PBSUMOTime.newBuilder().setSumoTime(10.).build();
            responseObserver.onNext(t);
            responseObserver.onCompleted();

        }

        @Override
        public void getPosition(NonInteractingProto.GetPos request, StreamObserver<NonInteractingProto.PBPosition> responseObserver) {

        }

        @Override
        public void getAngle(NonInteractingProto.GetAngle request, StreamObserver<NonInteractingProto.PBSUMOReal> responseObserver) {

        }

        @Override
        public void getWaitingTime(NonInteractingProto.PBSUMOTime request, StreamObserver<NonInteractingProto.PBSUMOTime> responseObserver) {

            NonInteractingProto.PBSUMOTime replay = NonInteractingProto.PBSUMOTime.newBuilder().setSumoTime(0).build();
            responseObserver.onNext(replay);
            responseObserver.onCompleted();

        }

        @Override
        public void getSpeed(NonInteractingProto.PBMSPersonStage_Walking request, StreamObserver<NonInteractingProto.PBSUMOReal> responseObserver) {

        }

        @Override
        public void getNextEdge(NonInteractingProto.PBMSPersonStage_Walking request, StreamObserver<NonInteractingProto.PBEdge> responseObserver) {

        }
    }

    private static double MAX2(double a, double b) {
        return a > b ? a : b;
    }
}
