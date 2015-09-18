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

#include <base/macros.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binderwrapper/binder_test_base.h>
#include <binderwrapper/stub_binder_wrapper.h>
#include <nativepower/constants.h>
#include <powermanager/PowerManager.h>

#include "power_manager.h"
#include "wake_lock_manager.h"
#include "wake_lock_manager_stub.h"

namespace android {

class PowerManagerTest : public BinderTestBase {
 public:
  PowerManagerTest()
      : power_manager_(new PowerManager()),
        wake_lock_manager_(new WakeLockManagerStub()) {
    power_manager_->set_wake_lock_manager_for_testing(
        std::unique_ptr<WakeLockManagerInterface>(wake_lock_manager_));
    CHECK(power_manager_->Init());
  }
  ~PowerManagerTest() override = default;

 protected:
  sp<PowerManager> power_manager_;
  WakeLockManagerStub* wake_lock_manager_;  // Owned by |power_manager_|.

 private:
  DISALLOW_COPY_AND_ASSIGN(PowerManagerTest);
};

TEST_F(PowerManagerTest, RegisterService) {
  EXPECT_EQ(power_manager_,
            binder_wrapper()->GetRegisteredService(kPowerManagerServiceName));
}

TEST_F(PowerManagerTest, AcquireAndReleaseWakeLock) {
  sp<BBinder> binder = binder_wrapper()->CreateLocalBinder();
  EXPECT_EQ(OK,
            interface_cast<IPowerManager>(power_manager_)->acquireWakeLock(
                0, binder, String16(), String16()));
  ASSERT_EQ(1u, wake_lock_manager_->request_binders().size());
  EXPECT_TRUE(wake_lock_manager_->has_request_binder(binder));

  EXPECT_EQ(OK,
            interface_cast<IPowerManager>(power_manager_)->releaseWakeLock(
                binder, 0));
  EXPECT_TRUE(wake_lock_manager_->request_binders().empty());
}

}  // namespace android
