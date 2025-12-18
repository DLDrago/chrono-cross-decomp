#!/usr/bin/env python3

"""
Example: Complete decompilation workflow
Shows how the pieces fit together
"""

from pathlib import Path
import cc_m2ctx as m2ctx
import tempfile
import subprocess

def example_workflow():
    """
    Demonstrates the complete decompilation process
    """
    
    print("=" * 80)
    print("CHRONO CROSS DECOMPILATION WORKFLOW EXAMPLE")
    print("=" * 80)
    print()
    
    # Step 1: You have an assembly file you want to decompile
    asm_file = Path("asm/slps_023.64/nonmatchings/system/sound/func_8004BDB8.s")
    print(f"Step 1: Target assembly file")
    print(f"  {asm_file}")
    print()
    
    # Step 2: Find the corresponding C file
    c_file = Path("src/slps_023.64/system/sound.c")
    print(f"Step 2: Corresponding C source")
    print(f"  {c_file}")
    print()
    
    # Step 3: Generate context from the C file
    print(f"Step 3: Generate context")
    print(f"  This preprocesses {c_file} with all includes and defines")
    print(f"  Running: m2ctx.preprocess_c_file(c_file)")
    
    # Uncomment to actually run:
    context = m2ctx.preprocess_c_file(c_file)
    print(f"  Generated {len(context)} bytes of context")

    with open("ctx.c", "w", encoding="utf-8") as f:
        f.write(context)
    
    print()
    
    # Step 4: Run m2c with the context
    print(f"Step 4: Run m2c")
    print(f"  Command would be something like:")
    print(f"    python -m m2c \\")
    print(f"      --context ctx.c \\")
    print(f"      --target mipsel-gcc-c \\")
    print(f"      {asm_file}")
    print()
    
    # Step 5: Use the output
    print(f"Step 5: Use the decompiled code")
    print(f"  - Copy the output into your .c file")
    print(f"  - Replace INCLUDE_ASM(...) macro")
    print(f"  - Compile and test")
    print(f"  - Iterate if needed")
    print()
    
    print("=" * 80)
    print("QUICK START")
    print("=" * 80)
    print()
    print("To decompile a function, just run:")
    print()
    print("  python tools/cc_decompile.py asm/path/to/func.s")
    print()
    print("The script handles steps 1-4 automatically!")
    print()
    
    print("=" * 80)
    print("WHAT THE CONTEXT DOES")
    print("=" * 80)
    print()
    print("The context file contains:")
    print("  • All type definitions (structs, enums, typedefs)")
    print("  • Function declarations")
    print("  • Macro definitions")
    print("  • Global variable declarations")
    print()
    print("This helps m2c:")
    print("  • Choose correct types for variables")
    print("  • Recognize function calls")
    print("  • Understand struct member access")
    print("  • Generate cleaner code")
    print()
    
    print("=" * 80)
    print("CUSTOMIZATION CHECKLIST")
    print("=" * 80)
    print()
    print("Before using these scripts, customize:")
    print()
    print("1. cc_m2ctx.py:")
    print("   [ ] Update CPP_FLAGS with your include paths")
    print("   [ ] Add any project-specific defines")
    print()
    print("2. cc_decompile.py:")
    print("   [ ] Update find_c_file_for_asm() for your project structure")
    print("   [ ] Adjust m2c options (indentation, style, etc.)")
    print()
    print("3. Test with a simple function first!")
    print()

if __name__ == "__main__":
    example_workflow()
