/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SYSTEM_NATIVEPOWER_DAEMON_WAKE_LOCK_MANAGER_STUB_H_
#define SYSTEM_NATIVEPOWER_DAEMON_WAKE_LOCK_MANAGER_STUB_H_

#include <set>
#include <string>

#include <base/macros.h>
#include <utils/StrongPointer.h>

#include "wake_lock_manager.h"

namespace android {

class IBinder;

// Stub implementation used by tests.
class WakeLockManagerStub : public WakeLockManagerInterface {
 public:
  using BinderSet = std::set<sp<IBinder>>;

  WakeLockManagerStub();
  ~WakeLockManagerStub() override;

  const BinderSet& request_binders() const { return request_binders_; }
  bool has_request_binder(const sp<IBinder>& binder) const {
    return request_binders_.count(binder);
  }

  // WakeLockManagerInterface:
  bool AddRequest(sp<IBinder> client_binder,
                  const std::string& tag,
                  const std::string& package,
                  int uid) override;
  bool RemoveRequest(sp<IBinder> client_binder) override;

 private:
  // Binders passed to AddRequest() for currently-active requests.
  BinderSet request_binders_;

  DISALLOW_COPY_AND_ASSIGN(WakeLockManagerStub);
};

}  // namespace android

#endif  // SYSTEM_NATIVEPOWER_DAEMON_WAKE_LOCK_MANAGER_STUB_H_
