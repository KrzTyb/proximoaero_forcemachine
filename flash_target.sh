#!/bin/bash
set -e

TARGET_IP=10.42.0.215

SRC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR=${SRC_DIR}/build/arm-debug

scp ${BUILD_DIR}/main/Proximo_Aero_ForceMachine root@${TARGET_IP}:/usr/bin