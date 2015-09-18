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

#include <memory>

#include <base/macros.h>
#include <binder/Binder.h>
#include <binder/IBinder.h>
#include <binderwrapper/binder_test_base.h>
#include <binderwrapper/stub_binder_wrapper.h>
#include <nativepower/constants.h>
#include <nativepower/power_manager_stub.h>
#include <nativepower/wake_lock.h>

namespace android {

class WakeLockTest : public BinderTestBase {
 public:
  WakeLockTest()
      : power_manager_(new PowerManagerStub()),
        power_manager_binder_(power_manager_) {
    binder_wrapper_->SetBinderForService(kPowerManagerServiceName,
                                         power_manager_binder_);
  }
  ~WakeLockTest() override = default;

 protected:
  PowerManagerStub* power_manager_;  // Owned by |power_manager_binder_|.
  sp<IBinder> power_manager_binder_;

 private:
  DISALLOW_COPY_AND_ASSIGN(WakeLockTest);
};

TEST_F(WakeLockTest, CreateAndDestroy) {
  std::unique_ptr<WakeLock> lock(WakeLock::Create("foo", "bar"));
  ASSERT_EQ(1u, power_manager_->num_locks());
  ASSERT_EQ(1u, binder_wrapper()->local_binders().size());
  EXPECT_EQ(
      PowerManagerStub::ConstructLockString("foo", "bar", -1),
      power_manager_->GetLockString(binder_wrapper()->local_binders()[0]));

  lock.reset();
  EXPECT_EQ(0u, power_manager_->num_locks());
}

TEST_F(WakeLockTest, PowerManagerDeath) {
  std::unique_ptr<WakeLock> lock(WakeLock::Create("foo", "bar"));
  binder_wrapper()->NotifyAboutBinderDeath(power_manager_binder_);

  // Since the WakeLock was informed that the power manager died, it shouldn't
  // try to release its lock on destruction.
  lock.reset();
  EXPECT_EQ(1u, power_manager_->num_locks());
}

}  // namespace android
