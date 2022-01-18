#!/bin/sh -uex

strip --strip-all ray

cat << EOF
[
    {
        "name": "Benchmark - Executable Size",
        "unit": "Bytes",
        "value": $(stat --format=%s ray)
    }
]
EOF
