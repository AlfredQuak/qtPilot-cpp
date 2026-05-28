// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "tools/chrome_tools.h"

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

void registerChromeTools(state::ServerState& state) {
  const QList<std::pair<QString, QString>> mappings = {
      {"chr_readPage", "chr.readPage"},
      {"chr_click", "chr.click"},
      {"chr_formInput", "chr.formInput"},
      {"chr_getPageText", "chr.getPageText"},
      {"chr_find", "chr.find"},
      {"chr_navigate", "chr.navigate"},
      {"chr_tabsContext", "chr.tabsContext"},
      {"chr_readConsoleMessages", "chr.readConsoleMessages"},
  };

  for (const auto& mapping : mappings) {
    registerForwardTool(state, mapping.first, mapping.second);
  }
}

}  // namespace qtPilot::tools
