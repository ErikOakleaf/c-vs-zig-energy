import random
import numpy as np

def generate_test_data(size_kb=50):
    # Calculate approximate size of each struct
    # struct with two 5x5 matrices of doubles + checksum
    # Each double is typically 8 bytes
    # So each struct is about: 2 matrices * 5 * 5 * 8 bytes + 8 bytes = 408 bytes
    
    # Calculate how many structs we need to reach target size (in KB)
    target_bytes = size_kb * 1024
    bytes_per_struct = 408
    num_structs = target_bytes // bytes_per_struct
    
    matrices = []
    checksums = []
    
    for _ in range(num_structs):
        matrix1 = []
        matrix2 = []
        for _ in range(5):
            # Generate random floats with decimals; for example, between -5000.0 and 5000.0
            matrix1.append([random.uniform(-5000, 5000) for _ in range(5)])
            matrix2.append([random.uniform(-5000, 5000) for _ in range(5)])
        
        # Convert to numpy arrays for multiplication using double precision
        np_matrix1 = np.array(matrix1, dtype=np.float64)
        np_matrix2 = np.array(matrix2, dtype=np.float64)
        
        # Calculate the result of matrix multiplication
        result = np.matmul(np_matrix1, np_matrix2)
        
        # Calculate checksum (sum of all elements in the result) as a double
        checksum = float(np.sum(result))
        
        matrices.append((matrix1, matrix2))
        checksums.append(checksum)
    
    return matrices, checksums, num_structs

def generate_c_header(matrices, checksums, num_structs):
    header = """#ifndef TEST_MATRICES_H
#define TEST_MATRICES_H

// Auto-generated test data for matrix multiplication
// Contains {num_structs} structs with two 5x5 matrices of doubles and a checksum

typedef struct {{
    double matrix1[5][5];
    double matrix2[5][5];
    double result_checksum; // Sum of all elements in the result of matrix1 * matrix2
}} MatrixPair;

MatrixPair test_matrices[{num_structs}] = {{
""".format(num_structs=num_structs)
    
    for i, ((matrix1, matrix2), checksum) in enumerate(zip(matrices, checksums)):
        header += "    {\n"
        
        header += "        {\n"
        for row_idx, row in enumerate(matrix1):
            header += "            {"
            header += ", ".join(f"{val:.5f}" for val in row)
            header += "}"
            if row_idx < 4:
                header += ","
            header += "\n"
        header += "        },\n"
        
        header += "        {\n"
        for row_idx, row in enumerate(matrix2):
            header += "            {"
            header += ", ".join(f"{val:.5f}" for val in row)
            header += "}"
            if row_idx < 4:
                header += ","
            header += "\n"
        header += "        },\n"
        
        header += "        {0:.5f}".format(checksum)
        
        header += "\n    }"
        if i < num_structs - 1:
            header += ","
        header += "\n"
    
    header += "};\n\n"
    header += "// Total number of test matrix pairs\n"
    header += "const int NUM_TEST_MATRICES = {};\n\n".format(num_structs)
    header += "#endif // TEST_MATRICES_H\n"
    
    return header

def generate_zig_file(matrices, checksums, num_structs):
    zig_code = """// Auto-generated test data for matrix multiplication
// Contains {num_structs} structs with two 5x5 matrices of f64 and a checksum

pub const MatrixPair = struct {{
    matrix1: [5][5]f64,
    matrix2: [5][5]f64,
    result_checksum: f64, // Sum of all elements in the result of matrix1 * matrix2
}};

pub const test_matrices = [_]MatrixPair{{
""".format(num_structs=num_structs)
    
    for i, ((matrix1, matrix2), checksum) in enumerate(zip(matrices, checksums)):
        zig_code += "    .{\n"
        
        zig_code += "        .matrix1 = .{\n"
        for row in matrix1:
            zig_code += "            .{" + ", ".join(f"{val:.5f}" for val in row) + "},\n"
        zig_code += "        },\n"
        
        zig_code += "        .matrix2 = .{\n"
        for row in matrix2:
            zig_code += "            .{" + ", ".join(f"{val:.5f}" for val in row) + "},\n"
        zig_code += "        },\n"
        
        zig_code += "        .result_checksum = {0:.5f},\n".format(checksum)
        zig_code += "    }"
        if i < num_structs - 1:
            zig_code += ","
        zig_code += "\n"
    
    zig_code += "};\n\n"
    zig_code += "// Total number of test matrix pairs\n"
    zig_code += "pub const NUM_TEST_MATRICES: usize = {};\n".format(num_structs)
    
    return zig_code

# Generate the matrix data with decimal values
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
