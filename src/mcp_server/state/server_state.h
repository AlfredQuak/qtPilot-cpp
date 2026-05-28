// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <QString>

#include "client/discovery_listener.h"
#include "client/probe_client.h"
#include "logging/event_recorder.h"
#include "logging/message_logger.h"
#include "mcp/tool_registry.h"

namespace qtPilot::state {

class ServerState {
 public:
  ServerState();

  mcp::ToolRegistry& toolRegistry();
  const mcp::ToolRegistry& toolRegistry() const;

  client::ProbeClient& probe();
  client::DiscoveryListener& discovery();
  logging::EventRecorder& recorder();
  logging::MessageLogger& logger();

  void setMode(const QString& mode);
  QString mode() const;

  bool connectProbe(const QString& wsUrl);
  void disconnectProbe();

  QJsonObject status() const;

 private:
  QString mode_ = QStringLiteral("native");
  mcp::ToolRegistry registry_;
  client::ProbeClient probe_;
  client::DiscoveryListener discovery_;
  logging::EventRecorder recorder_;
  logging::MessageLogger logger_;
};

}  // namespace qtPilot::state
