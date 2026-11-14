/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */
#pragma once

#include "platform/OSXAutoTypes.h"

#include <functional>
#include <string>

namespace deskflow::osx {

AutoCFArray copySelectableInputSources();
AutoTISInputSourceRef copyKeyTranslationSource(TISInputSourceRef source);
std::string inputSourceLanguage(TISInputSourceRef source);
std::string currentInputLanguage();

// TIS/TSM is not thread-safe. AppKit queries input sources on the main thread
// (menu shortcut updates, for example), so selecting an input source from a
// deskflow worker thread races with it and aborts the process. Run such work
// through this helper to keep it serialised with AppKit. Returns the result of
// the operation, or false if the main thread did not run it in time.
bool runOnMainThread(const std::function<bool()> &operation);

} // namespace deskflow::osx
