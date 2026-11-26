
#!/usr/bin/env python3

import argparse
import sys
from pathlib import Path


WORD_SIZE = 2


def convert_binary_to_hex(input_path: Path, output_path: Path, label: str):
    if not input_path.exists():
        raise FileNotFoundError(f"{label} file not found: {input_path}")

    raw = input_path.read_bytes()

    if len(raw) % WORD_SIZE != 0:
        raise ValueError(
            f"{label}: invalid file size ({len(raw)} bytes). "
            "A 16-bit binary file must contain an even number of bytes."
        )

    words = []

    for i in range(0, len(raw), WORD_SIZE):
        # SAC16 uses 16-bit Big Endian words
        word = int.from_bytes(raw[i:i + WORD_SIZE], byteorder="big")
        words.append(f"{word:04X}")

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(
        "\n".join(words) + ("\n" if words else ""),
        encoding="ascii"
    )

    print(f"[OK] {label}")
    print(f"     Input : {input_path}")
    print(f"     Output: {output_path}")
    print(f"     Words : {len(words)}")


def main():
    parser = argparse.ArgumentParser(
        description="Convert SAC16 16-bit binary files to Verilog HEX files."
    )

    parser.add_argument(
        "--bin",
        required=True,
        type=Path,
        help="Instruction binary file"
    )

    parser.add_argument(
        "--data",
        required=True,
        type=Path,
        help="Data binary file"
    )

    parser.add_argument(
        "--out-dir",
        default=Path("."),
        type=Path,
        help="Output directory (default: current directory)"
    )

    args = parser.parse_args()

    try:
        args.out_dir.mkdir(parents=True, exist_ok=True)

        convert_binary_to_hex(
            args.bin,
            args.out_dir / "program.hex",
            "Instruction memory"
        )

        convert_binary_to_hex(
            args.data,
            args.out_dir / "data.hex",
            "Data memory"
        )

        print("\nConversion completed successfully.")

    except (OSError, ValueError) as error:
        print(f"\n[ERROR] {error}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()

