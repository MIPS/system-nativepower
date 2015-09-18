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

#include <nativepower/BnPowerManager.h>

#include <binder/Parcel.h>

namespace android {

status_t BnPowerManager::onTransact(uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags) {
  switch (code) {
    case IPowerManager::ACQUIRE_WAKE_LOCK: {
      // The parameter orders in IPowerManager.aidl and IPowerManager.h don't
      // match. :-( (BpPowerManager in IPowerManager.cpp passes arguments in the
      // order that the AIDL file describes and that we use here, though.)
      CHECK_INTERFACE(IPowerManager, data, reply);
      sp<IBinder> lock = data.readStrongBinder();
      int32_t flags = data.readInt32();
      String16 tag = data.readString16();
      String16 package_name = data.readString16();
      // Ignore work source and history.
      return acquireWakeLock(flags, lock, tag, package_name);
    }
    case IPowerManager::ACQUIRE_WAKE_LOCK_UID: {
      CHECK_INTERFACE(IPowerManager, data, reply);
      sp<IBinder> lock = data.readStrongBinder();
      int32_t flags = data.readInt32();
      String16 tag = data.readString16();
      String16 package_name = data.readString16();
      int32_t uid = data.readInt32();
      return acquireWakeLockWithUid(flags, lock, tag, package_name, uid);
    }
    case IPowerManager::RELEASE_WAKE_LOCK: {
      CHECK_INTERFACE(IPowerManager, data, reply);
      sp<IBinder> lock = data.readStrongBinder();
      int32_t flags = data.readInt32();
      return releaseWakeLock(lock, flags);
    }
    case IPowerManager::UPDATE_WAKE_LOCK_UIDS: {
      CHECK_INTERFACE(IPowerManager, data, reply);
      sp<IBinder> lock = data.readStrongBinder();
      // TODO: There's no Parcel::readInt32Array(). Add one.
      return updateWakeLockUids(lock, 0, nullptr);
    }
    case IPowerManager::POWER_HINT: {
      CHECK_INTERFACE(IPowerManager, data, reply);
      int32_t hint_id = data.readInt32();
      int32_t params = data.readInt32();
      return powerHint(hint_id, params);
    }
    default:
      return BBinder::onTransact(code, data, reply, flags);
  }
}

}  // namespace android
