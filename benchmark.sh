#!/bin/sh -uex

U=artifact-ubuntu-latest
W=artifact-windows-latest
M=artifact-macos-latest

eval $(size "$U/ray-single" | awk '/ray-single/ { print "export TEXT=" $1 " DATA=" $2 " BSS=" $3 }')

export WIDTH=64 HEIGHT=64
chmod +x "$U/ray-single"
CYCLES=$(valgrind --tool=cachegrind "$U/ray-single" 2>&1 | awk '/I   refs:/ { gsub(",", "", $4); print $4 }')

cat << EOF
[
    {
        "name": "Benchmark - Executable Size",
        "unit": "Bytes",
        "value": $(stat --format=%s "$U/ray.vndh")
    },
    {
        "name": "Benchmark - Win32 Executable Size",
        "unit": "Bytes",
        "value": $(stat --format=%s "$W/MinSizeRel/ray.exe")
    },
    {
        "name": "Benchmark - MacOS Executable Size",
        "unit": "Bytes",
        "value": $(stat --format=%s "$M/ray")
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
