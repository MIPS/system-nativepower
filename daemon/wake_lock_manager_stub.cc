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

#include "wake_lock_manager_stub.h"

#include <binder/IBinder.h>

namespace android {

WakeLockManagerStub::WakeLockManagerStub() = default;

WakeLockManagerStub::~WakeLockManagerStub() = default;

bool WakeLockManagerStub::AddRequest(sp<IBinder> client_binder,
                                     const std::string& tag,
                                     const std::string& package,
                                     int uid) {
  request_binders_.insert(client_binder);
  return true;
}

bool WakeLockManagerStub::RemoveRequest(sp<IBinder> client_binder) {
  if (!request_binders_.count(client_binder))
    return false;

  request_binders_.erase(client_binder);
  return true;
}

}  // namespace android
