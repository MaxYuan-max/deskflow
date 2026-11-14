/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include "client/ServerProxy1_7.h"
#include "deskflow/KeyboardLayoutManager.h"

#include <string>
#include <string_view>

//! Proxy for server implementing protocol version 1.8
class ServerProxy1_8 : public ServerProxy1_7
{
public:
  ServerProxy1_8(Client *client, deskflow::IStream *stream, IEventQueue *events);
  ~ServerProxy1_8() override = default;

protected:
  ConnectionResult parseHandshakeMessage(const uint8_t *code) override;
  ConnectionResult parseMessage(const uint8_t *code) override;

private:
  void setServerLayouts();
  void setActiveServerLayout(const std::string_view &layout);

  //! Layout to forward with a key event.
  /*!
  Returns the server layout only when it changed since it was last seen. The
  client re-applies the layout on every keystroke it is given, so passing it
  unconditionally would immediately undo an input source change made locally
  (for example with \c Control-Space on macOS).
  */
  std::string layoutSyncFor(const std::string &lang) const;

  std::string m_serverLayout;
  bool m_isUserNotifiedAboutLayoutSyncError = false;
  deskflow::KeyboardLayoutManager m_layoutManager;
};
