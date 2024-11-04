#include<elf.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<elfutils/elf-knowledge.h>
#include<string.h>

#define BUFFER_SIZE 4096
char buffer[BUFFER_SIZE];
void section_header(Elf64_Ehdr* ehdr,char* p,int size);
void elfheader(FILE* fptr,Elf64_Ehdr* ehdr,char* p,unsigned long size);

void print_section_details(char* file,int index,Elf64_Shdr* shdr,Elf64_Ehdr* ehdr){
   
    int flags=shdr->sh_flags;
    char* str=NULL;
    switch(flags){
        case 0x01:
            str="WRITE ONLY";
            break;
        case 0x02:
            str="ALLOC ONLY";
            break;
        case 0x03:
            str="WRITE+ALLOC";
            break;
        case 0x04:
            str="EXEC ONLY";
            break;
        case 0x05:
            str="WR + EXEC";
            break;
        case 0x06:
            str="ALLOC + EXEC";
            break;
        case 0xf0000000:
            str="MASK PROC";
            break;
        default:
            str="something else";
            break;
    }
    char* strtab=file+ehdr->e_shoff+(ehdr->e_shstrndx*sizeof(Elf64_Shdr));
    char* section_name=file+((Elf64_Shdr*)strtab)->sh_offset+shdr->sh_name;
    // printf("name : ");
    // for(int i=0;i<4;i++){
    //     printf("%c",*(file+shdr->sh_name+i));
    // }
    printf("name :  %s & flags: %s & size: %x & address is: %lu\n",section_name,str,shdr->sh_size,shdr->sh_addr);
    if(shdr->sh_flags==0x06){
        printf("loading executable section at offset %lu\n",shdr->sh_offset);
        FILE* fptr=NULL;
        fptr=fopen("text_read.txt","w+");
        if(fptr==NULL){perror("fopen\n");return;}
        char* ptr=file+shdr->sh_offset;
        fwrite(ptr,sizeof(char),shdr->sh_size,fptr);
        fclose(fptr);
    }
    if(shdr->sh_type==SHT_STRTAB){
        puts("this section contains the string table\n");
        // for(size_t i=0;i<shdr->sh_size;i++){
        //     putchar(*(file+shdr->sh_addr+i));
        // }
        putchar('\n');
    }
    puts("\n");
   

}
void read_sec_header(FILE* fptr,char* file,unsigned long len,Elf64_Ehdr* ehdr){
    Elf64_Shdr shdr;
     puts("--------------Section header info---------------\n");
    printf("Section header offset is %lu\n",ehdr->e_shoff);
    printf("Section header size is %d\n",ehdr->e_shentsize);
    printf("Number of Section header entries are %d\n",ehdr->e_shnum);
    
    char* ptr=file+ehdr->e_shoff;
    for(int i=0;i<ehdr->e_shnum;i++){
        shdr=*(Elf64_Shdr*)ptr;
        printf("Entry %d :  ",i);
        print_section_details(file,i,&shdr,ehdr);
        ptr+=sizeof(Elf64_Shdr);
    }

    // int nr_elements=fread(&shdr,sizeof(Elf64_Shdr),)
}

void section_header(Elf64_Ehdr* ehdr,char* p,int size){
    unsigned long section_header_table=ehdr->e_shoff;
    Elf64_Half s_entries=ehdr->e_shnum;
    printf("section entries are %d\n",s_entries);
    char* s=p+ehdr->e_shoff;
    for(int i=0;i<s_entries;i++){
        Elf64_Shdr shdr=*(Elf64_Shdr*)(s+i);
        printf("name of section %d is %s\n",i,shdr.sh_name);
    }
}



void reach_out(const char name[],int size){
    
    FILE* fptr=NULL;
    int fd=open(name,O_RDONLY );
    if(fd==-1){perror("couldn't open file \n");exit(1);}
    struct stat st;
    fstat(fd,&st);
    if((fptr=fopen(name,"r"))==NULL){perror("couldn't fopen file \n");exit(1);}

    char* p=mmap(0,st.st_size,PROT_READ,MAP_SHARED,fd,0);
    
    if(p==MAP_FAILED){perror("mmap");}
    Elf64_Ehdr ehdr=*(Elf64_Ehdr*)p;
    elfheader(fptr,&ehdr,p,st.st_size);

    close(fd);
    fclose(fptr);
    munmap(p,st.st_size);
}




