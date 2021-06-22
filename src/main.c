#include <stdio.h>
#include <linux/elf.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
char* elf_arch(int arch){
  switch(arch){
    case(0x00):
      return "NA";
    case(0x02):
      return "SPARC";
    case(0x03):
      return "x86";
    case(0x08):
      return "MIPS";
    case (0x14):
      return "PowerPC";
    case(0x16):
      return "S390";
    case(0x28):
      return	"ARM";
    case (0x2A):
      return "SuperH";
    case (0x32):
      return	"IA-64";
    case  (0x3E):
      return "x86-64";
    case (0xB7):
      return "AArch64";
    case(0xF3):
      return "RISC-V";
  }
}

char * read_section(int fd, Elf64_Shdr sh, int buff)
{
	assert(lseek(fd, (off_t)sh.sh_offset, SEEK_SET) == (off_t)sh.sh_offset);
	assert(read(fd, (void *)buff, sh.sh_size) == sh.sh_size);

	return buff;
}


uint16_t find_segment(void *mmaped_elf, int fsize){
	Elf64_Ehdr* elf_hdr = (Elf64_Ehdr *) mmaped_elf;
	Elf64_Shdr* sh_table = (Elf64_Shdr*) mmaped_elf;

	int segments = elf_hdr->e_shnum;
	printf("TARGET STRING TABLE OFFSET :: %p\n", elf_hdr->e_shstrndx);
	char* sh_str = read_section(mmaped_elf, sh_table[elf_hdr->e_shstrndx], fsize);

	// auto sh_str = sh_table[elf_hdr->e_shstrndx]; // ELF String table

	Elf64_Phdr* phdr = (Elf64_Phdr*) (mmaped_elf + elf_hdr->e_phoff);
	printf("TARGET SEGMENTS :: %d\n", segments);

	for(int i = 0; i < segments; i++){
		if(strcmp(".text", (sh_str + sh_table[i].sh_name))) {
			printf(".text found :: %s", (sh_str + sh_table[i].sh_name));
		}
	};	
}

int main(int argc, char *argv[]) {
  int fsize, target_fd;
  void *mmaped_elf;
  printf("FILENAME :: %s\n", argv[1]);
  target_fd  = map_elf(argv[1], &mmaped_elf, &fsize);

  Elf64_Ehdr* elf_hdr = (Elf64_Ehdr *) mmaped_elf;
	Elf64_Phdr* elf_pdr = (Elf64_Phdr*) mmaped_elf;

  printf ("TARGET ENTRY POINT :: %p\n", elf_hdr->e_entry);
  printf ("TARGET ARCHITECTURE :: %s\n", elf_arch(elf_hdr->e_machine));
  printf ("TARGET E_IDENT :: %s\n", elf_hdr->e_ident);
	printf("TARGET E_IDENT :: %x\n", elf_hdr->e_ident[EI_CLASS]);
	uint16_t oldEntryPoint = elf_pdr->p_paddr;
	uint16_t textSegment = find_segment(mmaped_elf, &fsize);

	elf_pdr->p_paddr = 0x540;

}

int get_file_size(int fd){
  struct stat st;
  fstat(fd, &st);
  return st.st_size;
}

int map_elf(char* file_name, void** data, int *len){
  int size;
  int fd;
  if((fd = open(file_name, O_APPEND| O_RDWR, 0)) < 0){
    printf("File not found\n");
    exit(1);
  }
  size = get_file_size(fd);
  printf("FILESIZE :: %d\n", size);

  if((*data = mmap(0, size, PROT_READ| PROT_WRITE| PROT_EXEC, MAP_SHARED, fd, 0)) == MAP_FAILED){
    printf("Failed to mmap file\n");
    exit(1);
  }
  *len = size;
  printf("MAPPED AT ADDRESS :: %p\n", data);
  return fd;
}
