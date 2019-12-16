#!/usr/bin/python3

import argparse

def write_ucs2(a, description):
    l = len(a)

    print("/* %s */" % description)
    print("#define AO_%s_LEN 0x%02x" % (description, l * 2 + 2))
    print("#define AO_%s_STRING \"%s\"" % (description, a))
    print("#define AO_%s_UCS2" % description, end='')
    for i in range(l):
        c = a[i]
        if i > 0:
            print(",", end='')
        if 0x20 <= ord(c) and ord(c) < 128:
            print(" '%c', 0" % c, end='')
        else:
            print(" LE_WORD(0x%04x)" % ord(c), end='')
    print("\n")

def write_string(a,description):
    print("/* %s */" % description)
    print("#define AO_%s_STRING \"%s\"" % (description, a))

def write_int(a, description):
    print("/* %s */" % description)
    print("#define AO_%s_NUMBER %d\n" % (description, a))

def write_hex(a, description):
    print("/* %s */" % description)
    print("#define AO_%s_NUMBER 0x%04x\n" % (description, a))


def auto_int(x):
    return int(x,0)

def make_product():
    manufacturer = "keithp.com"
    product = "MetroSnek"
    version = "0.0"
    output = ""
    serial = 1
    user_argind = 0
    id_vendor = 0xfffe
    id_product = 0x000a
    parse = argparse.ArgumentParser()
    parse.add_argument("-m", "--manufacturer", help="manufacturer name", default=manufacturer)
    parse.add_argument("-p", "--product", help="product name", default=product)
    parse.add_argument("-V", "--id_vendor", type=auto_int, help="vendor id number", default=id_vendor)
    parse.add_argument("-i", "--id_product", type=auto_int, help="product id number", default=id_product)
    parse.add_argument("-v", "--version", help="version string", default=version)
    parse.add_argument("-s", "--serial", type=auto_int, help="serial number", default=serial)
    args = parse.parse_args()
    if args.manufacturer:
        manufacturer = args.manufacturer
    if args.product:
        product = args.product
    if args.id_vendor:
        id_vendor = args.id_vendor
    if args.id_product:
        id_product = args.id_product
    if args.version:
        version = args.version
    if args.serial:
        serial = args.serial
    write_ucs2(manufacturer, "iManufacturer")
    write_ucs2(product, "iProduct")
    write_ucs2("%06d" % serial, "iSerial")
    write_int(serial, "iSerial")
    write_hex(id_product, "idProduct")
    write_hex(id_vendor, "idVendor")
    write_string(version, "iVersion")

make_product()
