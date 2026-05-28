// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "tools/logging_tools.h"

#include <QJsonArray>
#include <QJsonDocument>

#include "state/server_state.h"

namespace qtPilot::tools {

void registerLoggingTools(state::ServerState& state) {
  state.toolRegistry().registerTool(
      mcp::ToolDefinition{"qtpilot_logging_start",
                          "Start JSONL logging",
                          QJsonObject{{"type", "object"}},
                          [&state](const QJsonObject& arguments) {
                            const QJsonObject result = state.logger().start(arguments.value("path").toString(),
                                                                            arguments.value("level").toInt(2),
                                                                            arguments.value("buffer_size").toInt(200));
                            return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QString::fromUtf8(QJsonDocument(result).toJson(QJsonDocument::Compact))}}}}};
                          }});

  state.toolRegistry().registerTool(
      mcp::ToolDefinition{"qtpilot_logging_stop",
                          "Stop logging",
                          QJsonObject{{"type", "object"}},
                          [&state](const QJsonObject&) {
                            const QJsonObject result = state.logger().stop();
                            return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QString::fromUtf8(QJsonDocument(result).toJson(QJsonDocument::Compact))}}}}};
                          }});

  state.toolRegistry().registerTool(
      mcp::ToolDefinition{"qtpilot_logging_status",
                          "Logging status",
                          QJsonObject{{"type", "object"}},
                          [&state](const QJsonObject&) {
                            const QJsonObject result = state.logger().status();
                            return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QString::fromUtf8(QJsonDocument(result).toJson(QJsonDocument::Compact))}}}}};
                          }});

  state.toolRegistry().registerTool(
      mcp::ToolDefinition{"qtpilot_logging_tail",
                          "Tail log buffer",
                          QJsonObject{{"type", "object"}},
                          [&state](const QJsonObject& arguments) {
                            const QJsonObject result = state.logger().tail(arguments.value("count").toInt(50));
                            return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QString::fromUtf8(QJsonDocument(result).toJson(QJsonDocument::Compact))}}}}};
                          }});
}

}  // namespace qtPilot::tools
