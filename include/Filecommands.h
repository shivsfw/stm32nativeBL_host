#ifndef FCMD_H
#define FCMD_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
//#include <stdexcept>

extern const char* USER_APP;
extern const char* OUTPUT_FILE;
extern const char* TEST_FILE;

class fileops   {
    public:
    uint32_t calc_file_len(const char *filename);
    void open_file(const char *filename);
    uint32_t read_the_file(std::vector<uint8_t>& buffer, uint32_t len);
    void close_the_file();
    void write_to_file(const char *filename, const std::vector<uint8_t>& buffer);
    bool append_to_file(const char *filename, const std::vector<uint8_t>& data);
    uint32_t read_from_position(std::vector<uint8_t>& buffer, uint32_t start, uint32_t len);
    bool compare_files(const char *, const char *);
};


#endif