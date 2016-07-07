// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "service/impala-internal-service.h"

#include "rpc/thrift-util.h"
#include "gen-cpp/ImpalaInternalService_types.h"
#include "service/impala-server.h"
#include "service/fragment-mgr.h"
#include "runtime/exec-env.h"
#include "kudu/rpc/rpc_context.h"

using kudu::rpc::RpcContext;

using kudu::rpc_test::ImpalaKRPCServiceIf;
using kudu::rpc_test::TransmitDataRequestPB;
using kudu::rpc_test::TransmitDataResponsePB;
using kudu::rpc_test::PublishFilterRequestPB;
using kudu::rpc_test::PublishFilterResponsePB;

using namespace kudu::rpc_test; // TODO - > decide if this a hack.

namespace impala {

// Helpers for TransmitData()
namespace {

impala::TUniqueId ToThriftID(const kudu::rpc_test::UniqueIdPB& pb) {
  impala::TUniqueId ret;
  ret.__set_lo(pb.lo());
  ret.__set_hi(pb.hi());
  return ret;
}

}

template <typename In, typename Out>
void ToTList(const google::protobuf::RepeatedField<In>& in, vector<Out>* out) {
  out->resize(in.size());
  for (int i = 0; i < in.size(); ++i) {
    (*out)[i] = in.Get(i);
  }
}

TRowBatch ToTRowBatch(const kudu::rpc_test::RowBatchPB& pb) {
  TRowBatch ret;
  ret.__set_num_rows(pb.num_rows());
  ToTList(pb.row_tuples(), &ret.row_tuples);
  ToTList(pb.tuple_offsets(), &ret.tuple_offsets);
  ret.__set_tuple_data(pb.tuple_data());
  ret.__set_uncompressed_size(pb.uncompressed_size());
  ret.__set_compression_type(static_cast<THdfsCompression::type>(pb.compression_type()));
  return ret;
}


void ImpalaKRPCServiceImpl::TransmitData(const TransmitDataRequestPB* request,
    TransmitDataResponsePB* response, RpcContext* context) {
  TTransmitDataParams tparams;
  tparams.__set_dest_fragment_instance_id(
      ToThriftID(request->dest_fragment_instance_id()));
  tparams.__set_sender_id(request->sender_id());
  tparams.__set_dest_node_id(request->dest_node_id());
  tparams.__set_row_batch(ToTRowBatch(request->row_batch()));
  tparams.__set_eos(request->eos());
  TTransmitDataResult result;
  ExecEnv::GetInstance()->impala_server()->TransmitData(result, tparams);
  context->RespondSuccess();
}

void ImpalaKRPCServiceImpl::PublishFilter(const PublishFilterRequestPB* request,
    PublishFilterResponsePB* response, RpcContext* context) {
  ExecEnv::GetInstance()->fragment_mgr()->PublishFilter(request, response);
  context->RespondSuccess();
}

void ImpalaKRPCServiceImpl::UpdateFilter(const UpdateFilterRequestPB* request,
    UpdateFilterResponsePB* response, RpcContext* context) {
  ExecEnv::GetInstance()->impala_server()->UpdateFilter(request, response);
  context->RespondSuccess();
}

void ImpalaKRPCServiceImpl::ExecPlanFragment(const ExecPlanFragmentRequestPB* request,
    ExecPlanFragmentResponsePB* response, RpcContext* context) {
  TExecPlanFragmentParams thrift_request;
  uint32_t len = request->thrift_struct().size();
  Status status = DeserializeThriftMsg(
      reinterpret_cast<const uint8_t*>(request->thrift_struct().data()),
      &len, true, &thrift_request);
  TExecPlanFragmentResult return_val;
  ExecEnv::GetInstance()->fragment_mgr()->ExecPlanFragment(thrift_request)
      .SetTStatus(&return_val);
  SerializeThriftToProtoWrapper(&return_val, true, response);
  context->RespondSuccess();
}

void ImpalaKRPCServiceImpl::ReportExecStatus(const ReportExecStatusRequestPB* request,
    ReportExecStatusResponsePB* response, RpcContext* context) {
  TReportExecStatusParams thrift_request;
  uint32_t len = request->thrift_struct().size();
  Status status = DeserializeThriftMsg(
      reinterpret_cast<const uint8_t*>(request->thrift_struct().data()),
      &len, true, &thrift_request);
  TReportExecStatusResult return_val;
  ExecEnv::GetInstance()->impala_server()->ReportExecStatus(return_val, thrift_request);
  SerializeThriftToProtoWrapper(&return_val, true, response);
  context->RespondSuccess();
}

void ImpalaKRPCServiceImpl::CancelPlanFragment(const CancelPlanFragmentRequestPB* request,
    CancelPlanFragmentResponsePB* response, RpcContext* context) {
  TCancelPlanFragmentParams thrift_request;
  uint32_t len = request->thrift_struct().size();
  Status status = DeserializeThriftMsg(
      reinterpret_cast<const uint8_t*>(request->thrift_struct().data()),
      &len, true, &thrift_request);
  TCancelPlanFragmentResult return_val;
  ExecEnv::GetInstance()->fragment_mgr()->CancelPlanFragment(return_val, thrift_request);
  SerializeThriftToProtoWrapper(&return_val, true, response);
  context->RespondSuccess();
}

}
