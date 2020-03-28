import os
import sys
import glob
SCRIPT_DIR = os.path.dirname(__file__)
sys.path.append(os.path.join(SCRIPT_DIR, "..", "Packaging"))
try:
    from generate_cryptodriver import get_flavors
    from generate_cryptodriver import generate_file_replacement
except:
    print("Failed to import")
    sys.exit(1)


def delete_files_of_pattern(pattern):
    files = glob.iglob(os.path.join(SCRIPT_DIR, pattern))
    for file_path in files:
        os.remove(file_path)


def main():
    class options():
        out_dir = SCRIPT_DIR
        in_dir = SCRIPT_DIR
        verbose = False
    flavors = get_flavors()
    phases = ["Pei", "Dxe", "Smm"]
    arches = ["X64", "ARM", "AARCH64", "IA32"]
    targets = ["DEBUG", "RELEASE"]

    # first we need to generate the INF files
    inf_files = []
    for flavor in flavors:
        for phase in phases:
            for target in targets:
                inf_files.append((flavor, phase, target))
    print(f"Generating {len(inf_files)} inf files")
    # first delete any files that we don't need
    inf_start = "CryptoDriverBin"
    delete_files_of_pattern(f"{inf_start}*.inf")

    for flavor, phase, target in inf_files:
        inf_lines = []
        guid = flavors[flavor]["guid"]
        module_types = {
            "Dxe": "DXE_DRIVER",
            "Pei": "PEIM",
            "Smm": "DXE_SMM_DRIVER"
        }
        mod_type = module_types[phase]
        if target == "DEBUG":
            guid = guid[:-4] + "D00"
        if target == "RELEASE":
            guid = guid[:-4] + "E00"
        if phase == "Pei":
            guid = guid[:-3] + '10'
        if phase == "Dxe":
            guid = guid[:-3] + '20'
        if phase == "Smm":
            guid = guid[:-3] + '30'
        inf_lines.extend(["[Defines]",
                          "INF_VERSION                    = 0x0001001B",
                          f"BASE_NAME                      = BaseCryptoDriver{phase}",
                          "MODULE_UNI_FILE                = Crypto.uni",
                          f"FILE_GUID                      = {guid}",
                          f"MODULE_TYPE                    = {mod_type}",
                          "VERSION_STRING                 = 1.0",
                          f"ENTRY_POINT                    = Crypto{phase}Entry"])
        inf_lines.append("\n[Binaries]")
        inf_lines.append(
            f"  DXE_DEPEX|edk2-basecrypto-driver-bin_extdep/{flavor}/{target}/Crypto{phase}.depex|{target}")
        for arch in arches:
            inf_lines.append(f"\n[Binaries.{arch}]")
            inf_lines.append(
                f"  PE32|edk2-basecrypto-driver-bin_extdep/{flavor}/{target}/{arch}/Crypto{phase}.efi")

        generate_file_replacement(
            inf_lines, None, f"{inf_start}_{flavor}_{phase}_{target}.inf", options(), comment="#")

    # now we generate the DSC include
    dsc_lines = []
    dsc_lines.append("# this is to be included by a platform :)")
    dsc_lines.append("[Defines]")
    all_flavors = "ALL "+" ".join(list(flavors))
    for phase in phases:
        phase = phase.upper()
        dsc_lines.append(f"!ifndef {phase}_CRYPTO_SERVICES")
        dsc_lines.append(f" DEFINE {phase}_CRYPTO_SERVICES = ALL")
        dsc_lines.append("!endif")
        dsc_lines.append(
            f"!if $({phase}_CRYPTO_SERVICES) IN \"{all_flavors}\"")
        dsc_lines.append(" # we don't have a problem")
        dsc_lines.append("!else")
        dsc_lines.append(
            f" !error CRYPTO_SERVICES must be set to one of {all_flavors}.")
        dsc_lines.append("!endif")
        dsc_lines.append(f"!ifndef {phase}_CRYPTO_ARCH")
        default_arch = "IA32" if phase == "PEI" else "X64"
        dsc_lines.append(f" DEFINE {phase}_CRYPTO_ARCH = {default_arch}")
        dsc_lines.append("!endif")
        dsc_lines.append("")

    # generate the components to include
    for flavor in flavors:
        for phase in phases:
            uphase = phase.upper()
            dsc_lines.append(f"!if $({uphase}_CRYPTO_SERVICES) == \"{flavor}\"")
            if phase == "Pei":
                dsc_lines.append("  [Components.$(PEI_CRYPTO_ARCH)]")
            elif phase == "Smm":
                dsc_lines.append("  [Components.$(SMM_CRYPTO_ARCH)]")
            else:
                dsc_lines.append("  [Components.$(DXE_CRYPTO_ARCH)]")
            dsc_lines.append(f"    CryptoPkg/Driver/Bin/{inf_start}_{flavor}_{phase}_$(TARGET).inf ")
            dsc_lines.append("")
            # Add the librart as well
            comp_types = ["", ]
            if phase == "Pei":
                comp_types = ["PEIM", ]
            elif phase == "Dxe":
                comp_types = ["DXE_DRIVER", "UEFI_DRIVER", "UEFI_APPLICATION"]
            elif phase == "Smm":
                comp_types = ["DXE_SMM_DRIVER", ]
            comp_str = ", ".join(
                map(lambda x: "Components."+x.upper(), comp_types))
            dsc_lines.append(f" [{comp_str}]")
            dsc_lines.append(f"   CryptoPkg/Library/BaseCryptLibOnProtocolPpi/{phase}CryptLib.inf " + "{")
            dsc_lines.append("     <PcdsFixedAtBuild>")
            dsc_lines.append(f"      !include CryptoPkg/Driver/Packaging/Crypto.pcd.{flavor}.inc.dsc")
            dsc_lines.append("    }")
            dsc_lines.append("!endif\n")
    dsc_lines.append("")
    # generate the library classes to include
    generate_file_replacement(
        dsc_lines, None, "CryptoDriver.inc.dsc", options(), comment="#")

    # now we generate the FDF includes (there should be two, one for BOOTBLOCK and one for DXE)

    fdf_bb_lines = []
    fdf_bb_lines.append(
        "# this is to be included a platform inside the BOOTBLOCK or other PEI FV")
    fdf_bb_lines.append("!ifndef PEI_CRYPTO_SERVICES")
    fdf_bb_lines.append("!error You need to define PEI_CRYPTO_SERVICES")
    fdf_bb_lines.append("!endif")
    for flavor in flavors:
        fdf_bb_lines.append(f"!if $(PEI_CRYPTO_SERVICES) == {flavor}")
        for target in targets:
            fdf_bb_lines.append(f" !if $(TARGET) == {target}")
            fdf_bb_lines.append(
                f"    INF  CryptoPkg/Driver/Bin/{inf_start}_{flavor}_Pei_{target}.inf")
            fdf_bb_lines.append("  !endif")
        fdf_bb_lines.append("!endif\n")
    generate_file_replacement(
        fdf_bb_lines, None, "CryptoDriver.BOOTBLOCK.inc.fdf", options(), comment="#")

    fdf_dxe_lines = []
    fdf_dxe_lines.append(
        "# this is to be included a platform inside the BOOTBLOCK or other PEI FV")
    fdf_dxe_lines.append("!ifndef DXE_CRYPTO_SERVICES")
    fdf_dxe_lines.append(
        "!error You need to define in your platform DXE_CRYPTO_SERVICES")
    fdf_dxe_lines.append("!endif")
    fdf_dxe_lines.append("!ifndef SMM_CRYPTO_SERVICES")
    fdf_dxe_lines.append(
        "!error You need to define in your platform SMM_CRYPTO_SERVICES")
    fdf_dxe_lines.append("!endif")
    for flavor in flavors:
        for phase in ["Dxe", "Smm"]:
            fdf_dxe_lines.append(
                f"!if $({phase.upper()}_CRYPTO_SERVICES) == {flavor}")
            for target in targets:
                fdf_dxe_lines.append(f" !if $(TARGET) == {target}")
                fdf_dxe_lines.append(
                    f"    INF  CryptoPkg/Driver/Bin/{inf_start}_{flavor}_{phase}_{target}.inf")
                fdf_dxe_lines.append("  !endif")
            fdf_dxe_lines.append("!endif\n")
    generate_file_replacement(
        fdf_dxe_lines, None, "CryptoDriver.DXE.inc.fdf", options(), comment="#")


if __name__ == "__main__":
    main()
