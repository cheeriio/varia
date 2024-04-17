
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "File $1 not found"
    exit 1
fi

filesize=$(wc -c "$1" | awk '{print $1}')
lines=$(wc -l "$1" | awk '{print $1}')

if [ "$lines" -gt 7 ]; then
    echo File $1 has too many lines.
    exit 1
elif [ "$lines" -lt 6 ]; then
    echo File $1 has too few lines.
    exit 1
fi

if [ "$filesize" -gt 512 ]; then
    echo File $1 is too big.
    exit 1
fi

dd count=1 seek=0 if=main of=/dev/c0d0
dd count=1 seek=1 if=$1 of=/dev/c0d0