// Copyright (c) 2018 The Brave Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "brave/utility/tor/tor_impl.h"

#include "base/command_line.h"
#include "base/process/kill.h"
#include "base/process/launch.h"

namespace brave {

TorLauncherImpl::TorLauncherImpl(
    std::unique_ptr<service_manager::ServiceContextRef> service_ref)
    : service_ref_(std::move(service_ref)) {}

TorLauncherImpl::~TorLauncherImpl() {
  if (tor_process_.IsValid()) {
    base::EnsureProcessTerminated(std::move(tor_process_));
  }
}

void TorLauncherImpl::Launch(const base::FilePath& tor_bin,
                             const std::vector<std::string>& args,
                             LaunchCallback callback) {
  if (tor_process_.IsValid())
    return;

  base::CommandLine cmdline(tor_bin);
  for (size_t i = 0; i < args.size(); ++i) {
    cmdline.AppendArg(args[i]);
  }
  base::LaunchOptions launchopts;
#if defined(OS_LINUX)
  launchopts.kill_on_parent_death = true;
#endif
  tor_process_ = base::LaunchProcess(cmdline, launchopts);

  bool result;
  if (tor_process_.IsValid())
    result = true;
  else
    result = false;

  if (callback)
    std::move(callback).Run(result);
}

void TorLauncherImpl::IsTorAlive(IsTorAliveCallback callback) {
  if (callback)
    std::move(callback).Run(tor_process_.IsValid());
}

}  // namespace brave
