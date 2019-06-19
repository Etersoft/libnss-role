#!/bin/sh
fatal()
{
    echo "Failed" >&2
    exit 1
}

LD_LIBRARY_PATH=$(pwd) ./test_role "$@" || fatal
diff -u test/role.source test/role.test.new || fatal
diff -u test/role.source.add test/role.test.add || fatal
diff -u test/role.source.del test/role.test.del || fatal
echo "Done"
