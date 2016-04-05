package de.dlr.sumo.hybridsim;

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
public class HybridSimulationGrpc {

  private HybridSimulationGrpc() {}

  public static final String SERVICE_NAME = "hybridsim.HybridSimulation";

  // Static method descriptors that strictly reflect the proto.
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.hybridsim.HybridSimProto.LeftClosedRightOpenTimeInterval,
      de.dlr.sumo.hybridsim.HybridSimProto.Empty> METHOD_SIMULATED_TIME_INERVAL =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "hybridsim.HybridSimulation", "simulatedTimeInerval"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.LeftClosedRightOpenTimeInterval.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.Empty.getDefaultInstance()));
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.hybridsim.HybridSimProto.Agent,
      de.dlr.sumo.hybridsim.HybridSimProto.Boolean> METHOD_TRANSFER_AGENT =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "hybridsim.HybridSimulation", "transferAgent"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.Agent.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.Boolean.getDefaultInstance()));
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.hybridsim.HybridSimProto.Empty,
      de.dlr.sumo.hybridsim.HybridSimProto.Trajectories> METHOD_RECEIVE_TRAJECTORIES =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "hybridsim.HybridSimulation", "receiveTrajectories"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.Empty.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.Trajectories.getDefaultInstance()));
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.hybridsim.HybridSimProto.Empty,
      de.dlr.sumo.hybridsim.HybridSimProto.Agents> METHOD_RETRIEVE_AGENTS =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "hybridsim.HybridSimulation", "retrieveAgents"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.Empty.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.Agents.getDefaultInstance()));
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.hybridsim.HybridSimProto.Empty,
      de.dlr.sumo.hybridsim.HybridSimProto.Empty> METHOD_SHUTDOWN =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "hybridsim.HybridSimulation", "shutdown"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.Empty.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.Empty.getDefaultInstance()));
  @io.grpc.ExperimentalApi
  public static final io.grpc.MethodDescriptor<de.dlr.sumo.hybridsim.HybridSimProto.Scenario,
      de.dlr.sumo.hybridsim.HybridSimProto.Empty> METHOD_INIT_SCENARIO =
      io.grpc.MethodDescriptor.create(
          io.grpc.MethodDescriptor.MethodType.UNARY,
          generateFullMethodName(
              "hybridsim.HybridSimulation", "initScenario"),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.Scenario.getDefaultInstance()),
          io.grpc.protobuf.ProtoUtils.marshaller(de.dlr.sumo.hybridsim.HybridSimProto.Empty.getDefaultInstance()));

  public static HybridSimulationStub newStub(io.grpc.Channel channel) {
    return new HybridSimulationStub(channel);
  }

  public static HybridSimulationBlockingStub newBlockingStub(
      io.grpc.Channel channel) {
    return new HybridSimulationBlockingStub(channel);
  }

  public static HybridSimulationFutureStub newFutureStub(
      io.grpc.Channel channel) {
    return new HybridSimulationFutureStub(channel);
  }

  public static interface HybridSimulation {

    public void simulatedTimeInerval(de.dlr.sumo.hybridsim.HybridSimProto.LeftClosedRightOpenTimeInterval request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Empty> responseObserver);

    public void transferAgent(de.dlr.sumo.hybridsim.HybridSimProto.Agent request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Boolean> responseObserver);

    public void receiveTrajectories(de.dlr.sumo.hybridsim.HybridSimProto.Empty request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Trajectories> responseObserver);

    public void retrieveAgents(de.dlr.sumo.hybridsim.HybridSimProto.Empty request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Agents> responseObserver);

    public void shutdown(de.dlr.sumo.hybridsim.HybridSimProto.Empty request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Empty> responseObserver);

    public void initScenario(de.dlr.sumo.hybridsim.HybridSimProto.Scenario request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Empty> responseObserver);
  }

  public static interface HybridSimulationBlockingClient {

    public de.dlr.sumo.hybridsim.HybridSimProto.Empty simulatedTimeInerval(de.dlr.sumo.hybridsim.HybridSimProto.LeftClosedRightOpenTimeInterval request);

    public de.dlr.sumo.hybridsim.HybridSimProto.Boolean transferAgent(de.dlr.sumo.hybridsim.HybridSimProto.Agent request);

    public de.dlr.sumo.hybridsim.HybridSimProto.Trajectories receiveTrajectories(de.dlr.sumo.hybridsim.HybridSimProto.Empty request);

    public de.dlr.sumo.hybridsim.HybridSimProto.Agents retrieveAgents(de.dlr.sumo.hybridsim.HybridSimProto.Empty request);

    public de.dlr.sumo.hybridsim.HybridSimProto.Empty shutdown(de.dlr.sumo.hybridsim.HybridSimProto.Empty request);

    public de.dlr.sumo.hybridsim.HybridSimProto.Empty initScenario(de.dlr.sumo.hybridsim.HybridSimProto.Scenario request);
  }

  public static interface HybridSimulationFutureClient {

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Empty> simulatedTimeInerval(
        de.dlr.sumo.hybridsim.HybridSimProto.LeftClosedRightOpenTimeInterval request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Boolean> transferAgent(
        de.dlr.sumo.hybridsim.HybridSimProto.Agent request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Trajectories> receiveTrajectories(
        de.dlr.sumo.hybridsim.HybridSimProto.Empty request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Agents> retrieveAgents(
        de.dlr.sumo.hybridsim.HybridSimProto.Empty request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Empty> shutdown(
        de.dlr.sumo.hybridsim.HybridSimProto.Empty request);

    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Empty> initScenario(
        de.dlr.sumo.hybridsim.HybridSimProto.Scenario request);
  }

  public static class HybridSimulationStub extends io.grpc.stub.AbstractStub<HybridSimulationStub>
      implements HybridSimulation {
    private HybridSimulationStub(io.grpc.Channel channel) {
      super(channel);
    }

    private HybridSimulationStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected HybridSimulationStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new HybridSimulationStub(channel, callOptions);
    }

    @java.lang.Override
    public void simulatedTimeInerval(de.dlr.sumo.hybridsim.HybridSimProto.LeftClosedRightOpenTimeInterval request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_SIMULATED_TIME_INERVAL, getCallOptions()), request, responseObserver);
    }

    @java.lang.Override
    public void transferAgent(de.dlr.sumo.hybridsim.HybridSimProto.Agent request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Boolean> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_TRANSFER_AGENT, getCallOptions()), request, responseObserver);
    }

    @java.lang.Override
    public void receiveTrajectories(de.dlr.sumo.hybridsim.HybridSimProto.Empty request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Trajectories> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_RECEIVE_TRAJECTORIES, getCallOptions()), request, responseObserver);
    }

    @java.lang.Override
    public void retrieveAgents(de.dlr.sumo.hybridsim.HybridSimProto.Empty request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Agents> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_RETRIEVE_AGENTS, getCallOptions()), request, responseObserver);
    }

    @java.lang.Override
    public void shutdown(de.dlr.sumo.hybridsim.HybridSimProto.Empty request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_SHUTDOWN, getCallOptions()), request, responseObserver);
    }

    @java.lang.Override
    public void initScenario(de.dlr.sumo.hybridsim.HybridSimProto.Scenario request,
        io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(METHOD_INIT_SCENARIO, getCallOptions()), request, responseObserver);
    }
  }

  public static class HybridSimulationBlockingStub extends io.grpc.stub.AbstractStub<HybridSimulationBlockingStub>
      implements HybridSimulationBlockingClient {
    private HybridSimulationBlockingStub(io.grpc.Channel channel) {
      super(channel);
    }

    private HybridSimulationBlockingStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected HybridSimulationBlockingStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new HybridSimulationBlockingStub(channel, callOptions);
    }

    @java.lang.Override
    public de.dlr.sumo.hybridsim.HybridSimProto.Empty simulatedTimeInerval(de.dlr.sumo.hybridsim.HybridSimProto.LeftClosedRightOpenTimeInterval request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_SIMULATED_TIME_INERVAL, getCallOptions()), request);
    }

    @java.lang.Override
    public de.dlr.sumo.hybridsim.HybridSimProto.Boolean transferAgent(de.dlr.sumo.hybridsim.HybridSimProto.Agent request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_TRANSFER_AGENT, getCallOptions()), request);
    }

    @java.lang.Override
    public de.dlr.sumo.hybridsim.HybridSimProto.Trajectories receiveTrajectories(de.dlr.sumo.hybridsim.HybridSimProto.Empty request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_RECEIVE_TRAJECTORIES, getCallOptions()), request);
    }

    @java.lang.Override
    public de.dlr.sumo.hybridsim.HybridSimProto.Agents retrieveAgents(de.dlr.sumo.hybridsim.HybridSimProto.Empty request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_RETRIEVE_AGENTS, getCallOptions()), request);
    }

    @java.lang.Override
    public de.dlr.sumo.hybridsim.HybridSimProto.Empty shutdown(de.dlr.sumo.hybridsim.HybridSimProto.Empty request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_SHUTDOWN, getCallOptions()), request);
    }

    @java.lang.Override
    public de.dlr.sumo.hybridsim.HybridSimProto.Empty initScenario(de.dlr.sumo.hybridsim.HybridSimProto.Scenario request) {
      return blockingUnaryCall(
          getChannel().newCall(METHOD_INIT_SCENARIO, getCallOptions()), request);
    }
  }

  public static class HybridSimulationFutureStub extends io.grpc.stub.AbstractStub<HybridSimulationFutureStub>
      implements HybridSimulationFutureClient {
    private HybridSimulationFutureStub(io.grpc.Channel channel) {
      super(channel);
    }

    private HybridSimulationFutureStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected HybridSimulationFutureStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new HybridSimulationFutureStub(channel, callOptions);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Empty> simulatedTimeInerval(
        de.dlr.sumo.hybridsim.HybridSimProto.LeftClosedRightOpenTimeInterval request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_SIMULATED_TIME_INERVAL, getCallOptions()), request);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Boolean> transferAgent(
        de.dlr.sumo.hybridsim.HybridSimProto.Agent request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_TRANSFER_AGENT, getCallOptions()), request);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Trajectories> receiveTrajectories(
        de.dlr.sumo.hybridsim.HybridSimProto.Empty request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_RECEIVE_TRAJECTORIES, getCallOptions()), request);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Agents> retrieveAgents(
        de.dlr.sumo.hybridsim.HybridSimProto.Empty request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_RETRIEVE_AGENTS, getCallOptions()), request);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Empty> shutdown(
        de.dlr.sumo.hybridsim.HybridSimProto.Empty request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_SHUTDOWN, getCallOptions()), request);
    }

    @java.lang.Override
    public com.google.common.util.concurrent.ListenableFuture<de.dlr.sumo.hybridsim.HybridSimProto.Empty> initScenario(
        de.dlr.sumo.hybridsim.HybridSimProto.Scenario request) {
      return futureUnaryCall(
          getChannel().newCall(METHOD_INIT_SCENARIO, getCallOptions()), request);
    }
  }

  public static io.grpc.ServerServiceDefinition bindService(
      final HybridSimulation serviceImpl) {
    return io.grpc.ServerServiceDefinition.builder(SERVICE_NAME)
      .addMethod(
        METHOD_SIMULATED_TIME_INERVAL,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.hybridsim.HybridSimProto.LeftClosedRightOpenTimeInterval,
              de.dlr.sumo.hybridsim.HybridSimProto.Empty>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.hybridsim.HybridSimProto.LeftClosedRightOpenTimeInterval request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Empty> responseObserver) {
              serviceImpl.simulatedTimeInerval(request, responseObserver);
            }
          }))
      .addMethod(
        METHOD_TRANSFER_AGENT,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.hybridsim.HybridSimProto.Agent,
              de.dlr.sumo.hybridsim.HybridSimProto.Boolean>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.hybridsim.HybridSimProto.Agent request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Boolean> responseObserver) {
              serviceImpl.transferAgent(request, responseObserver);
            }
          }))
      .addMethod(
        METHOD_RECEIVE_TRAJECTORIES,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.hybridsim.HybridSimProto.Empty,
              de.dlr.sumo.hybridsim.HybridSimProto.Trajectories>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.hybridsim.HybridSimProto.Empty request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Trajectories> responseObserver) {
              serviceImpl.receiveTrajectories(request, responseObserver);
            }
          }))
      .addMethod(
        METHOD_RETRIEVE_AGENTS,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.hybridsim.HybridSimProto.Empty,
              de.dlr.sumo.hybridsim.HybridSimProto.Agents>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.hybridsim.HybridSimProto.Empty request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Agents> responseObserver) {
              serviceImpl.retrieveAgents(request, responseObserver);
            }
          }))
      .addMethod(
        METHOD_SHUTDOWN,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.hybridsim.HybridSimProto.Empty,
              de.dlr.sumo.hybridsim.HybridSimProto.Empty>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.hybridsim.HybridSimProto.Empty request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Empty> responseObserver) {
              serviceImpl.shutdown(request, responseObserver);
            }
          }))
      .addMethod(
        METHOD_INIT_SCENARIO,
        asyncUnaryCall(
          new io.grpc.stub.ServerCalls.UnaryMethod<
              de.dlr.sumo.hybridsim.HybridSimProto.Scenario,
              de.dlr.sumo.hybridsim.HybridSimProto.Empty>() {
            @java.lang.Override
            public void invoke(
                de.dlr.sumo.hybridsim.HybridSimProto.Scenario request,
                io.grpc.stub.StreamObserver<de.dlr.sumo.hybridsim.HybridSimProto.Empty> responseObserver) {
              serviceImpl.initScenario(request, responseObserver);
            }
          })).build();
  }
}
