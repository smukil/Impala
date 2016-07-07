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

#ifndef IMPALA_SERVICE_IMPALA_INTERNAL_SERVICE_H
#define IMPALA_SERVICE_IMPALA_INTERNAL_SERVICE_H

#include "service/fragment-mgr.h"
#include "service/prototest.pb.h"
#include "service/prototest.service.h"

namespace impala {

/// Proxies RPC requests onto their implementing objects for the
/// ImpalaInternalService service.
class ImpalaKRPCServiceImpl : public kudu::rpc_test::ImpalaKRPCServiceIf {
 public:
  ImpalaKRPCServiceImpl(const scoped_refptr<kudu::MetricEntity>& entity,
      const scoped_refptr<kudu::rpc::ResultTracker> tracker) : ImpalaKRPCServiceIf(entity, tracker) {
  }

  virtual void TransmitData(const kudu::rpc_test::TransmitDataRequestPB* request,
      kudu::rpc_test::TransmitDataResponsePB* response, kudu::rpc::RpcContext* context);

  virtual void PublishFilter(const kudu::rpc_test::PublishFilterRequestPB* request,
      kudu::rpc_test::PublishFilterResponsePB* response, kudu::rpc::RpcContext* context);

  virtual void UpdateFilter(const kudu::rpc_test::UpdateFilterRequestPB* request,
      kudu::rpc_test::UpdateFilterResponsePB* response, kudu::rpc::RpcContext* context);

  virtual void ExecPlanFragment(const kudu::rpc_test::ExecPlanFragmentRequestPB* request,
      kudu::rpc_test::ExecPlanFragmentResponsePB* response, kudu::rpc::RpcContext* context);

  virtual void ReportExecStatus(const kudu::rpc_test::ReportExecStatusRequestPB* request,
      kudu::rpc_test::ReportExecStatusResponsePB* response, kudu::rpc::RpcContext* context);

  virtual void CancelPlanFragment(const kudu::rpc_test::CancelPlanFragmentRequestPB* request,
      kudu::rpc_test::CancelPlanFragmentResponsePB* response, kudu::rpc::RpcContext* context);
};


}

#endif
