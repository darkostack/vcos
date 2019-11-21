#!/bin/sh

CLANG_FORMAT_VERSION="clang-format version 9.0.0"

die() {
    echo " *** ERROR: " $*
    exit 1
}

if which clang-format > /dev/null; then
    case "$(clang-format --version)" in
        "$CLANG_FORMAT_VERSION"*)
            ;;
        *)
            die "$(clang-format --version); clang-format 9.0.0 required"
            ;;
    esac
else
    die "clang-format 9.0.0 required"
fi

clang-format $@ || die

# ensure EOF newline
REPLACE=no
for arg
do
    case $arg in
        -i)
            REPLACE=yes
            ;;
    esac
done

file=$arg

[ $REPLACE != yes ] || {
    [ -n "$(tail -c1 $file)" ] && echo >> $file
}

exit 0
