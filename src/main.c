#include <stdio.h>
#include <linux/elf.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

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

int main(int argc, char *argv[]) {
  int fsize, target_fd;
  void *mmaped_elf;
  printf("FILENAME :: %s\n", argv[1]);
  target_fd  = map_elf(argv[1], &mmaped_elf, &fsize);

  Elf64_Ehdr* elf_hdr = (Elf64_Ehdr *) mmaped_elf;

  printf ("TARGET ENTRY POINT :: %p\n", elf_hdr->e_entry);
  printf ("TARGET ARCHITECTURE :: %s\n", elf_arch(elf_hdr->e_machine));
  printf ("TARGET E_IDENT :: %c\n", elf_hdr->e_ident);
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
