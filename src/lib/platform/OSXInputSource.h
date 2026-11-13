/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */
#pragma once

#include "platform/OSXAutoTypes.h"

#include <string>

namespace deskflow::osx {

AutoCFArray copySelectableInputSources();
AutoTISInputSourceRef copyKeyTranslationSource(TISInputSourceRef source);
std::string inputSourceLanguage(TISInputSourceRef source);
std::string currentInputLanguage();

} // namespace deskflow::osx
