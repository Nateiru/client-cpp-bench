// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: greptime/v1/database.proto
// Original file comments:
// Copyright 2023 Greptime Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#ifndef GRPC_greptime_2fv1_2fdatabase_2eproto__INCLUDED
#define GRPC_greptime_2fv1_2fdatabase_2eproto__INCLUDED

#include "greptime/v1/database.pb.h"

#include <functional>
#include <grpcpp/generic/async_generic_service.h>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/client_context.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/rpc_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/support/stub_options.h>
#include <grpcpp/support/sync_stream.h>

namespace greptime {
namespace v1 {

class GreptimeDatabase final {
 public:
  static constexpr char const* service_full_name() {
    return "greptime.v1.GreptimeDatabase";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status Handle(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest& request, ::greptime::v1::GreptimeResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::greptime::v1::GreptimeResponse>> AsyncHandle(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::greptime::v1::GreptimeResponse>>(AsyncHandleRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::greptime::v1::GreptimeResponse>> PrepareAsyncHandle(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::greptime::v1::GreptimeResponse>>(PrepareAsyncHandleRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientWriterInterface< ::greptime::v1::GreptimeRequest>> HandleRequests(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response) {
      return std::unique_ptr< ::grpc::ClientWriterInterface< ::greptime::v1::GreptimeRequest>>(HandleRequestsRaw(context, response));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::greptime::v1::GreptimeRequest>> AsyncHandleRequests(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::greptime::v1::GreptimeRequest>>(AsyncHandleRequestsRaw(context, response, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::greptime::v1::GreptimeRequest>> PrepareAsyncHandleRequests(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::greptime::v1::GreptimeRequest>>(PrepareAsyncHandleRequestsRaw(context, response, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      virtual void Handle(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest* request, ::greptime::v1::GreptimeResponse* response, std::function<void(::grpc::Status)>) = 0;
      virtual void Handle(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest* request, ::greptime::v1::GreptimeResponse* response, ::grpc::ClientUnaryReactor* reactor) = 0;
      virtual void HandleRequests(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response, ::grpc::ClientWriteReactor< ::greptime::v1::GreptimeRequest>* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::greptime::v1::GreptimeResponse>* AsyncHandleRaw(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::greptime::v1::GreptimeResponse>* PrepareAsyncHandleRaw(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientWriterInterface< ::greptime::v1::GreptimeRequest>* HandleRequestsRaw(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response) = 0;
    virtual ::grpc::ClientAsyncWriterInterface< ::greptime::v1::GreptimeRequest>* AsyncHandleRequestsRaw(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncWriterInterface< ::greptime::v1::GreptimeRequest>* PrepareAsyncHandleRequestsRaw(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    ::grpc::Status Handle(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest& request, ::greptime::v1::GreptimeResponse* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::greptime::v1::GreptimeResponse>> AsyncHandle(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::greptime::v1::GreptimeResponse>>(AsyncHandleRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::greptime::v1::GreptimeResponse>> PrepareAsyncHandle(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::greptime::v1::GreptimeResponse>>(PrepareAsyncHandleRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientWriter< ::greptime::v1::GreptimeRequest>> HandleRequests(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response) {
      return std::unique_ptr< ::grpc::ClientWriter< ::greptime::v1::GreptimeRequest>>(HandleRequestsRaw(context, response));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriter< ::greptime::v1::GreptimeRequest>> AsyncHandleRequests(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncWriter< ::greptime::v1::GreptimeRequest>>(AsyncHandleRequestsRaw(context, response, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriter< ::greptime::v1::GreptimeRequest>> PrepareAsyncHandleRequests(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncWriter< ::greptime::v1::GreptimeRequest>>(PrepareAsyncHandleRequestsRaw(context, response, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void Handle(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest* request, ::greptime::v1::GreptimeResponse* response, std::function<void(::grpc::Status)>) override;
      void Handle(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest* request, ::greptime::v1::GreptimeResponse* response, ::grpc::ClientUnaryReactor* reactor) override;
      void HandleRequests(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response, ::grpc::ClientWriteReactor< ::greptime::v1::GreptimeRequest>* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::greptime::v1::GreptimeResponse>* AsyncHandleRaw(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::greptime::v1::GreptimeResponse>* PrepareAsyncHandleRaw(::grpc::ClientContext* context, const ::greptime::v1::GreptimeRequest& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientWriter< ::greptime::v1::GreptimeRequest>* HandleRequestsRaw(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response) override;
    ::grpc::ClientAsyncWriter< ::greptime::v1::GreptimeRequest>* AsyncHandleRequestsRaw(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncWriter< ::greptime::v1::GreptimeRequest>* PrepareAsyncHandleRequestsRaw(::grpc::ClientContext* context, ::greptime::v1::GreptimeResponse* response, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_Handle_;
    const ::grpc::internal::RpcMethod rpcmethod_HandleRequests_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status Handle(::grpc::ServerContext* context, const ::greptime::v1::GreptimeRequest* request, ::greptime::v1::GreptimeResponse* response);
    virtual ::grpc::Status HandleRequests(::grpc::ServerContext* context, ::grpc::ServerReader< ::greptime::v1::GreptimeRequest>* reader, ::greptime::v1::GreptimeResponse* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_Handle : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_Handle() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_Handle() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Handle(::grpc::ServerContext* /*context*/, const ::greptime::v1::GreptimeRequest* /*request*/, ::greptime::v1::GreptimeResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestHandle(::grpc::ServerContext* context, ::greptime::v1::GreptimeRequest* request, ::grpc::ServerAsyncResponseWriter< ::greptime::v1::GreptimeResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_HandleRequests : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_HandleRequests() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_HandleRequests() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status HandleRequests(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::greptime::v1::GreptimeRequest>* /*reader*/, ::greptime::v1::GreptimeResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestHandleRequests(::grpc::ServerContext* context, ::grpc::ServerAsyncReader< ::greptime::v1::GreptimeResponse, ::greptime::v1::GreptimeRequest>* reader, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncClientStreaming(1, context, reader, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_Handle<WithAsyncMethod_HandleRequests<Service > > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_Handle : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_Handle() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::greptime::v1::GreptimeRequest, ::greptime::v1::GreptimeResponse>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::greptime::v1::GreptimeRequest* request, ::greptime::v1::GreptimeResponse* response) { return this->Handle(context, request, response); }));}
    void SetMessageAllocatorFor_Handle(
        ::grpc::MessageAllocator< ::greptime::v1::GreptimeRequest, ::greptime::v1::GreptimeResponse>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(0);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::greptime::v1::GreptimeRequest, ::greptime::v1::GreptimeResponse>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_Handle() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Handle(::grpc::ServerContext* /*context*/, const ::greptime::v1::GreptimeRequest* /*request*/, ::greptime::v1::GreptimeResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Handle(
      ::grpc::CallbackServerContext* /*context*/, const ::greptime::v1::GreptimeRequest* /*request*/, ::greptime::v1::GreptimeResponse* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithCallbackMethod_HandleRequests : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_HandleRequests() {
      ::grpc::Service::MarkMethodCallback(1,
          new ::grpc::internal::CallbackClientStreamingHandler< ::greptime::v1::GreptimeRequest, ::greptime::v1::GreptimeResponse>(
            [this](
                   ::grpc::CallbackServerContext* context, ::greptime::v1::GreptimeResponse* response) { return this->HandleRequests(context, response); }));
    }
    ~WithCallbackMethod_HandleRequests() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status HandleRequests(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::greptime::v1::GreptimeRequest>* /*reader*/, ::greptime::v1::GreptimeResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerReadReactor< ::greptime::v1::GreptimeRequest>* HandleRequests(
      ::grpc::CallbackServerContext* /*context*/, ::greptime::v1::GreptimeResponse* /*response*/)  { return nullptr; }
  };
  typedef WithCallbackMethod_Handle<WithCallbackMethod_HandleRequests<Service > > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_Handle : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_Handle() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_Handle() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Handle(::grpc::ServerContext* /*context*/, const ::greptime::v1::GreptimeRequest* /*request*/, ::greptime::v1::GreptimeResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_HandleRequests : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_HandleRequests() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_HandleRequests() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status HandleRequests(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::greptime::v1::GreptimeRequest>* /*reader*/, ::greptime::v1::GreptimeResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_Handle : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_Handle() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_Handle() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Handle(::grpc::ServerContext* /*context*/, const ::greptime::v1::GreptimeRequest* /*request*/, ::greptime::v1::GreptimeResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestHandle(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawMethod_HandleRequests : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_HandleRequests() {
      ::grpc::Service::MarkMethodRaw(1);
    }
    ~WithRawMethod_HandleRequests() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status HandleRequests(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::greptime::v1::GreptimeRequest>* /*reader*/, ::greptime::v1::GreptimeResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestHandleRequests(::grpc::ServerContext* context, ::grpc::ServerAsyncReader< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* reader, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncClientStreaming(1, context, reader, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_Handle : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_Handle() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->Handle(context, request, response); }));
    }
    ~WithRawCallbackMethod_Handle() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Handle(::grpc::ServerContext* /*context*/, const ::greptime::v1::GreptimeRequest* /*request*/, ::greptime::v1::GreptimeResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Handle(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_HandleRequests : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_HandleRequests() {
      ::grpc::Service::MarkMethodRawCallback(1,
          new ::grpc::internal::CallbackClientStreamingHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, ::grpc::ByteBuffer* response) { return this->HandleRequests(context, response); }));
    }
    ~WithRawCallbackMethod_HandleRequests() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status HandleRequests(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::greptime::v1::GreptimeRequest>* /*reader*/, ::greptime::v1::GreptimeResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerReadReactor< ::grpc::ByteBuffer>* HandleRequests(
      ::grpc::CallbackServerContext* /*context*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_Handle : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_Handle() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler<
          ::greptime::v1::GreptimeRequest, ::greptime::v1::GreptimeResponse>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::greptime::v1::GreptimeRequest, ::greptime::v1::GreptimeResponse>* streamer) {
                       return this->StreamedHandle(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_Handle() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status Handle(::grpc::ServerContext* /*context*/, const ::greptime::v1::GreptimeRequest* /*request*/, ::greptime::v1::GreptimeResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedHandle(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::greptime::v1::GreptimeRequest,::greptime::v1::GreptimeResponse>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_Handle<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_Handle<Service > StreamedService;
};

}  // namespace v1
}  // namespace greptime


#endif  // GRPC_greptime_2fv1_2fdatabase_2eproto__INCLUDED
