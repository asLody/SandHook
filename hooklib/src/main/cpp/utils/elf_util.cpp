//
// Created by Swift Gan on 2019/3/14.
//
#include <malloc.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "../includes/elf_util.h"
#include "../includes/log.h"

using namespace SandHook;

ElfImg::ElfImg(const char *elf) {
    this->elf = elf;
    //load elf
    int fd = open(elf, O_RDONLY);
    if (fd < 0) {
        LOGE("failed to open %s", elf);
        return;
    }

    size = lseek(fd, 0, SEEK_END);
    if (size <= 0) {
        LOGE("lseek() failed for %s", elf);
    }

    header = reinterpret_cast<Elf_Ehdr *>(mmap(0, size, PROT_READ, MAP_SHARED, fd, 0));

    close(fd);

    Elf_Off symtab_entsize = 0;

    section_header = reinterpret_cast<Elf_Shdr *>(((void *) header) + header->e_shoff);

    void* shoff = section_header;
    char* section_str = reinterpret_cast<char *>(section_header[header->e_shstrndx].sh_offset + ((void *) header));

    bool has_strtab = false;
    bool has_dynsym = false;

    for (int i = 0; i < header->e_shnum; i++, shoff += header->e_shentsize) {
        Elf_Shdr *section_h = (Elf_Shdr *) shoff;
        char* sname = section_h->sh_name + section_str;
        switch (section_h->sh_type) {
            case SHT_DYNSYM:
                has_dynsym = true;
                dynsym = section_h;
                break;
            case SHT_SYMTAB:
                if (strcmp(sname, ".symtab") == 0) {
                    symtab = section_h;
                    symtab_offset = section_h->sh_offset;
                    symtab_size = section_h->sh_size;
                    symtab_entsize = section_h->sh_entsize;
                    symtab_count = symtab_size / symtab_entsize;
                }
                break;
            case SHT_STRTAB:
                has_strtab = true;
                if (strcmp(sname, ".strtab") == 0) {
                    strtab = section_h;
                    symstr_offset = section_h->sh_offset;
                }
                break;
            case SHT_PROGBITS:
                if (has_dynsym && has_strtab && bias == -4396) {
                    bias = (off_t) section_h->sh_addr - (off_t) section_h->sh_offset;
                }
                break;
        }
    }

    if(!symtab_offset) {
        LOGW("can't find symtab from sections\n");
    }

    sym_start = reinterpret_cast<Elf_Sym *>(((void *) header) + symtab_offset);

    //load module base
    base = getModuleBase(elf);
}

ElfImg::~ElfImg() {
    //open elf file local
    if (buffer) {
        free(buffer);
        buffer = nullptr;
    }
    //use mmap
    if (header) {
        munmap(header, size);
    }
}

Elf_Addr ElfImg::getSymbOffset(const char *name) {
    Elf_Addr _offset = 0;
    for(int i = 0 ; i < symtab_count; i++) {
        unsigned int st_type = ELF_ST_TYPE(sym_start[i].st_info);
        char* st_name = static_cast<char *>(((void *) header) + symstr_offset + sym_start[i].st_name);
        if (st_type == STT_FUNC && sym_start[i].st_size) {
            if(strcmp(st_name, name) == 0) {
                _offset = sym_start[i].st_value;
                LOGD("find %s: %x\n", elf ,_offset);
                break;
            }
        }
    }
    return _offset;
}

Elf_Addr ElfImg::getSymbAddress(const char *name) {
    Elf_Addr offset = getSymbOffset(name);
    if (offset > 0 && base > 0) {
        return reinterpret_cast<Elf_Addr>(base + offset - bias);
    } else {
        return 0;
    }
}

void *ElfImg::getModuleBase(const char *name) {
    FILE *maps;
    char buff[256];
    off_t load_addr;
    int found = 0;
    maps = fopen("/proc/self/maps", "r");
    while (!found && fgets(buff, sizeof(buff), maps))
        if (strstr(buff, "r-xp") && strstr(buff, name)) found = 1;

    if (sscanf(buff, "%lx", &load_addr) != 1)
        LOGE("failed to read load address for %s", name);

    fclose(maps);

    LOGD("get module base %s: %lu", name, load_addr);

    return reinterpret_cast<void *>(load_addr);
}
