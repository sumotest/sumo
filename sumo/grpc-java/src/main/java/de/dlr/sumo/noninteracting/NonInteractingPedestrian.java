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
            double time = request.getSumoTime().getSumoTime();
            System.out.println(time);

            NonInteractingProto.PBEdge prev = request.getPrev();
            printEdge(prev);

            NonInteractingProto.PBEdge edge = request.getStage().getEdge();
//            printEdge(edge);
            NonInteractingProto.PBEdge nxt = request.getStage().getNextRouteEdge();
//            printEdge(nxt);
            printStage(request.getStage());
            int dir = UNDF;
            double beginPos;
            double endPos;

            if (!prev.getFromJunctionId().isEmpty()) {
                beginPos = request.getStage().getDepartPos();
            } else {
                dir = (edge.getToJunctionId() == prev.getToJunctionId() || edge.getToJunctionId() == prev.getFromJunctionId()) ? BCKWD : FWD;
                beginPos = dir == FWD ? 0 : edge.getLength();
            }
            if (!nxt.getFromJunctionId().isEmpty()) {
                endPos = request.getStage().getArrivalPos();
            } else {
                if (dir == UNDF) {
                    dir = (edge.getToJunctionId() == prev.getToJunctionId() || edge.getToJunctionId() == prev.getFromJunctionId()) ? BCKWD : FWD;
                }
                endPos = dir == FWD ? edge.getLength() : 0;
            }

            double duration = MAX2(1, Math.abs(endPos-beginPos)/request.getStage().getMaxSpeed());

            System.out.println("duration:" + duration);

            NonInteractingProto.PBSUMOTime t = NonInteractingProto.PBSUMOTime.newBuilder().setSumoTime(duration).build();
            responseObserver.onNext(t);
            responseObserver.onCompleted();

        }

        @Override
        public void computeWalkingTimeFlat(NonInteractingProto.CMPWlkgTm_flat request, StreamObserver<NonInteractingProto.PBSUMOTime> responseObserver) {
            String prevFromId = request.getPrevFromId();
            String prevToId = request.getPrevToId();
            double prevLen = request.getPrevLen();
            String currentFromId = request.getCurrentFromId();
            String currentToId = request.getCurrentToId();
            double currentLen = request.getCurrentLen();
            String nextFromId = request.getNextFromId();
            String nextToId = request.getNextToId();
            double nextLen = request.getNextLen();
            double depPos = request.getDepPos();
            double arrPos = request.getArrivalPos();
            double mxSpd = request.getMaxSpeed();
            double time = request.getTime();

            int dir = UNDF;
            double beginPos;
            double endPos;
            System.out.println(time + " prevFrom: " + prevFromId + " prevTo: " + prevToId + " prevLen: " + prevLen + " currentFrom:" + currentFromId + " currentTo:" +
            currentToId + " currentLen:" + currentLen + " nextFrom:" + nextFromId + " nextTo:" + nextToId + " depPos:" + depPos + " arrPos:" + arrPos + " mxSpd:" +
            mxSpd + " time:" + time);

            if (!prevFromId.isEmpty()) {
                beginPos = depPos;
            } else {
                dir = currentToId.equals(prevToId) || currentToId.equals(prevFromId) ? BCKWD : FWD;
                beginPos = dir == FWD ? 0 : currentLen;
            }
            if (!nextFromId.isEmpty()) {
                endPos = arrPos;
            } else {
                if (dir == UNDF) {
                    dir = currentToId.equals(prevToId) || currentToId.equals(prevFromId) ? BCKWD : FWD;
                }
                endPos = dir == FWD ? currentLen : 0;
            }

            double duration = MAX2(1, Math.abs(endPos-beginPos)/mxSpd);

            System.out.println("duration:" + duration);


            NonInteractingProto.PBSUMOTime t = NonInteractingProto.PBSUMOTime.newBuilder().setSumoTime(duration).build();
            responseObserver.onNext(t);
            responseObserver.onCompleted();

        }

        private void printStage(NonInteractingProto.PBMSPersonStage_Walking stage) {
            double spd = stage.getMaxSpeed();
            System.out.println("max spd: " + spd);
            printEdge(stage.getEdge());
            printEdge(stage.getNextRouteEdge());
        }

        private void printEdge(NonInteractingProto.PBEdge prev) {
            StringBuffer buf = new StringBuffer();
            buf.append("Edge length: ");
            buf.append(prev.getLength());
            buf.append(" from: ");
            buf.append(prev.getFromJunctionId().toString());
            buf.append(" to: ");
            buf.append(prev.getToJunctionId().toString());
            buf.append("\n");
            System.out.println(buf.toString());
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
