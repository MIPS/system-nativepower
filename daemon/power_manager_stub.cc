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

#include <base/logging.h>
#include <base/strings/stringprintf.h>
#include <nativepower/power_manager_stub.h>
#include <utils/String8.h>

namespace android {

PowerManagerStub::LockInfo::LockInfo() : uid(-1) {}

PowerManagerStub::LockInfo::LockInfo(const LockInfo& info) = default;

PowerManagerStub::LockInfo::LockInfo(const std::string& tag,
                                     const std::string& package,
                                     int uid)
    : tag(tag),
      package(package),
      uid(uid) {}

// static
std::string PowerManagerStub::ConstructLockString(const std::string& tag,
                                                  const std::string& package,
                                                  int uid) {
  return base::StringPrintf("%s,%s,%d", tag.c_str(), package.c_str(), uid);
}

PowerManagerStub::PowerManagerStub() = default;

PowerManagerStub::~PowerManagerStub() = default;

std::string PowerManagerStub::GetLockString(const sp<IBinder>& binder) const {
  const auto it = locks_.find(binder);
  if (it == locks_.end())
    return std::string();

  const LockInfo& info = it->second;
  return ConstructLockString(info.tag, info.package, info.uid);
}

status_t PowerManagerStub::acquireWakeLock(int flags,
                                           const sp<IBinder>& lock,
                                           const String16& tag,
                                           const String16& packageName,
                                           bool isOneWay) {
  CHECK(!locks_.count(lock)) << "Got acquireWakeLock() request for "
                             << "already-registered binder " << lock.get();
  locks_[lock] =
      LockInfo(String8(tag).string(), String8(packageName).string(), -1);
  return OK;
}

status_t PowerManagerStub::acquireWakeLockWithUid(int flags,
                                                  const sp<IBinder>& lock,
                                                  const String16& tag,
                                                  const String16& packageName,
                                                  int uid,
                                                  bool isOneWay) {
  CHECK(!locks_.count(lock)) << "Got acquireWakeLockWithUid() request for "
                             << "already-registered binder " << lock.get();
  locks_[lock] =
      LockInfo(String8(tag).string(), String8(packageName).string(), uid);
  return OK;
}

status_t PowerManagerStub::releaseWakeLock(const sp<IBinder>& lock,
                                           int flags,
                                           bool isOneWay) {
  CHECK(locks_.count(lock)) << "Got releaseWakeLock() request for unregistered "
                            << "binder " << lock.get();
  locks_.erase(lock);
  return OK;
}

status_t PowerManagerStub::updateWakeLockUids(const sp<IBinder>& lock,
                                              int len,
                                              const int* uids,
                                              bool isOneWay) {
  return OK;
}

status_t PowerManagerStub::powerHint(int hintId, int data) {
  return OK;
}

}  // namespace android
