// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "tools/native_tools.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QStringList>

#include "state/server_state.h"

namespace qtPilot::tools {

namespace {

QJsonObject objectSchema() { return QJsonObject{{"type", "object"}}; }

void registerForwardTool(state::ServerState& state,
                         const QString& toolName,
                         const QString& method,
                         const QString& description) {
  state.toolRegistry().registerTool(
      mcp::ToolDefinition{toolName, description, objectSchema(),
                          [&state, method, toolName](const QJsonObject& arguments) {
                            try {
                              const QJsonObject result = state.probe().call(method, arguments);
                              return QJsonObject{{"content",
                                                  QJsonArray{QJsonObject{{"type", "text"},
                                                                         {"text", QString::fromUtf8(QJsonDocument(result).toJson(QJsonDocument::Compact))}}}}};
                            } catch (const std::exception& ex) {
                              return QJsonObject{{"isError", true},
                                                 {"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", QStringLiteral("%1 failed: %2").arg(toolName, ex.what())}}}}};
                            }
                          }});
}

}  // namespace

void registerNativeTools(state::ServerState& state) {
  const QList<std::pair<QString, QString>> mappings = {
      {"qt_ping", "qt.ping"},
      {"qt_version", "qt.version"},
      {"qt_objects_tree", "qt.objects.tree"},
      {"qt_objects_inspect", "qt.objects.inspect"},
      {"qt_objects_search", "qt.objects.search"},
      {"qt_properties_get", "qt.properties.get"},
      {"qt_properties_set", "qt.properties.set"},
      {"qt_methods_invoke", "qt.methods.invoke"},
      {"qt_signals_subscribe", "qt.signals.subscribe"},
      {"qt_signals_unsubscribe", "qt.signals.unsubscribe"},
      {"qt_signals_setLifecycle", "qt.signals.setLifecycle"},
      {"qt_events_start", "qt.events.start"},
      {"qt_events_stop", "qt.events.stop"},
      {"qt_ui_click", "qt.ui.click"},
      {"qt_ui_sendKeys", "qt.ui.sendKeys"},
      {"qt_ui_screenshot", "qt.ui.screenshot"},
      {"qt_ui_geometry", "qt.ui.geometry"},
      {"qt_ui_hitTest", "qt.ui.hitTest"},
      {"qt_names_register", "qt.names.register"},
      {"qt_names_unregister", "qt.names.unregister"},
      {"qt_names_list", "qt.names.list"},
      {"qt_names_validate", "qt.names.validate"},
      {"qt_names_load", "qt.names.load"},
      {"qt_models_list", "qt.models.list"},
      {"qt_models_data", "qt.models.data"},
      {"qt_models_search", "qt.models.search"},
      {"qt_ui_clickItem", "qt.ui.clickItem"},
  };

  for (const auto& mapping : mappings) {
    registerForwardTool(state, mapping.first, mapping.second,
                        QStringLiteral("Forward %1 to probe method %2").arg(mapping.first, mapping.second));
  }
}

}  // namespace qtPilot::tools
