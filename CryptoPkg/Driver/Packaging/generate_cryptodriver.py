##
# This file creates three files for the CryptoDriver
# - The Crypto.c file that populates the protocol struct
# - The Crypto.h file which contains defintions for the protocol itself and the functions
# - The Pcd.inc.dec file which has a list of all the PCD's the will get defined
#
# Copyright (c) Microsoft Corporation
# SPDX-License-Identifier: BSD-2-Clause-Patent
##
import os
import sys
import argparse
import datetime
from enum import Enum

SCRIPT_DIR = os.path.dirname(__file__)
ROOT_DIR = os.path.dirname(os.path.dirname(os.path.dirname(SCRIPT_DIR)))
REL_PATH = os.path.relpath(__file__, ROOT_DIR)


def main():
    options = ParseCommandLineOptions()
    crypto_functions = read_basecryptlib_and_tlslib(options)
    # generate the requested files
    if options.c_file:
        get_crypto_c(options, crypto_functions)
    if options.h_file:
        get_crypto_h(options, crypto_functions)
    if options.p_file:
        get_crypto_pcds(options, crypto_functions)
    if options.d_file:
        get_crypto_dsc(options, crypto_functions)
    if options.i_file:
        get_crypto_inf(options, crypto_functions)
    print("Success. Thanks for playing :)")


def ParseCommandLineOptions():
    ''' parse arguments '''
    ParserObj = argparse.ArgumentParser()
    ParserObj.add_argument("-c", "--c-file", dest="c_file", default=False, action="store_true",
                           help="Creates the crypto.c file")
    ParserObj.add_argument("-hdr", "--h-file", dest="h_file", default=False, action='store_true',
                           help="Creates the crypto.h file")
    ParserObj.add_argument("-pcd", "--pcd-file", dest="p_file", default=False, action='store_true',
                           help="Creates the pcd.inc.dec file")
    ParserObj.add_argument("-dsc", "--dsc-file", dest="d_file", default=False, action='store_true',
                           help="Creates the pcd.inc.dsc file")
    ParserObj.add_argument("-inf", "--inf-file", dest="i_file", default=False, action='store_true',
                           help="Creates the pcd.inc.inf file")
    ParserObj.add_argument("-a", "--all", dest="all", default=False, action='store_true',
                           help="Creates the crypto.c, crypto.h, and the pcd.inc.dec file")
    ParserObj.add_argument("-v", "--verbose", dest="verbose", default=False, action='store_true',
                           help="Logs verbosely")
    ParserObj.add_argument("-index", "--pcd-index", dest="pcd_index", default="0x02",
                           help="The hex index where the PCD will start (default 0x02")
    ParserObj.add_argument("-o", "--out", dest="out_dir", default="./",
                           help="The directory where the files are stored")
    ParserObj.add_argument("-in", "--template", dest="in_dir", default="./",
                           help="The directory where the template files are stored")
    options = ParserObj.parse_args()
    if options.all:
        options.p_file = True
        options.h_file = True
        options.c_file = True
        options.d_file = True
        options.i_file = True

    options.out_dir = os.path.abspath(
        os.path.join(SCRIPT_DIR, options.out_dir))

    if not os.path.exists(options.out_dir):
        raise FileNotFoundError(
            f"Make sure your output directory exists: {options.out_dir}")

    if options.verbose:
        print(f"Writing output to: {options.out_dir}")

    if not options.c_file and not options.h_file and not options.p_file:
        raise ValueError(
            "You must specify at least one file to build. Run -h if you're not sure")

    return options


