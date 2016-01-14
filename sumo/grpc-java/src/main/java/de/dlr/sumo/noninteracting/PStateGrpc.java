package de.dlr.sumo.noninteracting;

import static io.grpc.stub.ClientCalls.asyncUnaryCall;
import static io.grpc.stub.ClientCalls.asyncServerStreamingCall;
import static io.grpc.stub.ClientCalls.asyncClientStreamingCall;
import static io.grpc.stub.ClientCalls.asyncBidiStreamingCall;
import static io.grpc.stub.ClientCalls.blockingUnaryCall;
import static io.grpc.stub.ClientCalls.blockingServerStreamingCall;
import static io.grpc.stub.ClientCalls.futureUnaryCall;
import static io.grpc.MethodDescriptor.generateFullMethodName;
import static io.grpc.stub.ServerCalls.asyncUnaryCall;
import static io.grpc.stub.ServerCalls.asyncServerStreamingCall;
import static io.grpc.stub.ServerCalls.asyncClientStreamingCall;
import static io.grpc.stub.ServerCalls.asyncBidiStreamingCall;

@javax.annotation.Generated("by gRPC proto compiler")
public class PStateGrpc {

    private PStateGrpc() {
    }

    public static final String SERVICE_NAME = "noninteracting.PState";

    // Static method descriptors that strictly reflect the proto.
    @io.grpc.ExperimentalApi
    public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm,
            de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> METHOD_COMPUTE_WALKING_TIME =
            io.grpc.MethodDescriptor.create(
                    io.grpc.MethodDescriptor.MethodType.UNARY,
                    generateFullMethodName(
                            "noninteracting.PState", "computeWalkingTime"),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm.getDefaultInstance()),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime.getDefaultInstance()));
    @io.grpc.ExperimentalApi
    public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.GetPos,
            de.dlr.sumo.noninteracting.NonInteractingProto.Position> METHOD_GET_POSITION =
            io.grpc.MethodDescriptor.create(
                    io.grpc.MethodDescriptor.MethodType.UNARY,
                    generateFullMethodName(
                            "noninteracting.PState", "getPosition"),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.GetPos.getDefaultInstance()),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.Position.getDefaultInstance()));
    @io.grpc.ExperimentalApi
    public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle,
            de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> METHOD_GET_ANGLE =
            io.grpc.MethodDescriptor.create(
                    io.grpc.MethodDescriptor.MethodType.UNARY,
                    generateFullMethodName(
                            "noninteracting.PState", "getAngle"),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle.getDefaultInstance()),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal.getDefaultInstance()));
    @io.grpc.ExperimentalApi
    public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime,
            de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> METHOD_GET_WAITING_TIME =
            io.grpc.MethodDescriptor.create(
                    io.grpc.MethodDescriptor.MethodType.UNARY,
                    generateFullMethodName(
                            "noninteracting.PState", "getWaitingTime"),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime.getDefaultInstance()),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime.getDefaultInstance()));
    @io.grpc.ExperimentalApi
    public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking,
            de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> METHOD_GET_SPEED =
            io.grpc.MethodDescriptor.create(
                    io.grpc.MethodDescriptor.MethodType.UNARY,
                    generateFullMethodName(
                            "noninteracting.PState", "getSpeed"),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking.getDefaultInstance()),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal.getDefaultInstance()));
    @io.grpc.ExperimentalApi
    public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking,
            de.dlr.sumo.noninteracting.NonInteractingProto.Edge> METHOD_GET_NEXT_EDGE =
            io.grpc.MethodDescriptor.create(
                    io.grpc.MethodDescriptor.MethodType.UNARY,
                    generateFullMethodName(
                            "noninteracting.PState", "getNextEdge"),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking.getDefaultInstance()),
                    io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.Edge.getDefaultInstance()));

    public static PStateStub newStub(io.grpc.Channel channel) {
        return new PStateStub(channel);
    }

    public static PStateBlockingStub newBlockingStub(
            io.grpc.Channel channel) {
        return new PStateBlockingStub(channel);
    }

    public static PStateFutureStub newFutureStub(
            io.grpc.Channel channel) {
        return new PStateFutureStub(channel);
    }

    public static interface PState {

        public void computeWalkingTime(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request,
                                       io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> responseObserver);

        public void getPosition(de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request,
                                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.Position> responseObserver);

        public void getAngle(de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request,
                             io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> responseObserver);

        public void getWaitingTime(de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime request,
                                   io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> responseObserver);

        public void getSpeed(de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request,
                             io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> responseObserver);

        public void getNextEdge(de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request,
                                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.Edge> responseObserver);
    }

    public static interface PStateBlockingClient {

        public de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime computeWalkingTime(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request);

        public de.dlr.sumo.noninteracting.NonInteractingProto.Position getPosition(de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request);

        public de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal getAngle(de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request);

        public de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime getWaitingTime(de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime request);

        public de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal getSpeed(de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request);

        public de.dlr.sumo.noninteracting.NonInteractingProto.Edge getNextEdge(de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request);
    }

    public static interface PStateFutureClient {

        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> computeWalkingTime(
                de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request);

        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.Position> getPosition(
                de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request);

        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> getAngle(
                de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request);

        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> getWaitingTime(
                de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime request);

        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> getSpeed(
                de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request);

        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.Edge> getNextEdge(
                de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request);
    }

    public static class PStateStub extends io.grpc.stub.AbstractStub<PStateStub>
            implements PState {
        private PStateStub(io.grpc.Channel channel) {
            super(channel);
        }

        private PStateStub(io.grpc.Channel channel,
                           io.grpc.CallOptions callOptions) {
            super(channel, callOptions);
        }

        @java.lang.Override
        protected PStateStub build(io.grpc.Channel channel,
                                   io.grpc.CallOptions callOptions) {
            return new PStateStub(channel, callOptions);
        }

        @java.lang.Override
        public void computeWalkingTime(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request,
                                       io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> responseObserver) {
            asyncUnaryCall(
                    getChannel().newCall(METHOD_COMPUTE_WALKING_TIME, getCallOptions()), request, responseObserver);
        }

        @java.lang.Override
        public void getPosition(de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request,
                                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.Position> responseObserver) {
            asyncUnaryCall(
                    getChannel().newCall(METHOD_GET_POSITION, getCallOptions()), request, responseObserver);
        }

        @java.lang.Override
        public void getAngle(de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request,
                             io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> responseObserver) {
            asyncUnaryCall(
                    getChannel().newCall(METHOD_GET_ANGLE, getCallOptions()), request, responseObserver);
        }

        @java.lang.Override
        public void getWaitingTime(de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime request,
                                   io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> responseObserver) {
            asyncUnaryCall(
                    getChannel().newCall(METHOD_GET_WAITING_TIME, getCallOptions()), request, responseObserver);
        }

        @java.lang.Override
        public void getSpeed(de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request,
                             io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> responseObserver) {
            asyncUnaryCall(
                    getChannel().newCall(METHOD_GET_SPEED, getCallOptions()), request, responseObserver);
        }

        @java.lang.Override
        public void getNextEdge(de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request,
                                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.Edge> responseObserver) {
            asyncUnaryCall(
                    getChannel().newCall(METHOD_GET_NEXT_EDGE, getCallOptions()), request, responseObserver);
        }
    }

    public static class PStateBlockingStub extends io.grpc.stub.AbstractStub<PStateBlockingStub>
            implements PStateBlockingClient {
        private PStateBlockingStub(io.grpc.Channel channel) {
            super(channel);
        }

        private PStateBlockingStub(io.grpc.Channel channel,
                                   io.grpc.CallOptions callOptions) {
            super(channel, callOptions);
        }

        @java.lang.Override
        protected PStateBlockingStub build(io.grpc.Channel channel,
                                           io.grpc.CallOptions callOptions) {
            return new PStateBlockingStub(channel, callOptions);
        }

        @java.lang.Override
        public de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime computeWalkingTime(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request) {
            return blockingUnaryCall(
                    getChannel().newCall(METHOD_COMPUTE_WALKING_TIME, getCallOptions()), request);
        }

        @java.lang.Override
        public de.dlr.sumo.noninteracting.NonInteractingProto.Position getPosition(de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request) {
            return blockingUnaryCall(
                    getChannel().newCall(METHOD_GET_POSITION, getCallOptions()), request);
        }

        @java.lang.Override
        public de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal getAngle(de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request) {
            return blockingUnaryCall(
                    getChannel().newCall(METHOD_GET_ANGLE, getCallOptions()), request);
        }

        @java.lang.Override
        public de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime getWaitingTime(de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime request) {
            return blockingUnaryCall(
                    getChannel().newCall(METHOD_GET_WAITING_TIME, getCallOptions()), request);
        }

        @java.lang.Override
        public de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal getSpeed(de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request) {
            return blockingUnaryCall(
                    getChannel().newCall(METHOD_GET_SPEED, getCallOptions()), request);
        }

        @java.lang.Override
        public de.dlr.sumo.noninteracting.NonInteractingProto.Edge getNextEdge(de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request) {
            return blockingUnaryCall(
                    getChannel().newCall(METHOD_GET_NEXT_EDGE, getCallOptions()), request);
        }
    }

    public static class PStateFutureStub extends io.grpc.stub.AbstractStub<PStateFutureStub>
            implements PStateFutureClient {
        private PStateFutureStub(io.grpc.Channel channel) {
            super(channel);
        }

        private PStateFutureStub(io.grpc.Channel channel,
                                 io.grpc.CallOptions callOptions) {
            super(channel, callOptions);
        }

        @java.lang.Override
        protected PStateFutureStub build(io.grpc.Channel channel,
                                         io.grpc.CallOptions callOptions) {
            return new PStateFutureStub(channel, callOptions);
        }

        @java.lang.Override
        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> computeWalkingTime(
                de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request) {
            return futureUnaryCall(
                    getChannel().newCall(METHOD_COMPUTE_WALKING_TIME, getCallOptions()), request);
        }

        @java.lang.Override
        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.Position> getPosition(
                de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request) {
            return futureUnaryCall(
                    getChannel().newCall(METHOD_GET_POSITION, getCallOptions()), request);
        }

        @java.lang.Override
        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> getAngle(
                de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request) {
            return futureUnaryCall(
                    getChannel().newCall(METHOD_GET_ANGLE, getCallOptions()), request);
        }

        @java.lang.Override
        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> getWaitingTime(
                de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime request) {
            return futureUnaryCall(
                    getChannel().newCall(METHOD_GET_WAITING_TIME, getCallOptions()), request);
        }

        @java.lang.Override
        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> getSpeed(
                de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request) {
            return futureUnaryCall(
                    getChannel().newCall(METHOD_GET_SPEED, getCallOptions()), request);
        }

        @java.lang.Override
        public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.Edge> getNextEdge(
                de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request) {
            return futureUnaryCall(
                    getChannel().newCall(METHOD_GET_NEXT_EDGE, getCallOptions()), request);
        }
    }

    public static io.grpc.ServerServiceDefinition bindService(
            final PState serviceImpl) {
        return io.grpc.ServerServiceDefinition.builder(SERVICE_NAME)
                .addMethod(
                        METHOD_COMPUTE_WALKING_TIME,
                        asyncUnaryCall(
                                new io.grpc.stub.ServerCalls.UnaryMethod<
                                        de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm,
                                        de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime>() {
                                    @java.lang.Override
                                    public void invoke(
                                            de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request,
                                            io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> responseObserver) {
                                        serviceImpl.computeWalkingTime(request, responseObserver);
                                    }
                                }))
                .addMethod(
                        METHOD_GET_POSITION,
                        asyncUnaryCall(
                                new io.grpc.stub.ServerCalls.UnaryMethod<
                                        de.dlr.sumo.noninteracting.NonInteractingProto.GetPos,
                                        de.dlr.sumo.noninteracting.NonInteractingProto.Position>() {
                                    @java.lang.Override
                                    public void invoke(
                                            de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request,
                                            io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.Position> responseObserver) {
                                        serviceImpl.getPosition(request, responseObserver);
                                    }
                                }))
                .addMethod(
                        METHOD_GET_ANGLE,
                        asyncUnaryCall(
                                new io.grpc.stub.ServerCalls.UnaryMethod<
                                        de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle,
                                        de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal>() {
                                    @java.lang.Override
                                    public void invoke(
                                            de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request,
                                            io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> responseObserver) {
                                        serviceImpl.getAngle(request, responseObserver);
                                    }
                                }))
                .addMethod(
                        METHOD_GET_WAITING_TIME,
                        asyncUnaryCall(
                                new io.grpc.stub.ServerCalls.UnaryMethod<
                                        de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime,
                                        de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime>() {
                                    @java.lang.Override
                                    public void invoke(
                                            de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime request,
                                            io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOTime> responseObserver) {
                                        serviceImpl.getWaitingTime(request, responseObserver);
                                    }
                                }))
                .addMethod(
                        METHOD_GET_SPEED,
                        asyncUnaryCall(
                                new io.grpc.stub.ServerCalls.UnaryMethod<
                                        de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking,
                                        de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal>() {
                                    @java.lang.Override
                                    public void invoke(
                                            de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request,
                                            io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.SUMOReal> responseObserver) {
                                        serviceImpl.getSpeed(request, responseObserver);
                                    }
                                }))
                .addMethod(
                        METHOD_GET_NEXT_EDGE,
                        asyncUnaryCall(
                                new io.grpc.stub.ServerCalls.UnaryMethod<
                                        de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking,
                                        de.dlr.sumo.noninteracting.NonInteractingProto.Edge>() {
                                    @java.lang.Override
                                    public void invoke(
                                            de.dlr.sumo.noninteracting.NonInteractingProto.MSPersonStage_Walking request,
                                            io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.Edge> responseObserver) {
                                        serviceImpl.getNextEdge(request, responseObserver);
                                    }
                                })).build();
    }
}
