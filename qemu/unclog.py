#!/usr/bin/env python3

#
# (c) 2021-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

"""
  Unclogs a compiled shellcodes, with fills left as 0xff
  Aim: find 0xff sequences and replace them by valid emojis.
"""

import argparse
import random
import sys
import typing

min_emoji_len = 3
max_emoji_len = 340

EmojiList: typing.TypeAlias = list[typing.Tuple[str, str]]


def argparser() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Stage1 unclogger")
    parser.add_argument("-i", "--input", help="Input file", required=True)
    parser.add_argument("-o", "--output", help="Output file", required=True)
    parser.add_argument("-e", "--emoji", help="Emoji definitions file", required=True)
    parser.add_argument("-r", "--randomize", help="Randomize unclog step", action="store_true")
    parser.add_argument("-s", "--seed", help="Randomizer seed", type=int)
    return parser.parse_args()


def load_emoji(filename: str) -> EmojiList:
    emoji = {}
    with open(filename, "r", encoding="utf-8") as f:
        for line in f:
            split = line.split()
            hex_sequence = split[0]
            assert len(hex_sequence) >= 2 * min_emoji_len
            assert len(hex_sequence) <= 2 * max_emoji_len
            emoji[hex_sequence] = split[1]
    return list(emoji.items())


def gen_fill(fflen: int, randomize: bool, emojis: EmojiList) -> bytes:
    if (fflen < 3) or (fflen == 5):
        print(f"Cannot generate fill for {fflen} bytes", file=sys.stderr)
        sys.exit(1)
    fill = ""
    if not randomize:
        three = (-fflen) % 4
        four = ((fflen + 3) // 4) - three
        # print(f"Fill {three}*3 {four}*4 = {fflen}")
        assert three >= 0
        assert four >= 0
        assert 3 * three + 4 * four == fflen

        fill += ("⭕" * three) + ("🌑" * four)
    else:
        three = (-fflen) % 4
        four = ((fflen + 3) // 4) - three
        # print(f"Fill {three}*3 {four}*4 = {fflen}")
        assert three >= 0
        assert four >= 0
        assert 3 * three + 4 * four == fflen

        fill = ""
        while three > 0 or four > 0:
            random_key, random_emoji = random.choice(emojis)
            if (three > 0) and (len(random_key) == 6):
                fill += random_emoji
                three -= 1
            if (four > 0) and (len(random_key) == 8):
                fill += random_emoji
                four -= 1
    fill_encoded = fill.encode()
    # print(f"Fill fflen:{fflen} -> {len(fill_encoded)} ", fill_encoded)
    assert len(fill_encoded) == fflen
    return fill_encoded


def unclog(input_file: str, output_file: str, randomize: bool, emojis: EmojiList) -> None:
    with open(input_file, "rb") as in_f:
        data = bytearray(in_f.read())
        i = 0
        fflen = 0
        ffstart = 0
        for i in range(len(data)):  # pylint: disable=C0200
            if fflen > 0:
                if data[i] == 0xFF:
                    # Continuing of FF sequence
                    fflen += 1
                else:
                    # End of FF sequence
                    data[ffstart : ffstart + fflen] = gen_fill(fflen, randomize, emojis)
                    fflen = 0
            else:
                if data[i] == 0xFF:
                    # Start of FF sequence
                    ffstart = i
                    fflen += 1
        if fflen > 0:
            data[ffstart : ffstart + fflen] = gen_fill(fflen, randomize, emojis)

        with open(output_file, "wb") as out_f:
            out_f.write(data)


def main() -> None:
    args = argparser()
    emojis = load_emoji(args.emoji)
    if args.seed is not None:
        random.seed(args.seed)
    unclog(args.input, args.output, args.randomize, emojis)


if __name__ == "__main__":
    main()