def read_header_file(options, path):
    ''' reads a header file and pulls out the function definitions'''
    if not path.lower().endswith(".h"):
        raise ValueError("I cannot parse a non header file")
    header_file = open(path, 'r')
    modes = Enum("HEADER_PARSE_MODES", [
                 "COMMENT", "RETURN_TYPE", "EFI_API", "NAME", "PARAMS", "FINISHED", "UNKNOWN"])
    mode = modes.COMMENT
    file_name = os.path.basename(path)

    class crypto_function:
        ''' creates a blank function object'''

        def __init__(self):
            self.comment = []
            self.name = ""
            self.return_type = "VOID"
            self.params = []
            self.type = None
            self.source = file_name
            self.line_no = 0  # this is the line number that the name comes from

        def __repr__(self):
            return self.name

        def get_raw_repr(self):
            ''' this returns what was in the BaseCryptLib.h '''
            lines = []
            lines.extend(self.comment)
            lines.append(self.return_type)
            lines.append("EFI_API")
            lines.append(
                f"{self.name} ( // From {self.source}:{self.line_no}")
            lines.extend(self.params)
            lines.append("  );")

            return "\n".join(lines)

        def set_return_type_if_valid(self, line: str):
            line = line.upper()
            if line not in ["VOID *", "VOID*", "BOOLEAN", "UINTN", "VOID", "RETURN_STATUS", "UINT8", "UINT16", "UINT32", "INTN", "EFI_STATUS"]:
                return False
            self.return_type = line
            return True

        @classmethod
        def valid_types(cls):
            return ["HMACMD5", "HMACSHA1", "HMACSHA256", "MD4", "MD5", "PKCS", "DH", "RANDOM", "RSA", "SHA1",
                    "SHA256", "SHA384", "SHA512", "X509", "TDES", "AES", "ARC4", "SM3", "HKDF", "TLS", "TLSSET", "TLSGET"]

        def get_escaped_name(self):
            if self.name is None or len(self.name) == 0:
                return "???"
            escape_name = ""
            last_char_upper = True
            for char in self.name:
                cur_char_upper = char.isupper()
                if not last_char_upper and cur_char_upper:
                    escape_name += "_"
                escape_name += char.upper()
                last_char_upper = cur_char_upper
            return escape_name

        def get_protocoled_name(self):
            return "EDKII_CRYPTO_" + self.get_escaped_name()

        def get_default_value(self):
            if self.return_type == "BOOLEAN":
                return "FALSE"
            if self.return_type == "VOID*" or self.return_type == "VOID *":
                return "NULL"
            return "0"

        def get_param_tuple(self):
            param_names = list(
                map(lambda x: x.split()[-1].strip("*").strip(","), self.params))
            if len(param_names) == 1 and param_names[0] == 'VOID':
                return "()"
            return "(" + ", ".join(param_names) + ")"

        def get_pcd_name(self):
            return "gEfiCryptoPkgTokenSpaceGuid.PcdCryptoService"+self.name

        def get_type(self):
            ''' Based on the name, get the type '''
            if self.type is not None:
                return self.type
            if self.name == "":
                return None
            valid_types = self.valid_types()
            name = self.name.upper()
            if name == "VERIFYEKUSINPKCS7SIGNATURE" or name == "AUTHENTICODEVERIFY" or name == "IMAGETIMESTAMPVERIFY":
                name = "PKCS7"+name
            # filter them so it's only things that start with that
            possible_types = filter(lambda x: name.startswith(x), valid_types)
            self.type = ""
            for possible_type in possible_types:
                self.type = possible_type if len(
                    possible_type) > len(self.type) else self.type
            if self.type == "":
                raise ValueError(
                    f"{self.source}:{self.line_no} Unknown type: {name}")

            return self.type

    cur_function = crypto_function()
    all_functions = []
    # iterate through each line in the header
    for index, line in enumerate(header_file.readlines()):
        line = line.strip()
        if len(line) == 0 or line.startswith("#") or line.startswith("//"):
            continue
        if line.startswith("/**"):  # if we find a comment
            cur_function.comment = [line, ]
            mode = modes.COMMENT
        elif line.endswith("**/"):
            mode = modes.RETURN_TYPE
            cur_function.comment.append(line)
        elif mode == modes.COMMENT:
            cur_function.comment.append(line)
        elif mode == modes.PARAMS and line.endswith(";"):
            all_functions.append(cur_function)
            cur_function = crypto_function()
        elif mode == modes.PARAMS:
            cur_function.params.append("  "+line)
        elif mode == modes.RETURN_TYPE:
            if not cur_function.set_return_type_if_valid(line):
                if options.verbose:
                    print(f"{file_name}:{index} Invalid Return type: {line}")
                mode = modes.UNKNOWN
            else:
                mode = modes.EFI_API
        elif mode == modes.EFI_API:
            mode = modes.NAME if line == "EFIAPI" else modes.UNKNOWN
        elif mode == modes.NAME:
            if not line.endswith("(") != 0:
                mode = modes.UNKNOWN
            else:
                cur_function.line_no = index
                cur_function.name = line.strip("(").strip()
                mode = modes.PARAMS

    if options.verbose:
        print(f"Found {len(all_functions)} functions from {file_name}")
    return all_functions


def read_basecryptlib_and_tlslib(options):
    ''' reads in the BaseCryptLib and TlsLib header to generate the protocol '''
    # first we must find the BaseCryptLib and TlsLib
    # we can assume that it's located in ..\..\Include\Library\BaseCryptLib.h
    library_dir = os.path.abspath(os.path.join(
        SCRIPT_DIR, "..", "..", "Include", "Library"))
    if options.verbose:
        print(f"Looking for BaseCryptLib.h at: {library_dir}")
    if not os.path.exists(library_dir):
        raise FileNotFoundError("Unable to find BaseCryptLib.h")
    crypt_h_path = os.path.join(library_dir, "BaseCryptLib.h")
    tls_h_path = os.path.join(library_dir, "TlsLib.h")
    # we need somewhere to store the functions we found
    all_functions = []
    all_functions.extend(read_header_file(options, crypt_h_path))
    all_functions.extend(read_header_file(options, tls_h_path))

    return all_functions


