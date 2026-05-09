from dataclasses import dataclass
from typing import List
import numpy as np
import struct
import zlib

MAGIC = bytes(
    [
        0xB2,
        0x28,
        0x09,
        0x5B,
        0x30,
        0x52,
        0x45,
        0x39,
        0x83,
        0xB3,
        0x84,
        0x76,
        0x1C,
        0x9E,
        0x02,
        0x59,
    ]
)


@dataclass
class DataPoint:
    timestamp: int
    values: List[float]


def q_uncompress(data: bytes) -> bytes:
    """
    Replicates Qt's qUncompress.
    Qt adds a 4-byte big-endian expected length header before the zlib stream.
    """
    if len(data) < 4:
        return b""
    # We skip the first 4 bytes (expected length) and decompress the rest
    return zlib.decompress(data[4:])


def read_qperf_datapoints(file_path: str) -> List[DataPoint]:
    with open(file_path, "rb") as f:
        # 1. Verify Magic
        magic_check = f.read(len(MAGIC))
        if magic_check != MAGIC:
            raise ValueError(
                f"Invalid magic! Expected {MAGIC.hex('.')}, got {magic_check.hex('.')}"
            )

        # 2. Read and Uncompress the rest
        compressed_payload = f.read()
        decompressed_data = q_uncompress(compressed_payload)

        # 3. Parse Data Stream
        # QDataStream uses Big Endian by default
        offset = 0

        # Read nDataPoints (qint64 -> 8 bytes)
        n_datapoints = struct.unpack_from(">q", decompressed_data, offset)[0]
        offset += 8

        # Read nValuesPerPoint (qint64 -> 8 bytes)
        n_values_per_point = struct.unpack_from(">q", decompressed_data, offset)[0]
        offset += 8

        datapoints = []

        for _ in range(n_datapoints):
            # Read timestamp (qint64 -> 8 bytes)
            timestamp = struct.unpack_from(">q", decompressed_data, offset)[0]
            offset += 8

            values = []
            for _ in range(n_values_per_point):
                # Read double (8 bytes)
                val = struct.unpack_from(">d", decompressed_data, offset)[0]
                offset += 8
                values.append(val)

            datapoints.append(DataPoint(timestamp=timestamp, values=values))

        return datapoints
