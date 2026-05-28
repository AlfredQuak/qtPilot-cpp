// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "tools/discovery_tools.h"

#include <QJsonArray>
#include <QJsonDocument>

#include "state/server_state.h"

namespace qtPilot::tools {

void registerDiscoveryTools(state::ServerState& state) {
  state.toolRegistry().registerTool(
      mcp::ToolDefinition{"qtpilot_status",
                          "Return connection and discovery status",
                          QJsonObject{{"type", "object"}},
                          [&state](const QJsonObject&) {
                            return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QString::fromUtf8(QJsonDocument(state.status()).toJson(QJsonDocument::Compact))}}}}};
                          }});

  state.toolRegistry().registerTool(
      mcp::ToolDefinition{"qtpilot_connect_probe",
                          "Connect to probe websocket",
                          QJsonObject{{"type", "object"},
                                      {"properties", QJsonObject{{"ws_url", QJsonObject{{"type", "string"}}}}},
                                      {"required", QJsonArray{"ws_url"}}},
                          [&state](const QJsonObject& arguments) {
                            const QString wsUrl = arguments.value("ws_url").toString();
                            const bool ok = state.connectProbe(wsUrl);
                            return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", ok ? QStringLiteral("connected") : QStringLiteral("connection failed")}}}}, {"isError", !ok}};
                          }});

  state.toolRegistry().registerTool(
      mcp::ToolDefinition{"qtpilot_disconnect_probe",
                          "Disconnect current probe",
                          QJsonObject{{"type", "object"}},
                          [&state](const QJsonObject&) {
                            state.disconnectProbe();
                            return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QStringLiteral("disconnected")}}}}};
                          }});

  state.toolRegistry().registerTool(
      mcp::ToolDefinition{"qtpilot_set_mode",
                          "Set active mode",
                          QJsonObject{{"type", "object"},
                                      {"properties", QJsonObject{{"mode", QJsonObject{{"type", "string"}}}}},
                                      {"required", QJsonArray{"mode"}}},
                          [&state](const QJsonObject& arguments) {
                            const QString mode = arguments.value("mode").toString();
                            state.setMode(mode);
                            return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QStringLiteral("mode set to %1").arg(mode)}}}}};
                          }});
}

}  // namespace qtPilot::tools
