#ifndef OPCODE
#define OPCODE

#define type_client_master    777
#define type_client_manager   666
#define type_client_slave     420

#define open_call             1
#define close_call            2
#define read_call             3
#define write_call            4
#define seek_call             5
#define exec_call             6

// Commands
#define c_claim               20
#define c_release             21
#define c_list_slaves         22
#define c_masterlist          23
#define c_master_release      24
#define c_master_set_dbglvl   25
#define c_master_kill_client  99

#include <sys/stat.h>
#include <sys/socket.h>

// MetaData for read call
typedef struct md_open{
     int flags;
     mode_t mode;
     int str_len;   // Length of the path string following the OpCode Header
}md_open;

// MetaData for close call
typedef struct md_close{
     int close_fd;
}md_close;

// MetaData for close call
typedef struct md_read{
     int read_fd;
     void* buffer;
     size_t count;  // Note that size_t on one machine might not match on another. Stick to 32 bits to be safe
}md_read;

// MetaData for write call
typedef struct md_write{
     int write_fd;
     const void* buffer;
     size_t count;
}md_write;

// MetaData for lseek call
typedef struct md_seek{
     int seek_fd;
     off_t offset;
     int whence;
}md_seek;

// MetaData for exec call
typedef struct md_exec{
     int str_len; // Length of the string following the header
}md_exec;


// General Op Code Header
typedef struct OpHeader{
     int opcode;
     
     // MetaData for each specific call
     union{
          md_open   open_c;
          md_close  close_c;
          md_read   read_c;
          md_write  write_c;
          md_seek   seek_c;
          md_exec   exec_c;
     };

}OpHeader;

#endif
