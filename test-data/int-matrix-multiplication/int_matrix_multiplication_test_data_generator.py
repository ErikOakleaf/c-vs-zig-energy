import random
import numpy as np

def generate_test_data(size_kb=50):
    # Calculate approximate size of each struct
    # struct with two 5x5 int matrices + checksum
    # Each int is typically 4 bytes
    # So each struct is about: 2 matrices * 5 * 5 * 4 bytes + 4 bytes = 204 bytes
    
    # Calculate how many structs we need to reach target size (in KB)
    target_bytes = size_kb * 1024
    bytes_per_struct = 204
    num_structs = target_bytes // bytes_per_struct
    
    # Generate the matrix data - we'll use this for both C and Zig
    matrices = []
    checksums = []
    
    for _ in range(num_structs):
        matrix1 = []
        matrix2 = []
        for _ in range(5):
            matrix1.append([random.randint(-5000, 5000) for _ in range(5)])
            matrix2.append([random.randint(-5000, 5000) for _ in range(5)])
        
        # Convert to numpy arrays for multiplication
        np_matrix1 = np.array(matrix1)
        np_matrix2 = np.array(matrix2)
        
        # Calculate the result of matrix multiplication
        result = np.matmul(np_matrix1, np_matrix2)
        
        # Calculate checksum (sum of all elements in the result)
        checksum = int(np.sum(result))
        
        matrices.append((matrix1, matrix2))
        checksums.append(checksum)
    
    return matrices, checksums, num_structs

def generate_c_header(matrices, checksums, num_structs):
    header = """#ifndef TEST_MATRICES_H
#define TEST_MATRICES_H

// Auto-generated test data for matrix multiplication
// Contains {num_structs} structs with two 5x5 matrices each and a checksum

typedef struct {{
    int matrix1[5][5];
    int matrix2[5][5];
    int result_checksum; // Sum of all elements in the result of matrix1 * matrix2
}} MatrixPair;

MatrixPair test_matrices[{num_structs}] = {{
""".format(num_structs=num_structs)
    
    # Generate the structs
    for i, ((matrix1, matrix2), checksum) in enumerate(zip(matrices, checksums)):
        header += "    {\n"
        
        # Generate matrix1
        header += "        {\n"
        for row_idx, row in enumerate(matrix1):
            header += "            {"
            header += ", ".join(str(val) for val in row)
            header += "}"
            if row_idx < 4:
                header += ","
            header += "\n"
        header += "        },\n"
        
        # Generate matrix2
        header += "        {\n"
        for row_idx, row in enumerate(matrix2):
            header += "            {"
            header += ", ".join(str(val) for val in row)
            header += "}"
            if row_idx < 4:
                header += ","
            header += "\n"
        header += "        },\n"
        
        # Add the result checksum
        header += "        {0}".format(checksum)
        
        header += "\n    }"
        if i < num_structs - 1:
            header += ","
        header += "\n"
    
    header += "};\n\n"
    
    # Add the count of structs
    header += "// Total number of test matrix pairs\n"
    header += "const int NUM_TEST_MATRICES = {};\n\n".format(num_structs)
    
    header += "#endif // TEST_MATRICES_H\n"
    
    return header

def generate_zig_file(matrices, checksums, num_structs):
    zig_code = """// Auto-generated test data for matrix multiplication
// Contains {num_structs} structs with two 5x5 matrices each and a checksum

pub const MatrixPair = struct {{
    matrix1: [5][5]i32,
    matrix2: [5][5]i32,
    result_checksum: i32, // Sum of all elements in the result of matrix1 * matrix2
}};

pub const test_matrices = [_]MatrixPair{{
""".format(num_structs=num_structs)
    
    # Generate the structs
    for i, ((matrix1, matrix2), checksum) in enumerate(zip(matrices, checksums)):
        zig_code += "    .{\n"
        
        # Generate matrix1
        zig_code += "        .matrix1 = .{\n"
        for row in matrix1:
            zig_code += "            .{"
            zig_code += ", ".join(str(val) for val in row)
            zig_code += "},\n"
        zig_code += "        },\n"
        
        # Generate matrix2
        zig_code += "        .matrix2 = .{\n"
        for row in matrix2:
            zig_code += "            .{"
            zig_code += ", ".join(str(val) for val in row)
            zig_code += "},\n"
        zig_code += "        },\n"
        
        # Add the result checksum
        zig_code += "        .result_checksum = {0},\n".format(checksum)
        
        zig_code += "    }"
        if i < num_structs - 1:
            zig_code += ","
        zig_code += "\n"
    
    zig_code += "};\n\n"
    
    # Add the count of structs
    zig_code += "// Total number of test matrix pairs\n"
    zig_code += "pub const NUM_TEST_MATRICES: usize = {};\n".format(num_structs)
    
    return zig_code

# Generate the matrix data with checksums
matrices, checksums, num_structs = generate_test_data(50)

# Generate the C header file
c_header = generate_c_header(matrices, checksums, num_structs)
with open("test_matrices.h", "w") as f:
    f.write(c_header)

# Generate the Zig file
zig_file = generate_zig_file(matrices, checksums, num_structs)
with open("test_matrices.zig", "w") as f:
    f.write(zig_file)

print(f"Generated test_matrices.h and test_matrices.zig with the same test data")
print(f"Number of matrix pairs generated: {num_structs}")
print(f"Each matrix pair includes a checksum of the multiplication result")
