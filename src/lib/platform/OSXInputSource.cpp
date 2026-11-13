/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */
#include "platform/OSXInputSource.h"
#include "deskflow/KeyboardLayoutManager.h"

namespace deskflow::osx {

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
