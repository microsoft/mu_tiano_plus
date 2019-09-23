# @file
#
# Copyright (c), Microsoft Corporation
# SPDX-License-Identifier: BSD-2-Clause-Patent
##
import os
from edk2toolext.environment import shell_environment
from edk2toolext.invocables.edk2_ci_build import CiBuildSettingsManager
from edk2toolext.invocables.edk2_ci_setup import CiSetupSettingsManager
from edk2toolext.invocables.edk2_setup import SetupSettingsManager
from edk2toolext.invocables.edk2_update import UpdateSettingsManager
from edk2toollib.utility_functions import GetHostInfo


class Settings(CiBuildSettingsManager, CiSetupSettingsManager, UpdateSettingsManager, SetupSettingsManager):

    def __init__(self):
        # skip dependency check until we can get it under control
        plugin_skip_list = ["DependencyCheck"]
        env = shell_environment.GetBuildVars()
        for plugin in plugin_skip_list:
            env.SetValue(plugin.upper(), "skip", "set from settings file")
        pass

    def AddCommandLineOptions(self, parserObj):
        parserObj.add_argument('--Tool_Chain', "--toolchain", "--tool_chain", dest='tool_chain_tag',
                               default="VS2017", type=str, help='tool chain tag to use for this build')

    def RetrieveCommandLineOptions(self, args):
        if args.tool_chain_tag is not None:
            shell_environment.GetBuildVars().SetValue(
                "TOOL_CHAIN_TAG", args.tool_chain_tag, "Set as cli parameter")
        # cache this so usage within CISettings is consistant.
        self.ToolChainTagCacheValue = args.tool_chain_tag

    def GetRequiredRepos(self):
        return ["CryptoPkg/Library/OpensslLib/openssl"]

    def GetActiveScopes(self):
        ''' get scope '''
        scopes = ("corebuild", "project_mu")

        if (GetHostInfo().os == "Linux"
            and "AARCH64" in self.GetArchSupported() and
            self.ToolChainTagCacheValue is not None and
                self.ToolChainTagCacheValue.upper().startswith("GCC")):
            scopes += ("gcc_aarch64_linux",)

        return scopes

    def GetName(self):
        ''' The name of this repo '''
        return "TianoPlus"

    def GetDependencies(self):
        return [
            {
                "Path": "Silicon/Arm/MU_TIANO",
                "Url": "https://github.com/Microsoft/mu_silicon_arm_tiano.git",
                "Branch": "dev/201908"
            },
            {
                "Path": "MU_BASECORE",
                "Url": "https://github.com/Microsoft/mu_basecore.git",
                "Branch": "dev/201908"
            }
        ]

    def GetPackages(self):
        return (
            "CryptoPkg",
            "EmbeddedPkg",
            "FatPkg",
            "FmpDevicePkg",
            "ShellPkg",
            "SourceLevelDebugPkg")

    def GetPackagesPath(self):
        return ()

    def GetArchSupported(self):
        # Currently AARCH64 doesn't work with the MSVC compiler so disable it for now
        if self.ToolChainTagCacheValue and self.ToolChainTagCacheValue.upper().startswith("VS"):
            return ("IA32", "X64")
        else:
            return ("IA32",
                    "X64")  # removing aarch64 because too many errors
            #   "AARCH64")

    def GetTargetsSupported(self):
        return ("DEBUG", "RELEASE")

    def GetWorkspaceRoot(self):
        ''' get WorkspacePath '''
        return os.path.dirname(os.path.abspath(__file__))
