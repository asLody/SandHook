#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "../includes/elf_util.h"
#include "../includes/log.h"

using namespace SandHook;

ElfImg::ElfImg(const char *elf) {
    this->elf = elf;
    //load elf
    FILE *fp = NULL;
    if (!(fp = fopen(elf, "rb")))  {
        LOGE("Unable to open %s\n", elf);
        return;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = (char*)malloc(size);
    if (fread(buffer, 1, size, fp) != size) {
        LOGE("fread error\n");
        return;
    }
    fclose(fp);

    Elf_Off symtab_entsize = 0;
    header = reinterpret_cast<Elf_Ehdr *>(buffer);
    section_header = reinterpret_cast<Elf_Shdr *>(header + header->e_shoff);

    Elf_Off shoff = reinterpret_cast<Elf_Off>(((void *) elf) + header->e_shoff);

    for (int i = 0; i < header->e_shnum; i++, shoff += header->e_shentsize) {
        Elf_Shdr *section_h = (Elf_Shdr *) shoff;
        switch (section_h->sh_type) {
            case SHT_SYMTAB:
                symtab = section_h;
                symtab_offset = section_h->sh_offset;
                symtab_size = section_h->sh_size;
                symtab_entsize = section_h->sh_entsize;
                symtab_count = symtab_size / symtab_entsize;
                break;
            case SHT_STRTAB:
                symstr_offset = section_h->sh_offset;
                break;
        }
    }

    if(!symtab_offset) {
        LOGW("can't find symtab from sections\n");
    }

    sym_start = reinterpret_cast<Elf_Sym *>(header + symtab_offset);

    //load module base
    base = getModuleBase(0, elf);
}

ElfImg::~ElfImg() {
    if (buffer) {
        free(buffer);
        buffer = nullptr;
    }
}

Elf_Addr ElfImg::getSymbOffset(const char *name) {
    Elf_Addr _offset = 0;
    for(int i = 0 ; i < symtab_count; i++) {
        unsigned int st_type = ELF_ST_TYPE(sym_start[i].st_info);
        char* st_name = buffer + symstr_offset + sym_start[i].st_name;
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
    return reinterpret_cast<Elf_Addr>(base + getSymbOffset(name));
}

void *ElfImg::getModuleBase(int pid, const char *name) {
    FILE *fp;
    long addr = 0;
    char *pch;
    char filename[32];
    char line[1024];

    if (pid <= 0) {
        snprintf(filename, sizeof(filename), "/proc/self/maps");
    } else {
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
    }
    fp = fopen(filename, "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, name)) {
                pch = strtok(line, "-");
                addr = strtoul(pch, NULL, 16);
                break;
            }
        }
        fclose(fp);
    }

    LOGD("get module base %s: %lu", name, addr);

    return reinterpret_cast<void *>(addr);
}
