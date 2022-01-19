#!/bin/sh -uex

strip --strip-all ray

export WIDTH=64 HEIGHT=64
CYCLES=$(valgrind --tool=cachegrind ./ray-single 2>&1 | awk '/I   refs:/ { gsub(",", "", $4); print $4 }')

cat << EOF
[
    {
        "name": "Benchmark - Executable Size",
        "unit": "Bytes",
        "value": $(stat --format=%s ray)
    },
    {
        "name": "Benchmark - CPU Cycles",
        "unit": "Cycles",
        "value": ${CYCLES}
    }
]
EOF
