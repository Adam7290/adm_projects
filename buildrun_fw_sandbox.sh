#!/bin/sh
cmake --build ./ && (cd adm_framework/ && ./adm_framework_sandbox)
