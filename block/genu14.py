#!/usr/bin/env python3

#
# (c) 2021-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

import collections
import colorsys
import csv
import math
import os
import pathlib
import shutil
import sys
import time
import typing

import certifi  # type:ignore
import urllib3  # type:ignore
from PIL import Image  # type:ignore


def get_web_file(url: str, filename: str) -> None:
    c = urllib3.PoolManager(cert_reqs="CERT_REQUIRED", ca_certs=certifi.where())
    with c.request("GET", url, preload_content=False) as resp, open(filename, "wb") as out_file:
        shutil.copyfileobj(resp, out_file)
    resp.release_conn()


def emoji_base_uri() -> str:
    return "https://unicode.org/Public/emoji/14.0/"


def get_emoji_sequence_file(filename: str) -> None:
    get_web_file(emoji_base_uri() + "emoji-sequences.txt", filename)


def get_emoji_test_file(filename: str) -> None:
    get_web_file(emoji_base_uri() + "emoji-test.txt", filename)


Sequences: typing.TypeAlias = list[str]


def gen_sequences(sequences_files: list[str], build_dir: str) -> Sequences:
    start = time.time()
    sequences_set = set()
    for sequences_file in sequences_files:
        with open(sequences_file, encoding="utf-8") as csvfile:
            reader = csv.reader(filter(lambda row: (row[0] != "#" and len(row) > 1), csvfile), delimiter=";")
            for row in reader:
                if row[1].strip().startswith("unqualified"):
                    continue
                if row[1].strip().startswith("component"):
                    continue
                sequence = row[0].strip()
                if ".." in sequence:
                    if " " in sequence:
                        print("Error: have both '..' and ' ' in a sequence")
                        sys.exit(1)
                    iters = sequence.split(".")
                    for codepoint in range(int(iters[0], 16), int(iters[2], 16) + 1):
                        sequences_set.add(chr(codepoint))
                else:
                    sequence_list = [chr(int(x, 16)) for x in sequence.split(" ")]
                    sequences_set.add("".join(sequence_list))

    sequences = sorted(sequences_set)

    with open(build_dir + "/seq.txt", "wb") as f:
        for a in sequences:
            f.write(a.encode().hex().encode())
            f.write((" " + a + "\n").encode())

    with open(build_dir + "/texlist.txt", "wb") as f:
        for a in sequences:
            f.write(("{" + a + "}").encode())

    print(f"Parsed {len(sequences)} sequences in {time.time() - start:0.4f}s")
    return sequences


def gen_pairs_bin(sequences: Sequences) -> bytes:
    start = time.time()
    s = []
    for a in sequences:
        for b in sequences:
            s.append(a + b)
    pairs = "".join(s).encode()
    end = time.time()
    print(f"Generated binary data in {end - start:0.4f}s (len:{len(s)})")
    return pairs


def gen_pairs_quads(sequences: Sequences) -> tuple[dict[int, int], list[int]]:
    m = gen_pairs_bin(sequences)
    print("Generating pairs and quads (this will take a while)...")
    start = time.time()
    pairs: collections.defaultdict[int, int] = collections.defaultdict(int)
    quads = set()

    for i in range(0, len(m) - 1):
        num2 = m[i] + ((m[i + 1]) << 8)
        if num2 % 4 != 3:
            pairs[num2] += 1
        elif i + 3 < len(m):
            if (num2 & 0b11100) != 0b11100:
                num4 = num2 + (m[i + 2] << 16) + (m[i + 3] << 24)
                quads.add(num4)

    pairs_dict = dict(sorted(pairs.items()))
    quads_list = sorted(quads)

    end = time.time()
    print(f"Generated {len(pairs)} pairs and {len(quads)} quads in {end - start:0.1f}s")
    return pairs_dict, quads_list


def save_pair_graphics(pairs: dict[int, int], build_dir: str) -> None:
    pairs_occurs_max = max(pairs.values())
    img = Image.new("RGB", (256, 256), color="black")
    for key in pairs:
        x = key % 256
        y = key // 256
        i = math.log2(pairs[key]) / math.log2(pairs_occurs_max)
        color = colorsys.hsv_to_rgb(i * 300.0, 1.0, 1.0)
        color = (int(color[0] * 255), int(color[1] * 255), int(color[2] * 255))
        # print(color)
        img.putpixel((x, y), color)
    img.save(build_dir + "/pairs.png")


def main() -> None:
    build_dir = "build"
    pathlib.Path(build_dir).mkdir(parents=True, exist_ok=True)

    emoji_sequences_filename = build_dir + "/emoji-sequences.txt"
    if not os.path.isfile(emoji_sequences_filename):
        get_emoji_sequence_file(emoji_sequences_filename)

    emoji_test_filename = build_dir + "/emoji-test.txt"
    if not os.path.isfile(emoji_test_filename):
        get_emoji_test_file(emoji_test_filename)

    sequences = gen_sequences([emoji_sequences_filename, emoji_test_filename], build_dir)

    pairs, quads = gen_pairs_quads(sequences)

    save_pair_graphics(pairs, build_dir)

    with open(build_dir + "/diss.bin", "wb") as f:
        for a in pairs:
            f.write(bytes([a % 256]))
            f.write(bytes([a // 256]))
        for a in quads:
            for _ in range(4):
                f.write(bytes([a % 256]))
                a //= 256


if __name__ == "__main__":
    main()
