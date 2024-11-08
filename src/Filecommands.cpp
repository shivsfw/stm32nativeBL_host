#include "Filecommands.h"

std::ifstream file; // Global file stream object

fileops fileobj;
//const char* USER_APP = "C:\\Users\\Shivek\\Particlews\\leisure_practice\\Cpp_concepts\\a1_Host_app_for_STM32F767bootloader\\binaries\\Demo_BL_one_LED_v9.bin";
const char* USER_APP = "C:\\Users\\Shivek\\Particlews\\leisure_practice\\Cpp_concepts\\a1_Host_app_for_STM32F767bootloader\\binaries\\Demo_BL_three_LED_vtest.bin";
const char* OUTPUT_FILE = "C:\\Users\\Shivek\\Particlews\\leisure_practice\\Cpp_concepts\\a1_Host_app_for_STM32F767bootloader\\binaries\\OUT_EMS_40k.bin";
//const char* TEST_FILE = "C:\\Users\\Shivek\\Particlews\\leisure_practice\\Cpp_concepts\\a1_Host_app_for_STM32F767bootloader\\binaries\\TEST_to_read_from_position_and_write_to_flash.bin";
// Function to open the file and calculate its total length in bytes
uint32_t fileops::calc_file_len(const char *filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "\n\n   bin file not found\n";
        throw std::runtime_error("Unable to open the binary file.");
    }

    // Get the length of the file
    uint32_t fileLen = static_cast<uint32_t>(file.tellg());
    return fileLen;
}

// Function to read a given number of bytes from the file into a buffer
uint32_t fileops::read_the_file(std::vector<uint8_t>& buffer, uint32_t len) {
    if (!file.is_open()) {
        throw std::runtime_error("File is not open.");
    }

    buffer.resize(len);
    file.read(reinterpret_cast<char*>(buffer.data()), len);
    return static_cast<uint32_t>(file.gcount()); // Return the number of bytes read
}


// Function to read specific number of bytes starting from a given position
uint32_t fileops::read_from_position(std::vector<uint8_t>& buffer, uint32_t start, uint32_t len) {
    if (!file.is_open()) {
        throw std::runtime_error("File is not open.");
    }

    const uint32_t MAX_READ_SIZE = 248;  // Max bytes to read per command
    uint32_t total_bytes_read = 0;
    buffer.clear();  // Ensure buffer is empty before starting

    // Set the file position to the desired starting point
    file.seekg(start, std::ios::beg);
    if (!file.good()) {
        throw std::runtime_error("Failed to seek to the start position.");
    }

    // Read the file in chunks of MAX_READ_SIZE
    while (total_bytes_read < len) {
        uint32_t bytes_to_read = std::min(MAX_READ_SIZE, len - total_bytes_read);
        std::vector<uint8_t> temp_buffer(bytes_to_read);

        file.read(reinterpret_cast<char*>(temp_buffer.data()), bytes_to_read);
        uint32_t bytes_read = static_cast<uint32_t>(file.gcount());

        if (bytes_read == 0) {
            break;  // End of file reached or no more data to read
        }

        // Append the data to the main buffer
        buffer.insert(buffer.end(), temp_buffer.begin(), temp_buffer.begin() + bytes_read);
        total_bytes_read += bytes_read;
    }

    return total_bytes_read;  // Return total number of bytes read
}

// Function to open the file using the global file stream
void fileops::open_file(const char *filename) {
    file.open(filename, std::ios::binary);
    if (!file) {
        std::cerr << "\n   bin file not found\n";
        throw std::runtime_error("Unable to open the binary file.");
    }
}

// Function to write the buffer to a new output file
void fileops::write_to_file(const char *filename, const std::vector<uint8_t>& buffer) {
    std::ofstream output_file(filename, std::ios::binary);
    if (!output_file) {
        std::cout << "Failed to open output file.";
    }
    output_file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}

bool fileops::append_to_file(const char *filename, const std::vector<uint8_t>& data) {
    std::ofstream outfile(filename, std::ios::binary | std::ios::app);
    if (!outfile) {
        std::cout << "Error: Could not open file " << OUTPUT_FILE << " for appending." << std::endl;
        return false;
    }
    
    outfile.write(reinterpret_cast<const char*>(data.data()), data.size());
    if (!outfile) {
        std::cout << "Error: Writing to file " << OUTPUT_FILE << " failed." << std::endl;
        return false;
    }
    
    //outfile.close();
    return true;
}

// Function to close the global file stream
void fileops::close_the_file() {
    if (file.is_open()) {
        file.close();
    }
}

bool fileops::compare_files(const char *written_file, const char* read_file) {
    uint32_t pos = 0;
    uint32_t len = 0;
    std::vector<uint8_t> buffer1, buffer2;

    // Step 1:  read the input FW file which was written
    open_file(written_file);
    len = calc_file_len(written_file);
    read_from_position(buffer1, pos, len);
    close_the_file();

    // Step 2: read the output file which was read from STM.
    open_file(read_file);
    read_from_position(buffer2, pos, len);
    close_the_file();

    // Step 3: compare the file data
    if(buffer1 == buffer2)  {
        std::cout << "\n Files are same \n";
        return true;
    } else  {
        std::cout << "\n Files didn't match \n";
        return false;
    }  
    return false;
}
