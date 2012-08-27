/*
  man rtld-audit ld.so
  Siehe auch latrace
 */ 

#define _GNU_SOURCE

#include <link.h>
#include <stdio.h>

unsigned int
la_version(unsigned int version)
{
    printf("la_version(): %d\n", version);

    return version;
}

char *
la_objsearch(const char *name, uintptr_t *cookie, unsigned int flag)
{
    printf("la_objsearch(): name = %s; cookie = %x", name, cookie);
    printf("; flag = %s\n",
	    (flag == LA_SER_ORIG) ?    "LA_SER_ORIG" :
	    (flag == LA_SER_LIBPATH) ? "LA_SER_LIBPATH" :
	    (flag == LA_SER_RUNPATH) ? "LA_SER_RUNPATH" :
	    (flag == LA_SER_DEFAULT) ? "LA_SER_DEFAULT" :
	    (flag == LA_SER_CONFIG) ?  "LA_SER_CONFIG" :
	    (flag == LA_SER_SECURE) ?  "LA_SER_SECURE" :
	    "???");

    return name;
}

void
la_activity (uintptr_t *cookie, unsigned int flag)
{
    printf("la_activity(): cookie = %x; flag = %s\n", cookie,
	    (flag == LA_ACT_CONSISTENT) ? "LA_ACT_CONSISTENT" :
	    (flag == LA_ACT_ADD) ?        "LA_ACT_ADD" :
	    (flag == LA_ACT_DELETE) ?     "LA_ACT_DELETE" :
	    "???");
}

unsigned int
la_objopen(struct link_map *map, Lmid_t lmid, uintptr_t *cookie)
{
    printf("la_objopen(): loading \"%s\"; lmid = %s; cookie=%x\n",
	    map->l_name,
	    (lmid == LM_ID_BASE) ?  "LM_ID_BASE" :
	    (lmid == LM_ID_NEWLM) ? "LM_ID_NEWLM" :
	    "???",
	    cookie);

    return LA_FLG_BINDTO | LA_FLG_BINDFROM;
}

unsigned int
la_objclose (uintptr_t *cookie)
{
    printf("la_objclose(): %x\n", cookie);

    return 0;
}

void
la_preinit(uintptr_t *cookie)
{
    printf("la_preinit(): %x\n", cookie);
}

uintptr_t
la_symbind32(Elf32_Sym *sym, unsigned int ndx, uintptr_t *refcook,
	uintptr_t *defcook, unsigned int *flags, const char *symname){
    printf("la_symbind32(): symname = %s; sym->st_value = %p\n",
	    symname, sym->st_value);
    printf("        ndx = %d; flags = 0x%x", ndx, *flags);
    printf("; refcook = %x; defcook = %x\n", refcook, defcook);

    return sym->st_value;
}

uintptr_t
la_symbind64(Elf64_Sym *sym, unsigned int ndx, uintptr_t *refcook,
	uintptr_t *defcook, unsigned int *flags, const char *symname)
{
    printf("la_symbind64(): symname = %s; sym->st_value = %p\n",
	    symname, sym->st_value);
    printf("        ndx = %d; flags = 0x%x", ndx, *flags);
    printf("; refcook = %x; defcook = %x\n", refcook, defcook);

    return sym->st_value;
}

/*
Elf64_Addr
la_i86_gnu_pltenter(Elf64_Sym *sym, unsigned int ndx,
	uintptr_t *refcook, uintptr_t *defcook, La_i86_regs *regs,
	unsigned int *flags, const char *symname, long int *framesizep)
{
    printf("la_i86_gnu_pltenter(): %s (%p)\n", symname, sym->st_value);

    return sym->st_value;
}

*/