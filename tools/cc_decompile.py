#!/usr/bin/env python3

"""
Simple decompiler for Chrono Cross
Uses m2ctx to generate context and m2c to decompile assembly functions
"""

import argparse
import io
import subprocess
import sys
import tempfile
from contextlib import redirect_stdout
from pathlib import Path

# Import the context generator we created
import cc_m2ctx as m2ctx

# Add m2c from tools directory to Python path
script_dir = Path(__file__).parent.absolute()
m2c_path = script_dir / "m2c"  # Adjust if m2c is elsewhere
sys.path.insert(0, str(m2c_path))

# Try to import m2c
try:
    import m2c.main as m2c
    HAS_M2C = True
except ImportError:
    HAS_M2C = False
    print("Warning: m2c not found. Install with: pip install m2c-kit")
    print("Or clone from: https://github.com/matt-kempster/m2c")

def find_c_file_for_asm(asm_path: Path) -> Path:
    """
    Try to find the corresponding C file for an assembly file
    
    Assumes structure like:
    asm/path/to/nonmatchings/file/func_name.s
    src/path/to/file.c
    """
    parts = asm_path.parts
    
    # Try to find 'nonmatchings' in the path
    if 'nonmatchings' in parts:
        idx = parts.index('nonmatchings')
        # Get the path up to nonmatchings
        base_parts = parts[1:idx]  # Skip 'asm'
        # Get the filename (should be second-to-last or determined by pattern)
        if idx + 1 < len(parts):
            c_filename = parts[idx + 1] + ".c"
        else:
            c_filename = parts[-1].replace('.s', '.c')
        
        # Build expected C path
        c_path = Path("src") / Path(*base_parts) / c_filename
        
        if c_path.exists():
            return c_path
    
    # Fallback: try to guess based on asm structure
    # This might need adjustment based on your project layout
    print(f"Warning: Could not automatically find C file for {asm_path}")
    print("Please adjust the path detection logic in find_c_file_for_asm()")
    return None

def run_m2c_decompile(asm_file: Path, context: str) -> str:
    """
    Run m2c on an assembly file with the given context
    
    Returns the decompiled C code
    """
    if not HAS_M2C:
        print("Error: m2c is required but not installed", file=sys.stderr)
        sys.exit(1)
    
    # Write context to a temporary file
    with tempfile.NamedTemporaryFile(
        mode="w", 
        encoding="utf-8", 
        suffix=".c",
        delete=False
    ) as tmp_ctx:
        tmp_ctx.write(context)
        tmp_ctx.flush()
        ctx_path = tmp_ctx.name
    
    try:
        # Configure m2c options
        # Customize these flags for your project
        options = m2c.parse_flags([
            "-P", "4",                      # Pointer indentation
            "--pointer-style", "left",      # Type* ptr vs Type *ptr
            "--comment-style", "oneline",   # // comments vs /* */
            "--target", "mipsel-gcc-c",     # MIPS little-endian GCC
            "--context", ctx_path,          # Context file
            str(asm_file),                  # Assembly to decompile
        ])
        
        # Capture m2c output
        output_buffer = io.StringIO()
        with redirect_stdout(output_buffer):
            m2c.run(options)
        
        return output_buffer.getvalue()
    
    finally:
        # Clean up temp file
        Path(ctx_path).unlink(missing_ok=True)

def decompile_function(asm_file: Path, c_file: Path = None, output: Path = None):
    """
    Decompile a single function
    
    Args:
        asm_file: Path to .s assembly file
        c_file: Path to corresponding .c file (auto-detected if not provided)
        output: Path to write decompiled code (stdout if not provided)
    """
    # Validate assembly file
    if not asm_file.exists():
        print(f"Error: Assembly file not found: {asm_file}", file=sys.stderr)
        sys.exit(1)
    
    # Find or validate C file
    if c_file is None:
        c_file = find_c_file_for_asm(asm_file)
        if c_file is None:
            print("Error: Could not find C file. Please specify with -c", file=sys.stderr)
            sys.exit(1)
    elif not c_file.exists():
        print(f"Error: C file not found: {c_file}", file=sys.stderr)
        sys.exit(1)
    
    print(f"Assembly: {asm_file}")
    print(f"C file:   {c_file}")
    print()
    
    # Generate context
    print("Generating context...")
    context = m2ctx.preprocess_c_file(c_file)
    print(f"Context size: {len(context)} bytes")
    print()
    
    # Run m2c
    print("Running m2c...")
    decompiled = run_m2c_decompile(asm_file, context)
    
    if not decompiled:
        print("Error: m2c produced no output", file=sys.stderr)
        sys.exit(1)
    
    # Output results
    if output:
        with open(output, "w", encoding="utf-8") as f:
            f.write(decompiled)
        print(f"Decompiled code written to: {output}")
    else:
        print("=" * 80)
        print("DECOMPILED CODE:")
        print("=" * 80)
        print(decompiled)
        print("=" * 80)

def main():
    parser = argparse.ArgumentParser(
        description="Decompile PS1 MIPS functions for Chrono Cross"
    )
    parser.add_argument(
        "asm_file",
        type=Path,
        help="Assembly file (.s) to decompile"
    )
    parser.add_argument(
        "-c", "--c-file",
        type=Path,
        help="Corresponding C source file (auto-detected if not provided)"
    )
    parser.add_argument(
        "-o", "--output",
        type=Path,
        help="Output file (stdout if not provided)"
    )
    
    args = parser.parse_args()
    
    decompile_function(
        asm_file=args.asm_file,
        c_file=args.c_file,
        output=args.output
    )

if __name__ == "__main__":
    main()