void identify_seg(const Elf64_Phdr* seg){
    Elf64_Word word=seg->p_type;
    switch(word)
    {
        case 0x00000000:
            printf("NULL segment\n");
        break;
        case 0x00000001:
            printf("Loadable segment\n");
        break;
        case 0x00000002:
            printf("Dynamic Linking info segment\n");
        break;
        case 0x00000003:
            printf("Interpreter info segment\n");
        break;
        case 0x00000004:
            printf("Auxiliary info segment\n");
        break;
        case 0x00000005:
            printf("Reserved segment\n");
        break;
        case 0x00000006:
            printf("Program header segment\n");
        break;
        case 0x00000007:
            printf("Thread storage segment\n");
        break;
         case 0x60000000:
            printf(" Reserved OS specific segment\n");
        break;
         case 0x6fffffff:
            printf("Reserved OS specific segment\n");
        break;
         case 0x70000000:
            printf("Reserved Processor specific segment\n");
        break;
         case 0x7fffffff:
            printf("Reserved Processor specific segment\n");
        break;
        
    }
}
void read_prog_header(char* file,unsigned long len,Elf64_Ehdr* ehdr){
    Elf64_Phdr phdr;
    char* p=file+ehdr->e_phoff;
    int i=0;
   FILE* fptr=fopen("text_read.txt","w+");
   FILE* fptr2=fopen("data_read.txt","w+");
   if(fptr==NULL){perror("fopen\n");return;}
    for(i=0;i<ehdr->e_phnum;i++){
        phdr=*(Elf64_Phdr*)p;
        printf("seg %d resides at offset %lu having filesize %lx\n",i,phdr.p_offset,phdr.p_filesz);
        identify_seg(&phdr);
        if(phdr.p_type==PT_LOAD){
            puts("Loading.....\n");
            char* ptr=file+phdr.p_offset;
            printf("This segment resides at %lu in memory & has %lu size in memory & %lu size in elf\n",phdr.p_vaddr,phdr.p_memsz,phdr.p_filesz);
        //  puts("loading data.........\n");
            if((phdr.p_flags&PF_R) && (phdr.p_flags& PF_X)){
                puts("this is text segment\n");
                 fwrite(ptr,sizeof(char),phdr.p_filesz,fptr);
            }
            else if((phdr.p_flags& PF_R ) && ((phdr.p_flags& PF_W ))){
                puts("this is data segment\n");
                fwrite(ptr,sizeof(char),phdr.p_filesz,fptr2);
            }

          
           // for(int i=0;i<phdr.p_filesz;i++){
                //printf("%c",ptr+i);
               
            //}
            puts("\n");
        }
        p+=sizeof(Elf64_Phdr);
        
    }

    fclose(fptr);
    fclose(fptr2);
}

void elfheader(FILE* fptr,Elf64_Ehdr* ehdr,char* p,unsigned long size){
    puts("type---------------------------------------\n");
    
    switch(ehdr->e_type){
        case 0x02:
            puts("executable file\n");
            break;
        case 0x04:
            puts("core file\n");
            break;
        default: 
            printf("other type %x\n",ehdr->e_type);
            break;
    }

    puts("\nmachine---------------------------------------\n");

    switch(ehdr->e_machine){
        case 0x03:
            puts("x86 architecture\n");
            break;
        default:
            printf("other architecture\n");
            break;
    }


    puts("\nEntry address---------------------------------------\n");

    printf("%lu \n",ehdr->e_entry);


   // section_header(ehdr,p,size);
    //read_sec_header(fptr,p,size,ehdr);
    read_prog_header(p,size,ehdr);

}


int main(){
    char name[]="./ex";
    reach_out(name,strlen(name));
    return 0;
}