def sort_functions(functions):
    ''' sorts the crypto/tls functions according to type '''
    if len(functions) == 0:
        return []
    sorted_functions = []
    for valid_type in functions[0].valid_types():
        funcs_of_type = list(
            filter(lambda x: x.get_type() == valid_type, functions))
        sorted_functions.append((valid_type, funcs_of_type))
    return sorted_functions


def generate_file_replacement(replacement_lines, input_path, output_filename, options, comment="//"):
    ''' generates a file at the output path, taking in input from the input path '''
    input_lines = []
    lines = []
    lines.extend([f"{comment} ****************************************************************************",
                  f"{comment} AUTOGENERATED BY " + REL_PATH,
                  f"{comment} DO NOT MODIFY"])
    lines.append(f"{comment} GENERATED ON: " + str(datetime.datetime.now()))
    lines.append("")

    if input_path is not None:
        input_file_path = os.path.abspath(os.path.join(SCRIPT_DIR, options.in_dir, input_path))
        if os.path.exists(input_file_path):
            input_file = open(input_file_path)
            input_lines = input_file.read().splitlines()
        else:
            print(f"Warning: failed to find {input_file_path}")

    start_index = 0
    for line in input_lines:
        start_index += 1
        if line  == "<!-- REPLACEMENT -->":
            break
        lines.append(line)
    
    # we've found the replacement
    lines.extend(replacement_lines)

    for line in input_lines[start_index:]:
        lines.append(line)

    output_path = os.path.join(options.out_dir, output_filename)
    out_file = open(output_path, "w")
    out_file.write("\n".join(lines))
    out_file.close()
    if options.verbose:
        print(f"Outputted to {output_path}")

def get_crypto_c(options, functions):
    ''' generates the c file that populates the struct defined by crypto.h '''
    print("Generating C file")
    lines = []
    
    sorted_functions = sort_functions(functions)
    
    # generate the function bodies
    for valid_type, funcs in sorted_functions:
        lines.append(
            "//=============================================================================")
        lines.append(f"//     {valid_type} functions")
        lines.append(
            "//=============================================================================")
        for func in funcs:
            lines.extend(func.comment)
            lines.append(f"// See {func.source}:{func.line_no}")
            lines.append(func.return_type)
            lines.append(f"CryptoService{func.name} (")
            lines.extend(func.params if len(func.params) > 0 else ["  VOID", ])
            lines.append("  )")
            lines.append("{")
            if (func.return_type == "VOID"):
                lines.append(
                    f"  CALL_VOID_BASECRYPTLIB ({func.name}, {func.get_param_tuple()});")
            else:
                lines.append(
                    f"  return CALL_BASECRYPTLIB ({func.name}, {func.get_param_tuple()}, {func.get_default_value()});")

            lines.append("}")

    # Generate the struct
    lines.append("\nconst EDKII_CRYPTO_PROTOCOL mEdkiiCrypto = {")
    lines.append("  /// Version")
    lines.append("  CryptoServiceGetCryptoVersion,")
    for valid_type, funcs in sorted_functions:
        lines.append(f"  // {valid_type} functions")
        for func in funcs:
            lines.append(f"  CryptoService{func.name},")
    lines.append("};")

    generate_file_replacement(lines, "Crypto.template.c", "temp_Crypto.c", options)


def get_crypto_h(options, functions):
    ''' generates the crypto.h header file - this is the defintion of the protocol and it's struct'''
    print("Generating H file")

    lines = []
    sorted_functions = sort_functions(functions)

    # Generate the function prototypes
    for valid_type, funcs in sorted_functions:
        lines.append(
            "//=============================================================================")
        lines.append(f"//     {valid_type} functions")
        lines.append(
            "//=============================================================================")
        for func in funcs:
            lines.extend(func.comment)
            lines.append(f"// FROM {func.source}:{func.line_no}")
            lines.append("typedef")
            lines.append(func.return_type)
            lines.append(f"(EFIAPI *{func.get_protocoled_name()}) (")
            lines.extend(func.params if len(func.params) > 0 else ["  VOID", ])
            lines.append("  );\n")

    # generate the protocol struct
    lines.append("\n")
    lines.append("///\n/// EDK II Crypto Protocol\n///")
    lines.append("struct _EDKII_CRYPTO_PROTOCOL {")
    lines.append(" // VERSION")
    lines.append(
        "  EDKII_CRYPTO_GET_VERSION                          GetVersion;")

    for valid_type, funcs in sorted_functions:
        lines.append(f"  // {valid_type}")
        for func in funcs:
            member_name = func.get_protocoled_name().ljust(49)
            lines.append(f"  {member_name} {func.name};")
    lines.append("};")

    generate_file_replacement(lines, "Crypto.template.h", "temp_Crypto.h", options)


