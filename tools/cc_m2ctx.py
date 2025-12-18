#!/usr/bin/env python3

"""
Context generator for m2c decompilation
Preprocesses C files and creates context for the MIPS decompiler
"""

import argparse
import os
import sys
import subprocess
import tempfile
from pathlib import Path

# Get project root directory
script_dir = Path(__file__).parent.absolute()
root_dir = script_dir.parent if script_dir.name == "tools" else script_dir
src_dir = root_dir / "src"

# Project-specific preprocessor flags
# CUSTOMIZE THESE for your Chrono Cross project
CPP_FLAGS = [
    "-Iinclude",           # Your include directory
    "-Isrc",               # Source directory
    "-D_LANGUAGE_C",       # MIPS/PSY-Q convention
    "-D_MIPS_SZLONG=32",   # 32-bit longs
    "-ffreestanding",      # Freestanding environment
    "-DM2CTX",             # Flag to indicate we're building context
    # Add any other project-specific defines here
    # Examples:
    # "-DVERSION_US",
    # "-DPLATFORM_PSX",
]

def get_compiler():
    """Detect platform and return appropriate GCC compiler"""
    if sys.platform in ("linux", "linux2"):
        return "gcc", True  # compiler, delete_temp
    elif sys.platform == "win32":
        # Adjust this path to your Windows GCC location
        return "tools/win-gcc/bin/gcc.exe", False
    elif sys.platform == "darwin":
        return "gcc", True
    else:
        return "gcc", True

def preprocess_c_file(c_file: Path) -> str:
    """
    Preprocess a C file and extract context for m2c
    
    Returns the preprocessed code as a string
    """
    # Make path relative to root
    rel_path = c_file.relative_to(root_dir) if c_file.is_absolute() else c_file
    
    cpp, delete_temp = get_compiler()
    
    # First, get stock macros (standard library defines we want to exclude)
    with tempfile.NamedTemporaryFile(suffix=".c", delete=delete_temp) as tmp:
        try:
            stock_macros = subprocess.check_output(
                [cpp, "-E", "-P", "-dM", tmp.name],
                cwd=root_dir,
                encoding="utf-8",
                stderr=subprocess.DEVNULL
            )
        except subprocess.CalledProcessError as e:
            print(f"Warning: Could not get stock macros: {e}", file=sys.stderr)
            stock_macros = ""
    
    # Now preprocess the actual file
    cpp_command = [cpp, "-E", "-P", "-dD", *CPP_FLAGS, str(rel_path)]
    
    try:
        preprocessed = subprocess.check_output(
            cpp_command,
            cwd=root_dir,
            encoding="utf-8",
            stderr=subprocess.PIPE
        )
    except subprocess.CalledProcessError as e:
        print(
            f"Failed to preprocess {c_file}\n"
            f"Command: {' '.join(cpp_command)}\n"
            f"Error: {e.stderr}",
            file=sys.stderr
        )
        sys.exit(1)
    
    if not preprocessed:
        print("Error: Preprocessor output is empty", file=sys.stderr)
        sys.exit(1)
    
    # Parse the preprocessed output
    defines = {}
    source_lines = []
    
    for line in preprocessed.splitlines(keepends=True):
        if line.startswith("#define"):
            # Extract macro name
            parts = line.split(None, 2)
            if len(parts) >= 2:
                sym = parts[1].split("(")[0]
                defines[sym] = line
        elif line.startswith("#undef"):
            parts = line.split(None, 1)
            if len(parts) >= 2:
                sym = parts[1].strip()
                defines.pop(sym, None)
        else:
            # Filter out inline assembly
            if "__asm__" in line or "asm volatile" in line:
                continue
            source_lines.append(line)
    
    # Remove stock macros from our defines
    stock_macro_names = set()
    for line in stock_macros.strip().splitlines():
        parts = line.split(None, 2)
        if len(parts) >= 2:
            sym = parts[1].split("(")[0]
            stock_macro_names.add(sym)
    
    # Filter out stock macros
    filtered_defines = {
        sym: line for sym, line in defines.items() 
        if sym not in stock_macro_names
    }
    
    # Combine defines and source
    output = "".join(filtered_defines.values()) + "".join(source_lines)
    
    return output

def main():
    parser = argparse.ArgumentParser(
        description="Create context file for m2c decompilation"
    )
    parser.add_argument(
        "c_file",
        type=Path,
        help="C source file to create context from"
    )
    parser.add_argument(
        "-o", "--output",
        type=Path,
        default=None,
        help="Output file (default: ctx.c in project root)"
    )
    
    args = parser.parse_args()
    
    # Validate input file
    if not args.c_file.exists():
        print(f"Error: File not found: {args.c_file}", file=sys.stderr)
        sys.exit(1)
    
    # Generate context
    print(f"Generating context from {args.c_file}...")
    context = preprocess_c_file(args.c_file)
    
    # Determine output path
    output_path = args.output if args.output else (root_dir / "ctx.c")
    
    # Write output
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(context)
    
    print(f"Context written to {output_path}")
    print(f"Context size: {len(context)} bytes")

if __name__ == "__main__":
    main()
