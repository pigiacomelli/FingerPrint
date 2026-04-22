# ELF Watermarking Project Documentation

This project provides a system for injecting and extracting digital watermarks into/from ELF (Executable and Linkable Format) files.

## Core Concepts

### ELF (Executable and Linkable Format)
The standard file format for executables, object code, shared libraries, and core dumps in Unix-like systems. An ELF file consists of:
- **ELF Header**: Contains basic information about the file (architecture, entry point, section header table offset, etc.).
- **Section Header Table**: Contains descriptions of all sections in the file (name, type, offset, size).
- **Sections**: The actual data (code in `.text`, data in `.data`, strings in `.strtab`, etc.).

### Digital Watermarking
The process of embedding information into a digital signal (in this case, binary code) in a way that is difficult to remove. This is often used for ownership verification or tracing.

## How it Works

### 1. Watermark Structure
A watermark consists of:
- **Client ID**: Unique identifier for the user.
- **Timestamp**: When the watermark was generated.
- **Hash**: A SHA-256 hash calculated over the Client ID and Timestamp to ensure integrity.

### 2. Injection Process
The injector (`src/elf/injector.c`) performs the following steps:
1. Loads the original ELF file into memory.
2. Creates a new section named `.my_mark`.
3. Encodes the watermark data into a binary format.
4. Appends the watermark data to the end of the file, aligned to 8 bytes.
5. Updates the **Section Name String Table (`.shstrtab`)** to include the new section name.
6. Appends a new **Section Header** to the Section Header Table describing the `.my_mark` section.
7. Updates the **ELF Header** to reflect the new number of sections and the new offset of the Section Header Table.

### 3. Extraction Process
The extractor (`src/elf/extractor.c`) works by:
1. Loading the ELF file.
2. Iterating through the Section Header Table looking for a section named `.my_mark`.
3. If found, it reads the data from that section.
4. Decodes the data back into a `watermark_t` structure.

## Components

- **`src/crypto/`**: Implements SHA-256 hashing (`generate_hash`).
- **`src/encoding/`**: Handles the conversion between the `watermark_t` struct and a binary stream.
- **`src/elf/`**: Contains the core logic for parsing and modifying ELF files.
- **`include/`**: Header files for all modules.
- **`tests/`**: Test suite to verify encoding, injection, and extraction.

## How to Run

### Compilation
Run `make` to compile the project. This will produce the unified `fingerprint` binary and the test binaries.

### Using the CLI Tool
The project provides a unified tool named `fingerprint`.

#### Inject a Watermark
To inject a watermark into an ELF file:
```bash
./fingerprint inject <input_elf> <output_elf> <client_id>
```
Example:
```bash
./fingerprint inject samples/target_app build/target_app_marked "my_client_id"
```

#### Extract a Watermark
To extract a watermark from an ELF file:
```bash
./fingerprint extract <elf_file>
```
Example:
```bash
./fingerprint extract build/target_app_marked
```

### Running Tests
- `./test_encoding`: Verifies that watermarks can be encoded and decoded correctly.
- `./test_injector`: Injects a watermark into a sample binary (legacy test).
- `./test_parser <file>`: Lists the sections of an ELF file.
- `make tests`: Runs the automated test suite.
