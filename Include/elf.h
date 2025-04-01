#ifndef ELF_H
#define ELF_H

#include <Uefi.h>

#define EI_NIDENT (16)

#define ELF_MAGIC 0x464C457F        //0x7f - ELF


typedef struct {
  UINT8   e_ident[EI_NIDENT];   /* Magic number and other info */
  UINT16  e_type;               /* Object file type */
  UINT16  e_machine;            /* Architecture */
  UINT32  e_version;            /* Object file version */
  UINT64  e_entry;              /* Entry point virtual address */
  UINT64  e_phoff;              /* Program header table file offset */
  UINT64  e_shoff;              /* Section header table file offset */
  UINT32  e_flags;              /* Processor-specific flags */
  UINT16  e_ehsize;             /* ELF header size in bytes */
  UINT16  e_phentsize;          /* Program header table entry size */
  UINT16  e_phnum;              /* Program header table entry count */
  UINT16  e_shentsize;          /* Section header table entry size */
  UINT16  e_shnum;              /* Section header table entry count */
  UINT16  e_shstrndx;           /* Section header string table index */
} Elf64_Ehdr;

typedef struct {
  UINT32  sh_name;       /* Section name (string tbl index) */
  UINT32  sh_type;       /* Section type */
  UINT64  sh_flags;      /* Section flags */
  UINT64  sh_addr;       /* Section virtual addr at execution */
  UINT64  sh_offset;     /* Section file offset */
  UINT64  sh_size;       /* Section size in bytes */
  UINT32  sh_link;       /* Link to another section */
  UINT32  sh_info;       /* Additional section information */
  UINT64  sh_addralign;  /* Section alignment */
  UINT64  sh_entsize;    /* Entry size if section holds table */
} Elf64_Shdr;

typedef struct {
  UINT32  st_name;      /* Symbol name (string tbl index) */
  UINT8   st_info;      /* Symbol type and binding */
  UINT8   st_other;     /* Symbol visibility */
  UINT16  st_shndx;     /* Section index */
  UINT64  st_value;     /* Symbol value */
  UINT64  st_size;      /* Symbol size */
} Elf64_Sym;

typedef struct {
  UINT16  si_boundto;   /* Direct bindings, symbol bound to */
  UINT16  si_flags;     /* Per symbol flags */
} Elf64_Syminfo;

typedef struct {
  UINT64  r_offset;     /* Address */
  UINT64  r_info;       /* Relocation type and symbol index */
} Elf64_Rel;

typedef struct {
  UINT64  r_offset;     /* Address */
  UINT64  r_info;       /* Relocation type and symbol index */
  INT64   r_addend;     /* Addend */
} Elf64_Rela;

typedef struct {
  UINT32  p_type;       /* Segment type */
  UINT32  p_flags;      /* Segment flags */
  UINT64  p_offset;     /* Segment file offset */
  UINT64  p_vaddr;      /* Segment virtual address */
  UINT64  p_paddr;      /* Segment physical address */
  UINT64  p_filesz;     /* Segment size in file */
  UINT64  p_memsz;      /* Segment size in memory */
  UINT64  p_align;      /* Segment alignment */
} Elf64_Phdr;


#endif
