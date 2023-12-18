# LicheePi4A-Build

some script & blob for LicheePi image build

# build kernel

```
sh mklinux.sh
```

# build uboot

```
sh mkuboot.sh
```

# apply into your system

```
sudo mount rootfs.ext4 /mnt/chroot
sudo mount bootfs.ext4 /mnt/chroot/boot
sudo cp -arv ./overlay/* /mnt/chroot/
sudo umount /mnt/chroot
sudo umount /mnt/chroot/boot
sync
```

# device tree & uboot

we select devicetree by uboot:

```
u-boot-with-spl-cluster-8g.bin:     /boot/dtbs/thead/light-lpi4a-cluster.dtb
u-boot-with-spl-cluster-16g.bin:    /boot/dtbs/thead/light-lpi4a-cluster-16gb.dtb
u-boot-with-spl-console.bin:        /boot/dtbs/thead/light-lpi4a-console.dtb
u-boot-with-spl-console-16g.bin:    /boot/dtbs/thead/light-lpi4a-console-16g.dtb
u-boot-with-spl-sbc-16g.bin:        /boot/dtbs/thead/light-lpi4a-16gb.dtb
u-boot-with-spl-sbc.bin:            /boot/dtbs/thead/light-lpi4a.dtb
u-boot-with-spl-z14inch-m0-16g.bin: /boot/dtbs/thead/light-lpi4a-z14inch-m0-16g.dtb
u-boot-with-spl-z14inch-m0.bin:     /boot/dtbs/thead/light-lpi4a-z14inch-m0.dtb
```

# LicheeConsole4A screen rotate

```
/opt/touch-setup.sh
```

# changelog

2023/12/12: initial import
