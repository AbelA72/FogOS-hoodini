#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "user/user.h"


char buffer[2048];

/**
* Copies a file to a dest
* @param  source      file we want to copy
* @param  destination where we want the copied to go
* @param  verbose     flag for checking if we want the info for how much is copied, from where and to where
**/
void 
copy_file(const char *source, const char *destination, int verbose) 
{

	int src, dest;

	// opens source
	if ((src = open(source, O_RDONLY)) < 0) {
		fprintf(2, "cp: unable to open source file: %s\n", source);
		exit(1);
	}
	// opens destination
	if ((dest = open(destination, O_WRONLY | O_CREATE | O_TRUNC)) < 0) {
		fprintf(2, "cp: unable to open destination file: %s\n", destination);
		close(src);
		exit(1);
	}

    uint bytesRead;
    uint copied = 0;
    int totalSize = sizeof(source);

	// reads bytes into buffer and prints how much is in the buffer
    while ((bytesRead = read(src, buffer, sizeof(buffer))) > 0) {
        write(dest, buffer, bytesRead);
        
        printf("\nCopying: %d bytes out of total\n", bytesRead/totalSize * 100);

        copied += bytesRead;
        
                        
            
    }
	// handles the verbose flag
	// tells the source and dest file 
	// prints total bytes
    if (verbose) {
        printf("Copied file from '%s' to '%s'\n", source, destination);
        printf("Copied %d bytes\n", copied);
    }
	// closes fd's
	close(src);
	close(dest);

}





/**
* Recursively copies a directory to where you want
* @param  src     directory we want to copy
* @param  dest    what we want the copied directory to be
* @param  verbose flag for checking if we want the info for how much is copied, from where and to where
**/
void 
copy_directory(const char *src, const char *dest, int verbose) 
{

	char buf[512], *p, *d;
	char dest_buf[512];
	int src_fd;
	struct dirent de;
	struct stat st;

	
	// stats source
	if (stat(src, &st) < 0) {
		fprintf(2, "cp: cannot stat %s\n", src);
	    exit(1);
	}

	// makes directory for destination
	if (mkdir(dest) < 0) {
		fprintf(2, "cp: unable to create destination directory: %s\n", dest);
		exit(1);
	}

	// opens source
	if ((src_fd = open(src, O_RDONLY)) < 0) {
		fprintf(2, "cp: unable to open source directory: %s\n", src);
		exit(1);
	}

	// stats source fd
	if(fstat(src_fd, &st) < 0){
		    fprintf(2, "ls: cannot stat %s\n", src);
		    close(src_fd);
		    return;
	}


	if(strlen(src) + 1 + DIRSIZ + 1 > sizeof buf){
		printf("ls: path too long\n");
		exit(1);
	}
	
		// sets source path
	    strcpy(buf, src);
	    p = buf+strlen(buf);
	    *p++ = '/';

		// sets dest path
	    strcpy(dest_buf, dest);  
	    d = dest_buf + strlen(dest_buf);  
	    *d++ = '/'; 

	    
	    while(read(src_fd, &de, sizeof(de)) == sizeof(de)){
	      if(de.inum == 0){
	        continue;
		  }
	      // updates source path  
	      memmove(p, de.name, DIRSIZ);
	      p[DIRSIZ] = 0;


	      // updates dest path  	      
	      if (de.name[0] == '.') {
		      continue;
	      }
	      
	      memmove(d, de.name, DIRSIZ); 
	      d[DIRSIZ] = 0;
	          
	      // stats buff
	      if(stat(buf, &st) < 0){
	        printf("ls: cannot stat %s\n", buf);
	        continue;
	      }
	      // calls copy on file
	      copy_file(buf, dest_buf, verbose);

	      
	    }


	// closes source
	close(src_fd);
}



 

 int
 main(int argc, char *argv[])
 {
	// checks arg number
 	if (argc < 3){
 		fprintf(2, "Usage: /copy <-r> <-v> file_source file_destination\n");
 		exit(1);
 	} 
	// sets params
    int recursive = 0;
    int verbose = 0;
    char *src= 0;
    char *dest = 0;

	// processes command-line arguments
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

    // checks if the source is a directory and if the -r flag was provided
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
        copy_file(src, dest, verbose);
    }

    exit(0);

 }
