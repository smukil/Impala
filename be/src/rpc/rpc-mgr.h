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

#ifndef IMPALA_RPC_RPC_MGR_H
#define IMPALA_RPC_RPC_MGR_H

#include "kudu/util/net/sockaddr.h"
#include "kudu/util/net/net_util.h"
#include "kudu/rpc/rpc_context.h"
#include "kudu/rpc/messenger.h"

#include "exec/kudu-util.h"

#include "common/status.h"

namespace kudu {
namespace rpc {
class ServiceIf;
class ResultTracker;
}
}

namespace impala {

class RpcMgr {
 public:
  Status Start(int32_t port);

  template<typename T>
  Status RegisterService(T** service = nullptr) {
    T* service_ptr = new T(messenger_->metric_entity(), tracker_);
    if (service != nullptr) *service = service_ptr;
    return RegisterServiceImpl(service_ptr->service_name(), service_ptr);
  }

  template <typename P>
  Status GetProxy(const TNetworkAddress& address, std::unique_ptr<P>* proxy) {
    DCHECK(proxy != nullptr);
    std::vector<kudu::Sockaddr> addresses;
    KUDU_RETURN_IF_ERROR(
        kudu::HostPort(address.hostname, address.port).ResolveAddresses(&addresses),
        "Couldn't resolve addresses");
    proxy->reset(new P(messenger_, addresses[0]));
    return Status::OK();
  }

 private:
  Status RegisterServiceImpl(const std::string& name, kudu::rpc::ServiceIf* service);

  kudu::MetricRegistry registry_;

  std::shared_ptr<kudu::rpc::Messenger> messenger_;

  const scoped_refptr<kudu::rpc::ResultTracker> tracker_;

};

}

#endif
