// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "state/server_state.h"

namespace qtPilot::state {

ServerState::ServerState() {
  probe_.addNotificationHandler([this](const QString& method, const QJsonObject& params) {
    recorder_.recordNotification(method, params);
    if (logger_.isActive()) {
      logger_.log(QJsonObject{{"dir", "ntf"}, {"method", method}, {"params", params}});
    }
  });
}

mcp::ToolRegistry& ServerState::toolRegistry() { return registry_; }

const mcp::ToolRegistry& ServerState::toolRegistry() const { return registry_; }

client::ProbeClient& ServerState::probe() { return probe_; }

client::DiscoveryListener& ServerState::discovery() { return discovery_; }

logging::EventRecorder& ServerState::recorder() { return recorder_; }

logging::MessageLogger& ServerState::logger() { return logger_; }

void ServerState::setMode(const QString& mode) { mode_ = mode; }

QString ServerState::mode() const { return mode_; }

bool ServerState::connectProbe(const QString& wsUrl) { return probe_.connectToUrl(wsUrl); }

void ServerState::disconnectProbe() { probe_.disconnectFromProbe(); }

QJsonObject ServerState::status() const {
  return QJsonObject{{"connected", probe_.isConnected()},
                     {"ws_url", probe_.isConnected() ? probe_.wsUrl() : QString()},
                     {"mode", mode_},
                     {"discovery_active", discovery_.isRunning()},
                     {"discovered_probes", discovery_.probes().size()}};
}

}  // namespace qtPilot::state
