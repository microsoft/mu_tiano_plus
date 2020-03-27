import os
import sys
SCRIPT_DIR = os.path.dirname(__file__)
sys.path.append(os.path.join(SCRIPT_DIR, "..", "Packaging"))
from generate_cryptodriver import generate_file_replacement
from generate_cryptodriver import get_flavors


def main():
    class options():
        out_dir = SCRIPT_DIR
        in_dir = SCRIPT_DIR
        verbose = True
    flavors = get_flavors()
    # first we need to generate the INF files
    inf_files = []
    for flavor in flavors:
        for phase in ["PEI", "DXE", "SMM"]:
            for target in ["DEBUG", "RELEASE"]:
                inf_files.append((flavor, phase, target))
    print(f"Generating {len(inf_files)} inf files")

    for flavor, phase, target in inf_files:
        inf_lines = []
        guid = flavors[flavor]["guid"]
        inf_lines.extend(["[Defines]",
                    "INF_VERSION                    = 0x0001001B",
                    "BASE_NAME                      = BaseCryptoDriverDxe",
                    "MODULE_UNI_FILE                = Crypto.uni",
                    f"FILE_GUID                      = {guid}",
                    "MODULE_TYPE                    = DXE_DRIVER",
                    "VERSION_STRING                 = 1.0",
                    "ENTRY_POINT                    = CryptoDxeEntry"])
        generate_file_replacement(inf_lines, None, f"CryptoDriver{flavor}_{phase}_{target}.inf", options(), comment="#")
    
    dsc_lines = []

    # now we generate the DSC include
    generate_file_replacement(dsc_lines, None, "CryptoDriver.inc.dsc", options(), comment="#")
    # now we generate the FDF includes (there should be two, one for BOOTBLOCK and one for DXE)


if __name__ == "__main__":
    main()
