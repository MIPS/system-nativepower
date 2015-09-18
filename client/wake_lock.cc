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

#include <nativepower/wake_lock.h>

#include <base/bind.h>
#include <base/logging.h>
#include <binder/IBinder.h>
#include <binderwrapper/binder_wrapper.h>
#include <nativepower/constants.h>
#include <powermanager/PowerManager.h>

namespace android {

// static
std::unique_ptr<WakeLock> WakeLock::Create(const std::string& tag,
                                           const std::string& package) {
  std::unique_ptr<WakeLock> lock(new WakeLock(tag, package));
  if (!lock->Init())
    lock.reset();
  return lock;
}

WakeLock::~WakeLock() {
  if (power_manager_.get()) {
    BinderWrapper::Get()->UnregisterForDeathNotifications(
        IInterface::asBinder(power_manager_));
    status_t status =
        power_manager_->releaseWakeLock(lock_binder_, 0 /* flags */);
    if (status != OK) {
      LOG(ERROR) << "Wake lock release request for \"" << tag_ << "\" failed "
                 << "with status " << status;
    }
  }
}

WakeLock::WakeLock(const std::string& tag, const std::string& package)
    : tag_(tag),
      package_(package) {}

bool WakeLock::Init() {
  sp<IBinder> power_manager_binder =
      BinderWrapper::Get()->GetService(kPowerManagerServiceName);
  if (!power_manager_binder.get()) {
    LOG(ERROR) << "Didn't get " << kPowerManagerServiceName << " service";
    return false;
  }

  power_manager_ = interface_cast<IPowerManager>(power_manager_binder);
  BinderWrapper::Get()->RegisterForDeathNotifications(
      power_manager_binder,
      base::Bind(&WakeLock::OnPowerManagerDied, base::Unretained(this)));

  lock_binder_ = BinderWrapper::Get()->CreateLocalBinder();
  status_t status = power_manager_->acquireWakeLock(
      POWERMANAGER_PARTIAL_WAKE_LOCK,
      lock_binder_, String16(tag_.c_str()), String16(package_.c_str()));
  if (status != OK) {
    LOG(ERROR) << "Wake lock acquire request for tag \"" << tag_ << "\" failed "
               << "with status " << status;
    power_manager_.clear();
    return false;
  }

  return true;
}

void WakeLock::OnPowerManagerDied() {
  LOG(WARNING) << "Power manager died; lost wake lock for \"" << tag_ << "\"";
  power_manager_.clear();
}

}  // namespace android
