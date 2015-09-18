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

#ifndef SYSTEM_NATIVEPOWER_INCLUDE_NATIVEPOWER_WAKE_LOCK_H_
#define SYSTEM_NATIVEPOWER_INCLUDE_NATIVEPOWER_WAKE_LOCK_H_

#include <memory>
#include <string>

#include <base/macros.h>
#include <powermanager/IPowerManager.h>
#include <utils/StrongPointer.h>

namespace android {

class IBinder;

// RAII-style object that prevents the system from suspending.
//
// android::BinderWrapper must be initialized before constructing this class.
class WakeLock {
 public:
  // Creates and returns a wake lock identified by |tag| and |package|.
  // An empty pointer is returned on failure (e.g. due to issues communicating
  // with the power manager).
  static std::unique_ptr<WakeLock> Create(const std::string& tag,
                                          const std::string& package);

  ~WakeLock();

 private:
  // Called by Create().
  WakeLock(const std::string& tag, const std::string& package);

  // Initializes the object and returns true on success. Called by Create().
  bool Init();

  // Called by |death_recipient_| in response to |power_manager_| dying.
  void OnPowerManagerDied();

  std::string tag_;
  std::string package_;

  // Interface for communicating with the power manager.
  sp<IPowerManager> power_manager_;

  // Locally-created binder passed to the power manager.
  sp<IBinder> lock_binder_;

  DISALLOW_COPY_AND_ASSIGN(WakeLock);
};

}  // namespace android

#endif  // SYSTEM_NATIVEPOWER_INCLUDE_NATIVEPOWER_WAKE_LOCK_H_
