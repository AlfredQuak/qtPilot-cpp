// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <QString>

namespace qtPilot::download {

class Downloader {
 public:
  bool downloadTools(const QString& qtVersion, const QString& outputDir) const;
};

}  // namespace qtPilot::download
