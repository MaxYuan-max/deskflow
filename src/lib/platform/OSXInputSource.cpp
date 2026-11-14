/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */
#include "platform/OSXInputSource.h"
#include "base/Log.h"
#include "deskflow/KeyboardLayoutManager.h"

#include <dispatch/dispatch.h>
#include <pthread.h>

namespace deskflow::osx {

namespace {

struct MainThreadRequest
{
  std::function<bool()> operation;
  dispatch_semaphore_t done;
  bool result = false;
};

void runMainThreadRequest(void *context)
{
  auto *request = static_cast<MainThreadRequest *>(context);
  request->result = request->operation();
  dispatch_semaphore_signal(request->done);
}

} // namespace

bool runOnMainThread(const std::function<bool()> &operation)
{
  if (pthread_main_np() != 0) {
    return operation();
  }

  // Deliberately leaked when the wait times out: the main thread may still run
  // the request afterwards, so the captured state has to outlive this call.
  auto *request = new MainThreadRequest{operation, dispatch_semaphore_create(0), false};
  dispatch_async_f(dispatch_get_main_queue(), request, runMainThreadRequest);

  const auto timeout = dispatch_time(DISPATCH_TIME_NOW, 500 * NSEC_PER_MSEC);
  if (dispatch_semaphore_wait(request->done, timeout) != 0) {
    LOG_WARN("timed out waiting for the main thread to run input source work");
    return false;
  }

  const bool result = request->result;
  dispatch_release(request->done);
  delete request;
  return result;
}

AutoCFArray copySelectableInputSources()
{
  const void *keys[] = {kTISPropertyInputSourceCategory, kTISPropertyInputSourceIsSelectCapable};
  const void *values[] = {kTISCategoryKeyboardInputSource, kCFBooleanTrue};
  AutoCFDictionary filter(CFDictionaryCreate(nullptr, keys, values, 2, nullptr, nullptr), CFRelease);

  std::lock_guard<std::mutex> lock(g_tisMutex);
  return AutoCFArray(TISCreateInputSourceList(filter.get(), false), CFRelease);
}

AutoTISInputSourceRef copyKeyTranslationSource(TISInputSourceRef source)
{
  std::lock_guard<std::mutex> lock(g_tisMutex);
  if (!source) {
    return {nullptr, CFRelease};
  }
  if (TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData)) {
    return {reinterpret_cast<TISInputSourceRef>(const_cast<void *>(CFRetain(source))), CFRelease};
  }

  AutoTISInputSourceRef current(TISCopyCurrentKeyboardInputSource(), CFRelease);
  const auto sourceID = TISGetInputSourceProperty(source, kTISPropertyInputSourceID);
  const auto currentID = current ? TISGetInputSourceProperty(current.get(), kTISPropertyInputSourceID) : nullptr;
  if (sourceID && currentID && CFEqual(sourceID, currentID)) {
    AutoTISInputSourceRef override(TISCopyInputMethodKeyboardLayoutOverride(), CFRelease);
    if (override && TISGetInputSourceProperty(override.get(), kTISPropertyUnicodeKeyLayoutData)) {
      return override;
    }
  }
  return {TISCopyCurrentASCIICapableKeyboardLayoutInputSource(), CFRelease};
}

std::string inputSourceLanguage(TISInputSourceRef source)
{
  std::lock_guard<std::mutex> lock(g_tisMutex);
  if (!source) {
    return {};
  }
  const auto languages = static_cast<CFArrayRef>(TISGetInputSourceProperty(source, kTISPropertyInputSourceLanguages));
  if (!languages || CFArrayGetCount(languages) == 0) {
    return {};
  }
  const auto language = static_cast<CFStringRef>(CFArrayGetValueAtIndex(languages, 0));
  char identifier[128];
  if (!language || !CFStringGetCString(language, identifier, sizeof(identifier), kCFStringEncodingUTF8)) {
    return {};
  }
  return KeyboardLayoutManager::languageCode(identifier);
}

std::string currentInputLanguage()
{
  AutoTISInputSourceRef source(nullptr, CFRelease);
  {
    std::lock_guard<std::mutex> lock(g_tisMutex);
    source.reset(TISCopyCurrentKeyboardInputSource());
  }
  return inputSourceLanguage(source.get());
}

} // namespace deskflow::osx
