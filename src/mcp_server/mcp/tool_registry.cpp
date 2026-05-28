// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "mcp/tool_registry.h"

#include <QJsonArray>

namespace qtPilot::mcp {

void ToolRegistry::registerTool(const ToolDefinition& tool) { tools_[tool.name] = tool; }

void ToolRegistry::unregisterTool(const QString& name) { tools_.erase(name); }

bool ToolRegistry::hasTool(const QString& name) const { return tools_.find(name) != tools_.end(); }

QJsonObject ToolRegistry::callTool(const QString& name, const QJsonObject& arguments) const {
  auto it = tools_.find(name);
  if (it == tools_.end()) {
    return QJsonObject{{"isError", true}, {"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QStringLiteral("Unknown tool: %1").arg(name)}}}}};
  }
  return it->second.handler(arguments);
}

QJsonObject ToolRegistry::listTools() const {
  QJsonArray tools;
  for (const auto& item : tools_) {
    const ToolDefinition& tool = item.second;
    tools.append(QJsonObject{{"name", tool.name}, {"description", tool.description}, {"inputSchema", tool.inputSchema}});
  }
  return QJsonObject{{"tools", tools}};
}

}  // namespace qtPilot::mcp
