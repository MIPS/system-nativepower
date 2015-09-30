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
#include <cutils/android_reboot.h>
#include <nativepower/constants.h>
#include <powermanager/PowerManager.h>

#include "power_manager.h"
#include "system_property_setter_stub.h"
#include "wake_lock_manager_stub.h"

namespace android {

class PowerManagerTest : public BinderTestBase {
 public:
  PowerManagerTest()
      : power_manager_(new PowerManager()),
        interface_(interface_cast<IPowerManager>(power_manager_)),
        property_setter_(new SystemPropertySetterStub()),
        wake_lock_manager_(new WakeLockManagerStub()) {
    power_manager_->set_property_setter_for_testing(
        std::unique_ptr<SystemPropertySetterInterface>(property_setter_));
    power_manager_->set_wake_lock_manager_for_testing(
        std::unique_ptr<WakeLockManagerInterface>(wake_lock_manager_));
    CHECK(power_manager_->Init());
  }
  ~PowerManagerTest() override = default;

 protected:
  sp<PowerManager> power_manager_;
  sp<IPowerManager> interface_;
  SystemPropertySetterStub* property_setter_;  // Owned by |power_manager_|.
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
  EXPECT_EQ(OK, interface_->acquireWakeLock(0, binder, String16(), String16()));
  ASSERT_EQ(1u, wake_lock_manager_->request_binders().size());
  EXPECT_TRUE(wake_lock_manager_->has_request_binder(binder));

  EXPECT_EQ(OK, interface_->releaseWakeLock(binder, 0));
  EXPECT_TRUE(wake_lock_manager_->request_binders().empty());
}

TEST_F(PowerManagerTest, Reboot) {
  EXPECT_EQ(OK, interface_->reboot(false, String16(), false));
  EXPECT_EQ(PowerManager::kRebootPrefix,
            property_setter_->GetProperty(ANDROID_RB_PROPERTY));

  EXPECT_EQ(OK, interface_->reboot(false, String16(kRebootReasonRecovery),
                                   false));
  EXPECT_EQ(std::string(PowerManager::kRebootPrefix) + kRebootReasonRecovery,
            property_setter_->GetProperty(ANDROID_RB_PROPERTY));

  // Invalid values should be rejected.
  ASSERT_TRUE(property_setter_->SetProperty(ANDROID_RB_PROPERTY, ""));
  EXPECT_EQ(BAD_VALUE, interface_->reboot(false, String16("foo"), false));
  EXPECT_EQ("", property_setter_->GetProperty(ANDROID_RB_PROPERTY));
}

TEST_F(PowerManagerTest, Shutdown) {
  EXPECT_EQ(OK, interface_->shutdown(false, String16(), false));
  EXPECT_EQ(PowerManager::kShutdownPrefix,
            property_setter_->GetProperty(ANDROID_RB_PROPERTY));

  EXPECT_EQ(OK, interface_->shutdown(false,
                                     String16(kShutdownReasonUserRequested),
                                     false));
  EXPECT_EQ(std::string(PowerManager::kShutdownPrefix) +
            kShutdownReasonUserRequested,
            property_setter_->GetProperty(ANDROID_RB_PROPERTY));

  // Invalid values should be rejected.
  ASSERT_TRUE(property_setter_->SetProperty(ANDROID_RB_PROPERTY, ""));
  EXPECT_EQ(BAD_VALUE, interface_->shutdown(false, String16("foo"), false));
  EXPECT_EQ("", property_setter_->GetProperty(ANDROID_RB_PROPERTY));
}

}  // namespace android
