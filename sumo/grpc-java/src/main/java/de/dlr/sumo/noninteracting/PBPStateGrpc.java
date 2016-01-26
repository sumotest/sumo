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
      de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTmRpl> METHOD_COMPUTE_WALKING_TIME =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "noninteracting.PBPState", "computeWalkingTime"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTmRpl.getDefaultInstance()));
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
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.noninteracting.NonInteractingProto.GetEdgePos,
      de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> METHOD_GET_EDGE_POST =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "noninteracting.PBPState", "getEdgePost"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.GetEdgePos.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal.getDefaultInstance()));

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
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTmRpl> responseObserver);

    public void getWaitingTime(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> responseObserver);

    public void getSpeed(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> responseObserver);

    public void getEdgePost(de.dlr.sumo.noninteracting.NonInteractingProto.GetEdgePos request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> responseObserver);
  }

  public static interface PBPStateBlockingClient {

    public de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTmRpl computeWalkingTime(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request);

    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime getWaitingTime(de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime request);

    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal getSpeed(de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request);

    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal getEdgePost(de.dlr.sumo.noninteracting.NonInteractingProto.GetEdgePos request);
  }

  public static interface PBPStateFutureClient {

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTmRpl> computeWalkingTime(
        de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime> getWaitingTime(
        de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOTime request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> getSpeed(
        de.dlr.sumo.noninteracting.NonInteractingProto.PBMSPersonStage_Walking request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> getEdgePost(
        de.dlr.sumo.noninteracting.NonInteractingProto.GetEdgePos request);
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
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTmRpl> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_COMPUTE_WALKING_TIME, getCallOptions()), request, responseObserver);
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
    public void getEdgePost(de.dlr.sumo.noninteracting.NonInteractingProto.GetEdgePos request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_GET_EDGE_POST, getCallOptions()), request, responseObserver);
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
    public de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTmRpl computeWalkingTime(de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_COMPUTE_WALKING_TIME, getCallOptions()), request);
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
    public de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal getEdgePost(de.dlr.sumo.noninteracting.NonInteractingProto.GetEdgePos request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_GET_EDGE_POST, getCallOptions()), request);
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
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTmRpl> computeWalkingTime(
        de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_COMPUTE_WALKING_TIME, getCallOptions()), request);
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
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> getEdgePost(
        de.dlr.sumo.noninteracting.NonInteractingProto.GetEdgePos request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_GET_EDGE_POST, getCallOptions()), request);
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
              de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTmRpl>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTm request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.CMPWlkgTmRpl> responseObserver) {
              serviceImpl.computeWalkingTime(request, responseObserver);
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
        METHOD_GET_EDGE_POST,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.noninteracting.NonInteractingProto.GetEdgePos,
              de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.noninteracting.NonInteractingProto.GetEdgePos request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.noninteracting.NonInteractingProto.PBSUMOReal> responseObserver) {
              serviceImpl.getEdgePost(request, responseObserver);
            }
          })).build();
  }
}
