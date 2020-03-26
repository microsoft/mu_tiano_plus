##
# this script is for approximating what happens on the server and can be used to build shared_Crypto locally
# It assumes that stuart is setup and able to be invoked from the command line
# please see https://github.com/tianocore/edk2-pytool-extensions for more info 
#
# Copyright (c) Microsoft Corporation
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

import os
import sys
from edk2toollib.utility_functions import RunCmd
import logging
import shutil
import glob


def map_path(path: str):
    path = path.lstrip(os.path.sep)
    path_parts = path.split(os.path.sep)
    if len(path_parts) > 1: # DEBUG_321321/dasdasdsa
        path_parts[0] = path_parts[0].split("_")[0].upper()
        #if path_parts[0] not in ["DEBUG, RELEASE"]:
        #    print(f"Invalid { path_parts[0]}")
        #    return None
    else:
        return None
    if len(path_parts) > 3:
        return None 
    return os.path.sep.join(path_parts)

if __name__ == "__main__":
    clean = False
    verbose = False
    setup = False
    script_dir = os.path.dirname(__file__)    

    root_dir = os.path.abspath(os.path.join(script_dir, "..", "..", ".."))
    nuget_collection_dir = os.path.join(
        root_dir, "Build", "CryptoDriver_Nuget")
    pytools_config = os.path.join(root_dir, ".pytool", "CISettings.py")
    build_output = os.path.join(root_dir, "Build", "CryptoPkg")
    # check if we have the pytool config as a sanity check
    if not os.path.exists(pytools_config):
        print(f"Make sure the pytool config is correct: {pytools_config}")
        sys.exit(2)
    # clean out our collection dir
    if os.path.exists(nuget_collection_dir):
        print(f"Clearing out {nuget_collection_dir}")
        shutil.rmtree(nuget_collection_dir)
        os.makedirs(nuget_collection_dir)

    # make sure we're doing a clean build if requested
    if clean:
        shutil.rmtree(build_output)

    commands = ["stuart_setup", "stuart_ci_setup", "stuart_update"] if setup else []
    # first we do the setup and update and whatnot
    for command in commands:
        print(command)
        ret = RunCmd(command, f"-c {pytools_config}", workingdir=root_dir)
        if ret != 0:
            print(f"{command} failed with code: {ret}")
            sys.exit(ret)

    # copy the files we care about to the folder
    files = ["License.txt", "../readme.md"]
    for file_path in files:
        file_name = os.path.basename(file_path)
        shutil.copyfile(os.path.join(script_dir, file_path),
                        os.path.join(nuget_collection_dir, file_name))

    # now we do the build
    services = ["MIN_PEI", "MIN_DXE_MIN_SMM", "ALL"]
    build_command = "stuart_ci_build"
    for service in services:
        nuget_output_dir = os.path.join(nuget_collection_dir, service)
        os.mkdir(nuget_output_dir)
        print(f"{build_command}: {service}")
        params = f"-p CryptoPkg BLD_*_CRYPTO_SERVICES={service} BUILDREPORTING=TRUE BUILDREPORT_TYPES=\"LIBRARY DEPEX PCD\""
        ret = RunCmd(build_command, f"-c {pytools_config} {params}", workingdir=root_dir)
        if ret != 0:
            print(f"{build_command} failed with code: {ret}")
            sys.exit(ret)
        
        # find all the things we want to find
        build_reports = glob.iglob(os.path.join(build_output, "*", "Build_REPORT.TXT"), recursive=True)
        for build_report in build_reports:
            build_report_rel_path = build_report[len(build_output):] # remove the beginning off the found path
            mapped_path = map_path(build_report_rel_path)
            if mapped_path == None:
                if verbose:
                    print(f"Skipping {build_report_rel_path}")
                continue
            out_path = os.path.join(nuget_output_dir, mapped_path)
            if verbose:
                print(out_path)
            out_path_dir = os.path.dirname(out_path)
            if not os.path.exists(out_path_dir):
                os.makedirs(out_path_dir)
            shutil.copyfile(build_report, out_path)

        efi_files = glob.iglob(os.path.join(build_output, "**", "Crypto*.efi"), recursive=True)
        for efi_file in efi_files:
            efi_rel_path = efi_file[len(build_output):] # remove the beginning off the found path
            mapped_path = map_path(efi_rel_path)
            if mapped_path == None:
                if verbose:
                    print(f"Skipping {efi_rel_path}")
                continue
            out_path = os.path.join(nuget_output_dir, mapped_path)
            if verbose:
                print(out_path)
            out_path_dir = os.path.dirname(out_path)
            if not os.path.exists(out_path_dir):
                os.makedirs(out_path_dir)
            shutil.copyfile(efi_file, out_path)
        
    print("All done")
    sys.exit(0)
