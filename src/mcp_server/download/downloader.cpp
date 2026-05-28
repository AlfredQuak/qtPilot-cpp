// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "download/downloader.h"

#include <cstdio>

namespace qtPilot::download {

bool Downloader::downloadTools(const QString& qtVersion, const QString& outputDir) const {
  fprintf(stderr,
          "download-tools is not fully implemented yet in C++ server. "
          "Requested qtVersion=%s outputDir=%s\n",
          qPrintable(qtVersion), qPrintable(outputDir));
  return false;
}

}  // namespace qtPilot::download
