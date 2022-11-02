SUMMARY = "OpenRC psplash plugin."
DESCRIPTION = "OpenRC psplash plugin."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://src \
           "
S = "${WORKDIR}/src"

DEPENDS = "openrc"
RDEPENDS:${PN} = "psplash"

inherit cmake pkgconfig

FILES:${PN} = "${libdir}/rc/plugins/librc-psplash.so"
