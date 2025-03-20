# chiBoot

chiBoot is a little X64 UEFI bootloader aimed at booting my own operaring system, but I made it generic because why not.

It uses a chiConf file to describe boot entries, bootloader configuration and the arguments that must be passed to the kernel before startup.

## chiConf
```
# this is a chiConf file sample
# use "bootEntry" to describe as many entries as needed
# use "bootApplication" to bootstrap a UEFI app.

```

## TODO

- Load a chiConf file
- Locate and load kernels
- Pass arguments according to the chiConf file
- Support the following files: ELF, PE32, machO (X64)
