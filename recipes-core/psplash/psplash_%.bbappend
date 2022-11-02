FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://init-rc \
            file://profile"

SPLASH_IMAGES = "file://play-os-img.h;outsuffix=default"

inherit openrc-run

do_install:append() {
    # if ${@bb.utils.contains('DISTRO_FEATURES', 'openrc', 'true', 'false', d)}; then
    #     install -d ${D}${sysconfdir}/init.d/
    #     install -m 0755 ${WORKDIR}/init-rc ${D}${sysconfdir}/init.d/psplash
    # fi

    install -d ${D}${sysconfdir}/profile.d/
    install -m0755 ${WORKDIR}/profile ${D}${sysconfdir}/profile.d/psplash.sh
}

RC_INITSCRIPT_NAME = "psplash"
RC_INITSCRIPT_PARAMS = "sysinit"

FILES:${PN} += "${sysconfdir}/init.d/psplash"

PACKAGECONFIG[progress-bar] = ",--disable-progress-bar"
