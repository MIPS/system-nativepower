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

#include <map>
#include <string>
#include <vector>

#include <base/macros.h>
#include <nativepower/BnPowerManager.h>

namespace android {

// Stub implementation of BnPowerManager for use in tests.
class PowerManagerStub : public BnPowerManager {
 public:
  PowerManagerStub();
  ~PowerManagerStub() override;

  // Constructs a string that can be compared with one returned by
  // GetLockString().
  static std::string ConstructLockString(const std::string& tag,
                                         const std::string& package,
                                         int uid);

  // Constructs a string that can be compared with one returned by
  // GetSuspendRequestString().
  static std::string ConstructSuspendRequestString(int64_t event_time_ms,
                                                   int reason,
                                                   int flags);

  size_t num_locks() const { return locks_.size(); }
  size_t num_suspend_requests() const { return suspend_requests_.size(); }
  const std::vector<std::string>& reboot_reasons() const {
    return reboot_reasons_;
  }
  const std::vector<std::string>& shutdown_reasons() const {
    return shutdown_reasons_;
  }

  // Returns a string describing the lock registered for |binder|, or an empty
  // string if no lock is present.
  std::string GetLockString(const sp<IBinder>& binder) const;

  // Returns a string describing position |index| in |suspend_requests_|.
  std::string GetSuspendRequestString(size_t index) const;

  // BnPowerManager:
  status_t acquireWakeLock(int flags,
                           const sp<IBinder>& lock,
                           const String16& tag,
                           const String16& packageName,
                           bool isOneWay=false) override;
  status_t acquireWakeLockWithUid(int flags,
                                  const sp<IBinder>& lock,
                                  const String16& tag,
                                  const String16& packageName,
                                  int uid,
                                  bool isOneWay=false) override;
  status_t releaseWakeLock(const sp<IBinder>& lock,
                           int flags,
                           bool isOneWay=false) override;
  status_t updateWakeLockUids(const sp<IBinder>& lock,
                              int len,
                              const int* uids,
                              bool isOneWay=false) override;
  status_t powerHint(int hintId, int data) override;
  status_t goToSleep(int64_t event_time_ms, int reason, int flags) override;
  status_t reboot(bool confirm, const String16& reason, bool wait) override;
  status_t shutdown(bool confirm, const String16& reason, bool wait) override;
  status_t crash(const String16& message) override;

 private:
  // Contains information passed to acquireWakeLock() or
  // acquireWakeLockWithUid().
  struct LockInfo {
    LockInfo();
    LockInfo(const LockInfo& info);
    LockInfo(const std::string& tag,
             const std::string& package,
             int uid);

    std::string tag;
    std::string package;

    // -1 if acquireWakeLock() was used.
    int uid;
  };

  // Details about a request passed to goToSleep().
  struct SuspendRequest {
    SuspendRequest(int64 uptime_ms, int reason, int flags);

    int64 event_time_ms;
    int reason;
    int flags;
  };

  using LockInfoMap = std::map<sp<IBinder>, LockInfo>;
  LockInfoMap locks_;

  // Information about calls to goToSleep(), in the order they were made.
  using SuspendRequests = std::vector<SuspendRequest>;
  SuspendRequests suspend_requests_;

  // Reasons passed to reboot() and shutdown(), in the order in which they were
  // received.
  std::vector<std::string> reboot_reasons_;
  std::vector<std::string> shutdown_reasons_;

  DISALLOW_COPY_AND_ASSIGN(PowerManagerStub);
};

}  // namespace android
