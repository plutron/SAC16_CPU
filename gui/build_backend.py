from pathlib import Path
import subprocess
import sys

GUI_DIR = Path(__file__).resolve().parent
ROOT = GUI_DIR.parent
SIM = ROOT / "simulator"
ASM = ROOT / "assembler"
BUILD = GUI_DIR / "build"
BUILD.mkdir(exist_ok=True)

def run(command):
    print("Running:", " ".join(map(str, command)))
    subprocess.run([str(x) for x in command], check=True)

def build():
    gcc = "gcc"
    asm_sources = [ASM / "compiler" / "compiler.c"]
    asm_sources += sorted((ASM / "compiler" / "src").glob("*.c"))
    asm_exe = BUILD / "sac16_asm"
    run([
        gcc, "-std=c11", "-D_DEFAULT_SOURCE", "-include", "strings.h",
        "-O2", "-Wall", "-Wextra", "-Wpedantic",
        "-I", ASM / "compiler" / "include",
        *asm_sources, "-o", asm_exe
    ])

    sim_sources = sorted((SIM / "src").glob("*.c"))
    library = BUILD / "libsac16.so"
    run([
        gcc, "-std=gnu11", "-O2", "-Wall", "-Wextra", "-Wpedantic",
        "-fPIC", "-shared", "-I", SIM / "include",
        *sim_sources, GUI_DIR / "bridge.c", "-o", library
    ])
    print("\\nBuild completed.")
    print("Assembler:", asm_exe)
    print("Backend:  ", library)

if __name__ == "__main__":
    try:
        build()
    except subprocess.CalledProcessError as exc:
        print("\\nBuild failed:", exc, file=sys.stderr)
        sys.exit(1)
