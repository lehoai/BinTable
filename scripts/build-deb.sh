#!/bin/bash
set -e

APP="bintable"
VERSION="1.0"
ARCH="amd64"
PKG_DIR="${APP}_${VERSION}_${ARCH}"
BUILD_DIR="cmake-build-release"

echo "==> Building Release..."
cmake --build ${BUILD_DIR} --config Release

echo "==> Creating package structure..."
rm -rf ${PKG_DIR}
mkdir -p ${PKG_DIR}/DEBIAN
mkdir -p ${PKG_DIR}/usr/bin
mkdir -p ${PKG_DIR}/usr/share/${APP}/assets/fonts
mkdir -p ${PKG_DIR}/usr/share/applications

echo "==> Copying files..."
# Wrapper script (handles relative asset paths)
cat > ${PKG_DIR}/usr/bin/${APP} << 'WRAPPER'
#!/bin/bash
cd /usr/share/bintable
exec /usr/share/bintable/bintable-bin "$@"
WRAPPER
chmod 755 ${PKG_DIR}/usr/bin/${APP}

cp ${BUILD_DIR}/amgui ${PKG_DIR}/usr/share/${APP}/bintable-bin
chmod 755 ${PKG_DIR}/usr/share/${APP}/bintable-bin

cp assets/fonts/DejaVuSansMono.ttf    ${PKG_DIR}/usr/share/${APP}/assets/fonts/
cp assets/fonts/NotoMonoJP-subset.ttf ${PKG_DIR}/usr/share/${APP}/assets/fonts/

echo "==> Writing control file..."
INSTALLED_SIZE=$(du -sk ${PKG_DIR}/usr | cut -f1)
DEPS=$(ldd ${BUILD_DIR}/amgui \
  | awk '/libpq/{print "libpq5"} /libGL/{print "libgl1"}' \
  | sort -u | paste -sd ', ')

cat > ${PKG_DIR}/DEBIAN/control << EOF
Package: ${APP}
Version: ${VERSION}
Architecture: ${ARCH}
Maintainer: $(git config user.name) <$(git config user.email)>
Installed-Size: ${INSTALLED_SIZE}
Depends: ${DEPS}
Description: BinTable - PostgreSQL table viewer
 A lightweight desktop GUI for browsing and querying PostgreSQL databases.
EOF

echo "==> Building .deb..."
dpkg-deb --build --root-owner-group ${PKG_DIR}

echo ""
echo "Done! Package: ${PKG_DIR}.deb"
dpkg-deb --info ${PKG_DIR}.deb