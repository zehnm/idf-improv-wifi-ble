#!/usr/bin/env bash
#
# Based on https://docs.espressif.com/projects/esp-idf/en/latest/esp32/contribute/style-guide.html
#

set -euo pipefail

if [ ! -x "$(command -v astyle_py)" ]; then
	echo "astyle_py not found, please install astyle_py:"
	echo "  pip install -U astyle_py==VERSION"
	echo "where VERSION is the same as in .pre-commit-config.yaml."
	exit 1
fi

astyle_py --astyle-version=3.4.7 \
	--style=otbs \
	--attach-namespaces \
	--attach-classes \
	--indent=spaces=4 \
	--convert-tabs \
	--align-reference=name \
	--keep-one-line-statements \
	--pad-header \
	--pad-oper \
	--unpad-paren \
	--max-continuation-indent=120 \
	components/**/*.h components/**/*.c main/*.h main/*.c
