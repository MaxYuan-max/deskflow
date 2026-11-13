/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2014 - 2021 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "KeyboardLayoutManager.h"
#include "base/Log.h"

#include <algorithm>

namespace {

std::string vectorToString(const std::vector<std::string> &vector, const std::string_view &delimiter = "")
{
  std::string string;
  for (const auto &item : vector) {
    if (&item != &vector[0]) {
      string += delimiter;
    }
    string += item;
  }
  return string;
}

} // anonymous namespace

namespace deskflow {

KeyboardLayoutManager::KeyboardLayoutManager(const std::vector<std::string> &localLayouts)
    : m_localLayouts(localLayouts)
{
  LOG_INFO("local layouts: %s", vectorToString(m_localLayouts, ", ").c_str());
}

std::string KeyboardLayoutManager::languageCode(std::string_view identifier)
{
  const auto separator = identifier.find_first_of("-_");
  const auto language = identifier.substr(0, separator);
  if (language.size() != 2) {
    return {};
  }

  char code[2];
  for (size_t i = 0; i < 2; ++i) {
    auto letter = language[i];
    if (letter >= 'A' && letter <= 'Z') {
      letter += 'a' - 'A';
    }
    if (letter < 'a' || letter > 'z') {
      return {};
    }
    code[i] = letter;
  }
  return std::string(code, 2);
}

void KeyboardLayoutManager::setRemoteLayouts(const std::string_view &remoteLayouts)
{
  if (!remoteLayouts.empty() && remoteLayouts.size() % 2 != 0) {
    LOG_ERR("remote layouts are the incorrect size, can not process them");
    return;
  }

  m_remoteLayouts.clear();
  for (size_t i = 0; i + 2 <= remoteLayouts.size(); i += 2) {
    auto rLangs = remoteLayouts.substr(i, 2);
    m_remoteLayouts.emplace_back(rLangs);
  }
  LOG_INFO("remote layouts: %s", vectorToString(m_remoteLayouts, ", ").c_str());
}

const std::vector<std::string> &KeyboardLayoutManager::getRemoteLayouts() const
{
  return m_remoteLayouts;
}

const std::vector<std::string> &KeyboardLayoutManager::getLocalLayouts() const
{
  return m_localLayouts;
}

std::string KeyboardLayoutManager::getMissedLayouts() const
{
  std::string missedLayouts;

  for (const auto &layout : m_remoteLayouts) {
    if (!isLayoutInstalled(layout)) {
      if (!missedLayouts.empty()) {
        missedLayouts += ", ";
      }
      missedLayouts += layout;
    }
  }

  return missedLayouts;
}

std::string KeyboardLayoutManager::getSerializedLocalLayouts() const
{
  return vectorToString(m_localLayouts);
}

bool KeyboardLayoutManager::isLayoutInstalled(const std::string &layout) const
{
  bool isInstalled = true;

  if (!m_localLayouts.empty()) {
    isInstalled = (std::find(m_localLayouts.begin(), m_localLayouts.end(), layout) != m_localLayouts.end());
  }

  return isInstalled;
}

} // namespace deskflow
