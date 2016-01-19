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
public class PBPStateGrpc {

  private PBPStateGrpc() {}

  public static final String SERVICE_NAME = "noninteracting.PBPState";

  // Static method descriptors that strictly reflect the proto.
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm,
      de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> METHOD_COMPUTE_WALKING_TIME =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "noninteracting.PBPState", "computeWalkingTime"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime.getDefaultInstance()));
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.GetPos,
      de.dlr.sumo.noninteracting.NonInteractingProto.PBPosition> METHOD_GET_POSITION =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "noninteracting.PBPState", "getPosition"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.GetPos.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.PBPosition.getDefaultInstance()));
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle,
      de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> METHOD_GET_ANGLE =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "noninteracting.PBPState", "getAngle"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal.getDefaultInstance()));
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime,
      de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> METHOD_GET_WAITING_TIME =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "noninteracting.PBPState", "getWaitingTime"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime.getDefaultInstance()));
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking,
      de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> METHOD_GET_SPEED =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "noninteracting.PBPState", "getSpeed"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal.getDefaultInstance()));
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking,
      de.dlr.sumo.noninteracting.NonInteractingProto.PBEdge> METHOD_GET_NEXT_EDGE =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "noninteracting.PBPState", "getNextEdge"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.PBEdge.getDefaultInstance()));

  public static PBPStateStub newStub(io.grpc.Channel channel) {
    return new PBPStateStub(channel);
  }

  public static PBPStateBlockingStub newBlockingStub(
      io.grpc.Channel channel) {
    return new PBPStateBlockingStub(channel);
  }

  public static PBPStateFutureStub newFutureStub(
      io.grpc.Channel channel) {
    return new PBPStateFutureStub(channel);
  }

  public static interface PBPState {

    public void computeWalkingTime(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> responseObserver);

    public void getPosition(de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBPosition> responseObserver);

    public void getAngle(de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> responseObserver);

    public void getWaitingTime(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> responseObserver);

    public void getSpeed(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> responseObserver);

    public void getNextEdge(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBEdge> responseObserver);
  }

  public static interface PBPStateBlockingClient {

    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime computeWalkingTime(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request);

    public de.dlr.sumo.noninteracting.NonInteractingProto.PBPosition getPosition(de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request);

    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal getAngle(de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request);

    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime getWaitingTime(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime request);

    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal getSpeed(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request);

    public de.dlr.sumo.noninteracting.NonInteractingProto.PBEdge getNextEdge(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request);
  }

  public static interface PBPStateFutureClient {

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> computeWalkingTime(
        de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBPosition> getPosition(
        de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> getAngle(
        de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> getWaitingTime(
        de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> getSpeed(
        de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBEdge> getNextEdge(
        de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request);
  }

  public static class PBPStateStub extends io.grpc.stub.AbstractStub<PBPStateStub>
      implements PBPState {
    private PBPStateStub(io.grpc.Channel channel) {
      super(channel);
    }

    private PBPStateStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected PBPStateStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new PBPStateStub(channel, callOptions);
    }

    @java.lang.Override
    public void computeWalkingTime(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_COMPUTE_WALKING_TIME, getCallOptions()), request, responseObserver);
    }

    @java.lang.Override
    public void getPosition(de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBPosition> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_GET_POSITION, getCallOptions()), request, responseObserver);
    }

    @java.lang.Override
    public void getAngle(de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_GET_ANGLE, getCallOptions()), request, responseObserver);
    }

    @java.lang.Override
    public void getWaitingTime(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_GET_WAITING_TIME, getCallOptions()), request, responseObserver);
    }

    @java.lang.Override
    public void getSpeed(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_GET_SPEED, getCallOptions()), request, responseObserver);
    }

    @java.lang.Override
    public void getNextEdge(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBEdge> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_GET_NEXT_EDGE, getCallOptions()), request, responseObserver);
    }
  }

  public static class PBPStateBlockingStub extends io.grpc.stub.AbstractStub<PBPStateBlockingStub>
      implements PBPStateBlockingClient {
    private PBPStateBlockingStub(io.grpc.Channel channel) {
      super(channel);
    }

    private PBPStateBlockingStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected PBPStateBlockingStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new PBPStateBlockingStub(channel, callOptions);
    }

    @java.lang.Override
    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime computeWalkingTime(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_COMPUTE_WALKING_TIME, getCallOptions()), request);
    }

    @java.lang.Override
    public de.dlr.sumo.noninteracting.NonInteractingProto.PBPosition getPosition(de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_GET_POSITION, getCallOptions()), request);
    }

    @java.lang.Override
    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal getAngle(de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_GET_ANGLE, getCallOptions()), request);
    }

    @java.lang.Override
    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime getWaitingTime(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_GET_WAITING_TIME, getCallOptions()), request);
    }

    @java.lang.Override
    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal getSpeed(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_GET_SPEED, getCallOptions()), request);
    }

    @java.lang.Override
    public de.dlr.sumo.noninteracting.NonInteractingProto.PBEdge getNextEdge(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_GET_NEXT_EDGE, getCallOptions()), request);
    }
  }

  public static class PBPStateFutureStub extends io.grpc.stub.AbstractStub<PBPStateFutureStub>
      implements PBPStateFutureClient {
    private PBPStateFutureStub(io.grpc.Channel channel) {
      super(channel);
    }

    private PBPStateFutureStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected PBPStateFutureStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new PBPStateFutureStub(channel, callOptions);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> computeWalkingTime(
        de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_COMPUTE_WALKING_TIME, getCallOptions()), request);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBPosition> getPosition(
        de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_GET_POSITION, getCallOptions()), request);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> getAngle(
        de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_GET_ANGLE, getCallOptions()), request);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> getWaitingTime(
        de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_GET_WAITING_TIME, getCallOptions()), request);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> getSpeed(
        de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_GET_SPEED, getCallOptions()), request);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBEdge> getNextEdge(
        de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_GET_NEXT_EDGE, getCallOptions()), request);
    }
  }

  public static io.grpc.ServerServiceDefinition bindService(
      final PBPState serviceImpl) {
    return io.grpc.ServerServiceDefinition.builder(SERVICE_NAME)
      .addMethod(
        METHOD_COMPUTE_WALKING_TIME,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm,
              de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> responseObserver) {
              serviceImpl.computeWalkingTime(request, responseObserver);
            }
          }))
      .addMethod(
        METHOD_GET_POSITION,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.noninteracting.NonInteractingProto.GetPos,
              de.dlr.sumo.noninteracting.NonInteractingProto.PBPosition>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.noninteracting.NonInteractingProto.GetPos request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBPosition> responseObserver) {
              serviceImpl.getPosition(request, responseObserver);
            }
          }))
      .addMethod(
        METHOD_GET_ANGLE,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle,
              de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.noninteracting.NonInteractingProto.GetAngle request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> responseObserver) {
              serviceImpl.getAngle(request, responseObserver);
            }
          }))
      .addMethod(
        METHOD_GET_WAITING_TIME,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime,
              de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> responseObserver) {
              serviceImpl.getWaitingTime(request, responseObserver);
            }
          }))
      .addMethod(
        METHOD_GET_SPEED,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking,
              de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> responseObserver) {
              serviceImpl.getSpeed(request, responseObserver);
            }
          }))
      .addMethod(
        METHOD_GET_NEXT_EDGE,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking,
              de.dlr.sumo.noninteracting.NonInteractingProto.PBEdge>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBEdge> responseObserver) {
              serviceImpl.getNextEdge(request, responseObserver);
            }
          })).build();
  }
}
