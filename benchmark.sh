#!/bin/sh -uex

strip --strip-all ray
eval $(size ray-single | awk '/ray-single/ { print "export TEXT=" $1 " DATA=" $2 " BSS=" $3 }')

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
        "name": "Benchmark - text Size",
        "unit": "Bytes",
        "value": ${TEXT}
    },
    {
        "name": "Benchmark - data Size",
        "unit": "Bytes",
        "value": ${DATA}
    },
    {
        "name": "Benchmark - bss Size",
        "unit": "Bytes",
        "value": ${BSS}
    },
    {
        "name": "Benchmark - CPU Cycles",
        "unit": "Cycles",
        "value": ${CYCLES}
    }
]
EOF
