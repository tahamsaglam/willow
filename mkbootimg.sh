cp arch/arm/boot/zImage ../img/
cp arch/arm/boot/zImage ../willow_android/device/thinkware/willow/
../willow_android/out/host/linux-x86/bin/mkbootimg --kernel arch/arm/boot/zImage --ramdisk ../willow_android/out/target/product/willow/ramdisk.img --cmdline "console=ttySAC2,115200" --base 40008000 --pagesize 4096 -o ../willow_android/out/target/product/willow/boot.img
../willow_android/out/host/linux-x86/bin/mkbootimg --kernel arch/arm/boot/zImage --ramdisk ../willow_android/out/target/product/willow/ramdisk-recovery.img --cmdline "console=ttySAC2,115200" --base 40008000 --pagesize 4096 -o ../willow_android/out/target/product/willow/recovery.img

# JB
cp arch/arm/boot/zImage ../willow_jb_android/device/thinkware/willow/
../willow_jb_android/out/host/linux-x86/bin/mkbootimg --kernel arch/arm/boot/zImage --ramdisk ../willow_jb_android/out/target/product/willow/ramdisk.img --cmdline "console=ttySAC2,115200" --base 0x40008000 --pagesize 4096 --output ../willow_jb_android/out/target/product/willow/boot.img
../willow_jb_android/out/host/linux-x86/bin/mkbootimg --kernel arch/arm/boot/zImage --ramdisk ../willow_jb_android/out/target/product/willow/ramdisk-recovery.img --cmdline "console=ttySAC2,115200" --base 0x40008000 --pagesize 4096 --output ../willow_jb_android/out/target/product/willow/recovery.img

# JB SW1
cp arch/arm/boot/zImage ../willow_jb_android_sw1/device/thinkware/willow/
../willow_jb_android_sw1/out/host/linux-x86/bin/mkbootimg --kernel arch/arm/boot/zImage --ramdisk ../willow_jb_android_sw1/out/target/product/willow/ramdisk.img --cmdline "console=ttySAC2,115200" --base 0x40008000 --pagesize 4096 --output ../willow_jb_android_sw1/out/target/product/willow/boot.img
../willow_jb_android_sw1/out/host/linux-x86/bin/mkbootimg --kernel arch/arm/boot/zImage --ramdisk ../willow_jb_android_sw1/out/target/product/willow/ramdisk-recovery.img --cmdline "console=ttySAC2,115200" --base 0x40008000 --pagesize 4096 --output ../willow_jb_android_sw1/out/target/product/willow/recovery.img

