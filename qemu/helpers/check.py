#!/usr/bin/env python3

#
# (c) 2021-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

import argparse
import sys
import typing

min_emoji_len = 3
max_emoji_len = 40

EmojiDict: typing.TypeAlias = dict[str, bool]


def argparser() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Stage1 unclogger")
    parser.add_argument("-i", "--input", help="Input file", required=True)
    parser.add_argument("-e", "--emoji", help="Emoji definitions file", required=True)
    return parser.parse_args()


def load_emoji(filename: str) -> EmojiDict:
    emoji = {}
    with open(filename, "r", encoding="utf-8") as f:
        for line in f:
            hex_sequence = line.split()[0]
            assert len(hex_sequence) >= 2 * min_emoji_len
            assert len(hex_sequence) <= 2 * max_emoji_len
            emoji[hex_sequence] = True
    return emoji


def check_file(filename: str, emojis: EmojiDict) -> bool:
    with open(filename, "rb") as f:
        data = f.read()
        i = 0
        while i < len(data):
            l = max_emoji_len
            while l >= min_emoji_len:
                if data[i : i + l].hex() in emojis:
                    break
                l -= 1
            if l < min_emoji_len:
                print(f"Bad sequence in {filename} at offset {i} (0x{i:06X})", file=sys.stderr)
                return False
            i += l
    return True


def main() -> None:
    args = argparser()
    emojis = load_emoji(args.emoji)
    if not check_file(args.input, emojis):
        sys.exit(1)


if __name__ == "__main__":
    main()
