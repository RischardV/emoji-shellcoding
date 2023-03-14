#!/usr/bin/env python3

#
# (c) 2021-2022 Hadrien Barral
# SPDX-License-Identifier: Apache-2.0
#

import binascii
import sys


def main() -> None:
    print(binascii.unhexlify(sys.argv[1]).decode("utf-8"))


if __name__ == "__main__":
    main()