def get_crypto_dsc(options, functions):
    ''' generates the flavors of the DSC '''
    print("Generating DSC file")
    flavors = {
        "MIN_PEI": {
            "families": ["HMACSHA256", "SHA1", "SHA256", "SHA384", "SHA512", "SM3"],
            "individuals": ["Pkcs5HashPassword"],
            "exclude": ["Sha256HashAll", "Sha1HashAll"]
        },
        "MIN_PEI_RSA": {
            "families": ["HMACSHA256", "SHA1", "SHA256", "SHA384", "SHA512", "SM3"],
            "individuals": ["RsaPkcs1Verify", "RsaNew", "RsaFree", "RsaSetKey", "Pkcs5HashPassword"],
            "exclude": ["Sha256HashAll", "Sha1HashAll"]
        },
        "MIN_DXE_MIN_SMM": {
            "families": ["HMACSHA1", "HMACSHA256", "PKCS", "SHA1", "SHA256", "RANDOM", "TLS", "TLSGET", "TLSSET"],
            "individuals": ["AesInit", "AesCbcEncrypt", "AesCbcDecrypt", "RsaPkcs1Verify", "RsaNew", "RsaFree", "RsaGetPublicKeyFromX509", "X509GetSubjectName", "X509GetCommonName", "X509GetOrganizationName", "X509GetTBSCert"],
            "exclude": ["Sha1HashAll", "Sha256HashAll", "Pkcs7Sign", "Pkcs7GetCertificatesList", "ImageTimestampVerify"]
        }
    }

    lines= []
    # Check to make sure crypto services is configured
    lines.append("[Defines]")
    all_flavors = "ALL NONE PACKAGE "+" ".join(list(flavors))
    lines.append("!ifndef CRYPTO_SERVICES")
    lines.append(" DEFINE CRYPTO_SERVICES = PACKAGE")
    lines.append("!endif")
    lines.append(f"!if $(CRYPTO_SERVICES) IN \"{all_flavors}\"")
    lines.append(" # we don't have a problem")
    lines.append("!else")
    lines.append(
        f" !error CRYPTO_SERVICES must be set to one of {all_flavors}.")
    lines.append("!endif")
    lines.append("")
    lines.append("[PcdsFixedAtBuild]")
    # get the functions sorted into a row
    sorted_functions = sort_functions(functions)
    # first the all flavor
    lines.append("!if $(CRYPTO_SERVICES) IN \"PACKAGE ALL\"")
    for function in functions:
        lines.append(f"  {function.get_pcd_name().ljust(70)}| TRUE")
    lines.append("!endif\n")
    # now we do the flavors
    for flavor in flavors:
        lines.append(f"!if $(CRYPTO_SERVICES) == {flavor}")
        families = flavors[flavor]["families"]
        exclude = flavors[flavor]["exclude"]
        for sort_type, funcs in sorted_functions:
            if sort_type not in families:
                continue
            lines.append(f"# {sort_type} family")
            for function in funcs:
                if function.name not in exclude:
                    lines.append(
                        f"  {function.get_pcd_name().ljust(70)}| TRUE")
        indiv = flavors[flavor]["individuals"]
        lines.append("# Individuals")
        for function in functions:
            if function.name in indiv and function.name not in exclude:
                lines.append(f"  {function.get_pcd_name().ljust(70)}| TRUE")
        lines.append("!endif\n")

    generate_file_replacement(lines, None, "Crypto.inc.dsc", options, "#")

def get_crypto_pcds(options, functions):
    ''' Generates the PCDs to be included in a DEC file '''
    print("Generating PCD file")
    sorted_functions = sort_functions(functions)
    lines = []
    lines.append("[PcdsFixedAtBuild]")
    index = int(options.pcd_index, 16)
    for valid_type, funcs in sorted_functions:
        lines.append(f"# {valid_type}")
        for func in funcs:
            lines.append(
                f"  {func.get_pcd_name()}|FALSE|BOOLEAN|{hex(index)}")
            index += 1

    generate_file_replacement(lines, None, "temp_crypto_pcd.inc.dec", options, "#")


def get_crypto_inf(options, functions):
    ''' Generates the list of PCD's to be included in an INF '''
    print("Generating INF file")
    lines = []
    lines.append("[Pcd]")
    for func in functions:
        lines.append(f"  {func.get_pcd_name().ljust(50)} # CONSUMES")
    generate_file_replacement(lines, None, "temp_crypto_pcd.inc.inf", options, "#")


if __name__ == "__main__":
    main()
