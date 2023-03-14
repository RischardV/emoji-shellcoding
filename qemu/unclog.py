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

EmojiList: typing.TypeAlias = list[tuple[str, str]]
EmojiListByLen: typing.TypeAlias = dict[int, EmojiList]
MinifyChoice: typing.TypeAlias = dict[int, list[list[int]]]


def argparser() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Stage1 unclogger")
    parser.add_argument("-i", "--input", help="Input file", required=True)
    parser.add_argument("-o", "--output", help="Output file", required=True)
    parser.add_argument("-e", "--emoji", help="Emoji definitions file", required=True)
    parser.add_argument("-m", "--minify", help="Use the least possible number of emojis", action="store_true")
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


class Unclog:
    """The core work of unclogging"""

    def __init__(self, randomize: bool, minify: bool, emojis: EmojiList):
        self.randomize = randomize
        self.minify = minify
        self.emojis = emojis

        self.emoji_by_length = self.prebuild_emoji_by_length()
        self.minify_choice = self.prebuild_minify()

    def prebuild_emoji_by_length(self) -> EmojiListByLen:
        emoji_by_length: EmojiListByLen = {}
        for key, emoji in self.emojis:
            length = len(key) // 2
            if length not in emoji_by_length:
                emoji_by_length[length] = []
            emoji_by_length[length].append((key, emoji))
        emoji_by_length = dict(sorted(emoji_by_length.items()))  # Sort by key
        return emoji_by_length

    def prebuild_minify(self) -> MinifyChoice:
        minify_choice: MinifyChoice = {}
        emoji_lengths = list(self.emoji_by_length.keys())
        for i in range(len(emoji_lengths)):  # pylint: disable=C0200
            key1 = emoji_lengths[i]
            minify_choice[key1] = [[key1]]
            for j in range(i, len(emoji_lengths)):
                key2 = emoji_lengths[j]
                ksum = key1 + key2
                elem = [key1, key2]
                if (ksum not in minify_choice) or (len(elem) < len(minify_choice[ksum][0])):
                    minify_choice[ksum] = [elem]
                elif len(elem) == len(minify_choice[ksum][0]):
                    minify_choice[ksum].append(elem)
        return minify_choice

    def gen_fill(self, fflen: int) -> bytes:  # pylint: disable=R0912,R0914
        if (fflen < 3) or (fflen == 5):
            print(f"Cannot generate fill for {fflen} bytes", file=sys.stderr)
            sys.exit(1)
        fill = ""
        if self.minify:
            remain_fflen = fflen
            if fflen not in self.minify_choice:
                highest_emoji_len = max(self.emoji_by_length)
                big = 2 * highest_emoji_len
                while remain_fflen > big:
                    fill += self.gen_fill(highest_emoji_len).decode("utf-8")
                    remain_fflen -= highest_emoji_len
                if remain_fflen not in self.minify_choice:
                    print(f"Unhandled minify len {fflen}", file=sys.stderr)
                    sys.exit(1)
            len_possibilities = self.minify_choice[remain_fflen]
            if self.randomize:
                len_list_pick = random.choice(len_possibilities)
                random.shuffle(len_list_pick)
            else:
                len_list_pick = len_possibilities[0]

            for pick in len_list_pick:
                if self.randomize:
                    _, emoji = random.choice(self.emoji_by_length[pick])
                else:
                    _, emoji = self.emoji_by_length[pick][0]
                fill += emoji
        else:
            three = (-fflen) % 4
            four = ((fflen + 3) // 4) - three
            # print(f"Fill {three}*3 {four}*4 = {fflen}")
            assert three >= 0
            assert four >= 0
            assert 3 * three + 4 * four == fflen

            if self.randomize:
                while three > 0 or four > 0:
                    random_key, random_emoji = random.choice(self.emojis)
                    if (three > 0) and (len(random_key) == 6):
                        fill += random_emoji
                        three -= 1
                    if (four > 0) and (len(random_key) == 8):
                        fill += random_emoji
                        four -= 1
            else:
                fill += ("⭕" * three) + ("🌑" * four)
        fill_encoded = fill.encode()
        # print(f"Fill fflen:{fflen} -> {len(fill_encoded)} ", fill_encoded)
        assert len(fill_encoded) == fflen
        return fill_encoded

    def process(self, input_file: str, output_file: str) -> tuple[int, int]:
        bytes_unclogged = 0
        with open(input_file, "rb") as in_f:
            data = bytearray(in_f.read())
            all_bytes = len(data)
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
                        data[ffstart : ffstart + fflen] = self.gen_fill(fflen)
                        bytes_unclogged += fflen
                        fflen = 0
                else:
                    if data[i] == 0xFF:
                        # Start of FF sequence
                        ffstart = i
                        fflen += 1
            if fflen > 0:
                data[ffstart : ffstart + fflen] = self.gen_fill(fflen)
                bytes_unclogged += fflen

            with open(output_file, "wb") as out_f:
                out_f.write(data)
        return (bytes_unclogged, all_bytes)


def main() -> None:
    args = argparser()
    emojis = load_emoji(args.emoji)
    if args.seed is not None:
        random.seed(args.seed)
    unclog = Unclog(args.randomize, args.minify, emojis)
    bytes_unclogged, all_bytes = unclog.process(args.input, args.output)
    print(f"Unclogged {bytes_unclogged}/{all_bytes} bytes from {args.input} to {args.output}")


if __name__ == "__main__":
    main()
