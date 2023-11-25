#!/usr/bin/env bash

NAME="risk"

CONFIG="${CONFIG:-$NAME}"

CONFIG_FILE="config/test.toml"

echo "CONFIG_FILE=$CONFIG_FILE"

# debug?

if [ "$1" == "debug" ]; then
  KERNEL="$(uname -a)"
  case "$KERNEL" in
    Linux*)
      PREFIX="gdb --args"
      ;;
    Darwin*)
      PREFIX="lldb --"
      ;;
  esac
  shift 1
else
  PREFIX=
fi

$PREFIX "./roq-fix-auth" \
  --name "$NAME" \
  --config_file "$CONFIG_FILE" \
  --client_listen_address "ws://localhost:1234" \
  $@
