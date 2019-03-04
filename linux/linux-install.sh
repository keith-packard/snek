#!/bin/sh

can_ask=y

finish()
{
    if [ "$can_ask" = "y" ]; then
	echo ""
	echo -n "Press enter to continue..."
	read foo
    fi
    exit $1
}

#
# Make sure we have a terminal to talk to
#

if tty -s; then
    :
else
    case "$DISPLAY" in
	"")
	echo 'No user input available'
	can_ask=n
	;;
	*)
	GUESS_XTERMS="x-terminal-emulator xterm rxvt roxterm gnome-terminal dtterm eterm Eterm kvt konsole aterm"
        for a in $GUESS_XTERMS; do
            if type $a >/dev/null 2>&1; then
                XTERM=$a
                break
            fi
        done
	case "$XTERM" in
	    "")
	    echo 'No terminal emulator available'
	    can_ask=n
	    ;;
	    *)
	    exec "$XTERM" -e "sh '$0'"
	    ;;
	esac
	;;
    esac
fi

#
# Make sure we can run python3
#

echo -n "Checking for python3..."

if python3 --version > /dev/null 2>&1; then
    echo " found it."
else
    echo " python3 isn't working."
    echo ""
    echo "You'll need to install lsusb and python3 with pyserial and curses"
    echo "on this computer before Snek will work properly."
    finish 1
fi
    
echo -n "Checking for pyserial..."

if python3 -m serial.tools.list_ports > /dev/null 2>&1; then
    echo "  found it."
else
    echo " pyserial isn't working."
    echo ""
    echo "You'll need to install lsusb and python3 with pyserial and curses"
    echo "on this computer before Snek will work properly."
    finish 1
fi

echo -n "Checking for python curses..."

if python3 -c 'import curses' > /dev/null 2>&1; then
    echo "   found it."
else
    echo " python curses isn't working."
    echo ""
    echo "You'll need to install lsusb and python3 with pyserial and curses"
    echo "on this computer before Snek will work properly."
    finish 1
fi

#
# Pick an installation target
# 

if [ '(' -d /opt -a -w /opt ')' -o '(' -d /opt/Snek -a -w /opt/Snek ')' ]; then
    target_default=/opt
else
    target_default="$HOME"
fi

case "$#" in
0)
    echo -n "Installation location [default: $target_default] "
    if [ "$can_ask" = "y" ]; then
	read target
    else
	echo ""
	target=""
    fi
    case "$target" in
	"")
	target="$target_default"
	;;
    esac
    ;;
*)
    target="$1"
    ;;
esac

target_snek="$target"/Snek

echo -n "Installing to $target..."

#
# Make sure the target exists
#
mkdir -p "$target_snek"

if [ ! -d "$target_snek" ]; then
    echo "$target_snek does not exist and cannot be created"
    finish 1
fi

if [ ! -w "$target_snek" ]; then
    echo "$target_snek cannot be written"
    finish 1
fi

#
# Unpack the tar archive appended to the end of this script
#
archive_line=`awk '/^__ARCHIVE_BELOW__/ {print NR + 1; exit 0; }' "$0"`

tail -n+$archive_line "$0" | tar xjf - -C "$target"

case $? in
0)
    echo " done."
    ;;
*)
    echo "Install failed."
    finish 1
    ;;
esac

#
# Create custom files by editing the paths
#

case "$target" in
/*)
    target_abs="$target"
    ;;
*)
    target_abs=`pwd`/$target
    ;;
esac

BIN="$target_abs"/Snek

for infile in "$target"/Snek/*.in; do
    desktop=`basename "$infile" .in`
    # pre-pend a vendor to .desktop filenames
    case "$desktop" in
	*.desktop)
	    desktop="keithp.com-$desktop"
	    ;;
    esac
    desktop="$target"/Snek/"$desktop"
    rm -f "$desktop"
    sed -e "s;@SHAREDIR@;$BIN;" -e "s;@BINDIR@;$BIN;" -e "s;@ICONDIR@;$BIN;" "$infile" > "$desktop"
    chmod +x "$desktop"
done

#
# Install the .desktop files
#

for desktop in "$target"/Snek/*.desktop; do
    case `id -u` in
	0)
	    xdg-desktop-menu install --mode system "$desktop"
	    ;;
	*)
	    xdg-desktop-menu install --mode user "$desktop"
	    ;;
    esac
done

#
# Install icons
#

for icon_dir in /usr/share/icons/hicolor/scalable/mimetypes "$HOME/.icons" "$HOME/.kde/share/icons"; do
    if [ -w "$icon_dir" ]; then
	cp "$target"/Snek/*.svg "$icon_dir"
	update-icon-caches "$icon_dir"
    fi
done


#
# Install icon to desktop if desired
#

if [ -d $HOME/Desktop ]; then
    default_desktop=n
    if [ "$can_ask" = "y" ]; then
	:
    else
	default_desktop=y
    fi

    answered=n
    while [ "$answered" = "n" ]; do
	echo -n "Install icons to desktop? [default: $default_desktop] "
	if [ "$can_ask" = "y" ]; then
	    read do_desktop
	else
	    echo
	    do_desktop=""
	fi

	case "$do_desktop" in
	    "")
	    do_desktop=$default_desktop
	    ;;
	esac

	case "$do_desktop" in
	[yYnN]*)
	    answered=y
	    ;;
	esac
    done

    case "$do_desktop" in
	[yY]*)
	    echo -n "Installing desktop icons..."
	    for d in "$target"/Snek/*.desktop; do
		base=`basename $d`
		cp --remove-destination "$d" "$HOME/Desktop/"
	    done
	    ;;
    esac

    echo " done."
fi

finish 0

__ARCHIVE_BELOW__
