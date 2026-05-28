// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "tools/recording_tools.h"

#include <QJsonArray>
#include <QJsonDocument>

#include "state/server_state.h"

namespace qtPilot::tools {

void registerRecordingTools(state::ServerState& state) {
  state.toolRegistry().registerTool(
      mcp::ToolDefinition{"qtpilot_record_start",
                          "Start event recording",
                          QJsonObject{{"type", "object"}},
                          [&state](const QJsonObject&) {
                            const QJsonObject result = state.recorder().start();
                            return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QString::fromUtf8(QJsonDocument(result).toJson(QJsonDocument::Compact))}}}}};
                          }});

  state.toolRegistry().registerTool(
      mcp::ToolDefinition{"qtpilot_record_stop",
                          "Stop event recording",
                          QJsonObject{{"type", "object"}},
                          [&state](const QJsonObject&) {
                            const QJsonObject result = state.recorder().stop();
                            return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QString::fromUtf8(QJsonDocument(result).toJson(QJsonDocument::Compact))}}}}};
                          }});

  state.toolRegistry().registerTool(
      mcp::ToolDefinition{"qtpilot_record_status",
                          "Recording status",
                          QJsonObject{{"type", "object"}},
                          [&state](const QJsonObject&) {
                            const QJsonObject result = state.recorder().status();
                            return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QString::fromUtf8(QJsonDocument(result).toJson(QJsonDocument::Compact))}}}}};
                          }});
}

}  // namespace qtPilot::tools
