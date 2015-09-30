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

#include "power_manager.h"

#include <base/logging.h>
#include <binderwrapper/binder_wrapper.h>
#include <nativepower/constants.h>
#include <powermanager/IPowerManager.h>
#include <utils/Errors.h>
#include <utils/String8.h>

namespace android {

PowerManager::PowerManager() = default;

PowerManager::~PowerManager() = default;

bool PowerManager::Init() {
  if (!wake_lock_manager_) {
    wake_lock_manager_.reset(new WakeLockManager());
    if (!static_cast<WakeLockManager*>(wake_lock_manager_.get())->Init())
      return false;
  }

  LOG(INFO) << "Registering with service manager as "
            << kPowerManagerServiceName;
  return BinderWrapper::Get()->RegisterService(kPowerManagerServiceName, this);
}

status_t PowerManager::acquireWakeLock(int flags,
                                       const sp<IBinder>& lock,
                                       const String16& tag,
                                       const String16& packageName,
                                       bool isOneWay) {
  return AddWakeLockRequest(lock, tag, packageName, -1) ? OK : UNKNOWN_ERROR;
}

status_t PowerManager::acquireWakeLockWithUid(int flags,
                                              const sp<IBinder>& lock,
                                              const String16& tag,
                                              const String16& packageName,
                                              int uid,
                                              bool isOneWay) {
  return AddWakeLockRequest(lock, tag, packageName, uid) ? OK : UNKNOWN_ERROR;
}

status_t PowerManager::releaseWakeLock(const sp<IBinder>& lock,
                                       int flags,
                                       bool isOneWay) {
  return wake_lock_manager_->RemoveRequest(lock) ? OK : UNKNOWN_ERROR;
}

status_t PowerManager::updateWakeLockUids(const sp<IBinder>& lock,
                                          int len,
                                          const int* uids,
                                          bool isOneWay) {
  NOTIMPLEMENTED() << "updateWakeLockUids: lock=" << lock.get()
                   << " len=" << len;
  return OK;
}

status_t PowerManager::powerHint(int hintId, int data) {
  NOTIMPLEMENTED() << "powerHint: hintId=" << hintId << " data=" << data;
  return OK;
}

status_t PowerManager::goToSleep(int64_t event_time_ms, int reason, int flags) {
  return OK;
}

status_t PowerManager::reboot(bool confirm, const String16& reason, bool wait) {
  return OK;
}

status_t PowerManager::shutdown(bool confirm,
                                const String16& reason,
                                bool wait) {
  return OK;
}

status_t PowerManager::crash(const String16& message) {
  return OK;
}

bool PowerManager::AddWakeLockRequest(const sp<IBinder>& lock,
                                      const String16& tag,
                                      const String16& packageName,
                                      int uid) {
  return wake_lock_manager_->AddRequest(lock, String8(tag).string(),
                                        String8(packageName).string(), uid);
}

}  // namespace android
