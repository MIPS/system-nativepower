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

#include <unistd.h>

#include <memory>

#include <base/at_exit.h>
#include <base/logging.h>
#include <base/message_loop/message_loop.h>
#include <binderwrapper/binder_wrapper.h>
#include <chromeos/flag_helper.h>
#include <nativepower/wake_lock.h>

using android::BinderWrapper;
using android::WakeLock;

int main(int argc, char *argv[]) {
  DEFINE_int32(sleep_sec, 5, "Number of seconds to sleep");

  chromeos::FlagHelper::Init(argc, argv, "Example power-management client.");
  logging::InitLogging(logging::LoggingSettings());
  base::AtExitManager at_exit;
  base::MessageLoopForIO loop;
  BinderWrapper::Create();

  LOG(INFO) << "Creating wake lock";
  std::unique_ptr<WakeLock> lock(WakeLock::Create("power_example", "power"));

  LOG(INFO) << "Sleeping for " << FLAGS_sleep_sec << " seconds";
  sleep(FLAGS_sleep_sec);

  LOG(INFO) << "Exiting";
  return 0;
}
