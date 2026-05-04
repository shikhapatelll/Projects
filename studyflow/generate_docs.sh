#!/usr/bin/env bash
set -e
doxygen dox.config
echo "Doxygen HTML generated under docs/html/"
