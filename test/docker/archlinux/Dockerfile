FROM base/archlinux

WORKDIR /data

RUN echo 'Server = http://arch.jensgutermuth.de/$repo/os/$arch' > /etc/pacman.d/mirrorlist
RUN pacman-key --init 
RUN pacman-key --populate
RUN pacman-key --refresh-keys
RUN pacman --noconfirm -Sy archlinux-keyring
RUN pacman --noconfirm -Su
RUN pacman-db-upgrade
RUN pacman --noconfirm -S base-devel cmake boost boost-libs python python2 libpqxx postgresql openmpi hdf5 netcdf



