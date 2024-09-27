#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "user/user.h"


char buffer[2048];

// Function to copy a file
void 
copy_file(const char *source, const char *destination, int verbose) 
{

	int src, dest;


	if ((src = open(source, O_RDONLY)) < 0) {
		fprintf(2, "cp: unable to open source file: %s\n", source);
		exit(1);
	}

	if ((dest = open(destination, O_WRONLY | O_CREATE | O_TRUNC)) < 0) {
		fprintf(2, "cp: unable to open destination file: %s\n", destination);
		close(src);
		exit(1);
	}

    uint bytesRead;
    uint copied = 0;
    int totalSize = sizeof(source);

    while ((bytesRead = read(src, buffer, sizeof(buffer))) > 0) {
        write(dest, buffer, bytesRead);
        copied += bytesRead;
        // Calculates and displays progress
		if (totalSize > 0) {
           int progress = (int)copied / totalSize * 100;
           printf("\nCopying: %d%\n", progress);
       	}
                        
            
    }

    if (verbose) {
        printf("Copied file from '%s' to '%s'\n", source, destination);
        printf("Copied %d bytes\n", copied);
    }

	close(src);
	close(dest);

}





// Function to recursively copy a directory
void 
copy_directory(const char *src, const char *dest, int verbose) 
{

	char buf[512], *p;
	char buff[512];
	int src_fd;
	struct dirent de;
	struct stat st;
	struct stat desti;

	int progress = 0;
	

	if (stat(src, &st) < 0) {
		fprintf(2, "cp: cannot stat %s\n", src);
	    exit(1);
	}


	// Create destination directory
	if (mkdir(dest) < 0) {
		fprintf(2, "cp: unable to create destination directory: %s\n", dest);
		exit(1);
	}

	if ((src_fd = open(src, O_RDONLY)) < 0) {
		fprintf(2, "cp: unable to open source directory: %s\n", src);
		exit(1);
	}

	if(fstat(src_fd, &st) < 0){
		    fprintf(2, "ls: cannot stat %s\n", src);
		    close(src_fd);
		    return;
	}


	if(strlen(src) + 1 + DIRSIZ + 1 > sizeof buf){
		printf("ls: path too long\n");
		exit(1);
	}
	    strcpy(buf, src);
	    p = buf+strlen(buf);
	    *p++ = '/';
	    while(read(src_fd, &de, sizeof(de)) == sizeof(de)){
	      if(de.inum == 0)
	        continue;
	      memmove(p, de.name, DIRSIZ);
	      p[DIRSIZ] = 0;
	      if(stat(buf, &st) < 0){
	        printf("ls: cannot stat %s\n", buf);
	        continue;
	      }
		  if (stat(dest, &desti) < 0) {
		          fprintf(2, "cp: cannot stat %s\n", src);
		          exit(1);
		   }

		   
	      if (desti.type == T_DIR){
	        strcpy(buff, buf);
	        strcpy(buff, p + 1);
	        printf("new dest %s", buff);
	      	copy_file(p, buff, verbose);
	      }
	      
	    }

	
	if (verbose) {
		printf("Copied directory from '%s' to '%s'\n", src, dest);
			printf("Copied %d bytes\n", progress);
	}
	

	
	close(src_fd);
}



 

 int
 main(int argc, char *argv[])
 {

 	if (argc < 3){
 		fprintf(2, "Usage: /copy <-r> <-v> file_source file_destination\n");
 		exit(1);
 	} 

    int recursive = 0;
    int verbose = 0;
    char *src= 0;
    char *dest = 0;

	// Process command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } 
        else if (strcmp(argv[i], "-r") == 0) {
        	recursive = 1;
        } 
        else {
                src = argv[i];
                dest = argv[i+1];
                break;
        }
    }

	struct stat st;
    if (stat(src, &st) < 0) {
        fprintf(2, "cp: cannot stat %s\n", src);
        exit(1);
    }

    // Check if the source is a directory and if the -r flag was provided
    if (st.type == T_DIR) {
        if (recursive) {
        	copy_directory(src, dest, verbose);
        } 
        else {
            fprintf(2, "cp: %s is a directory, use -r to copy directories\n", src);
            exit(1);
        }
    } 
    else {
    	// Source is a file, just copy the file
        copy_file(src, dest, verbose);
    }

    exit(0);

 }
