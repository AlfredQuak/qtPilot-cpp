// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "tools/cu_tools.h"

#include <QJsonArray>
#include <QJsonDocument>

#include "state/server_state.h"

namespace qtPilot::tools {

namespace {

void registerForwardTool(state::ServerState& state,
                         const QString& toolName,
                         const QString& method) {
  state.toolRegistry().registerTool(
      mcp::ToolDefinition{toolName,
                          QStringLiteral("Forward %1 to %2").arg(toolName, method),
                          QJsonObject{{"type", "object"}},
                          [&state, method, toolName](const QJsonObject& arguments) {
                            try {
                              const QJsonObject result = state.probe().call(method, arguments);
                              return QJsonObject{{"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QString::fromUtf8(QJsonDocument(result).toJson(QJsonDocument::Compact))}}}}};
                            } catch (const std::exception& ex) {
                              return QJsonObject{{"isError", true}, {"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QStringLiteral("%1 failed: %2").arg(toolName, ex.what())}}}}};
                            }
                          }});
}

}  // namespace

void registerCuTools(state::ServerState& state) {
  const QList<std::pair<QString, QString>> mappings = {
      {"cu_screenshot", "cu.screenshot"},
      {"cu_leftClick", "cu.click"},
      {"cu_rightClick", "cu.rightClick"},
      {"cu_middleClick", "cu.middleClick"},
      {"cu_doubleClick", "cu.doubleClick"},
      {"cu_mouseMove", "cu.mouseMove"},
      {"cu_mouseDrag", "cu.mouseDrag"},
      {"cu_mouseDown", "cu.mouseDown"},
      {"cu_mouseUp", "cu.mouseUp"},
      {"cu_type", "cu.type"},
      {"cu_key", "cu.key"},
      {"cu_scroll", "cu.scroll"},
      {"cu_cursorPosition", "cu.cursorPosition"},
  };

  for (const auto& mapping : mappings) {
    registerForwardTool(state, mapping.first, mapping.second);
  }
}

}  // namespace qtPilot::tools
