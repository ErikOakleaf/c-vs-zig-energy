import random
import os

# Settings
target_size_kb = 50
target_size_bytes = target_size_kb * 1024

min_array_len = 1
max_array_len = 1000

# Generate arrays of random uint8_t values, staying close to 50KB total
arrays = []
total_bytes = 0

while total_bytes < target_size_bytes:
    length = random.randint(min_array_len, max_array_len)
    if total_bytes + length > target_size_bytes:
        length = target_size_bytes - total_bytes
    data = [random.randint(0, 255) for _ in range(length)]
    arrays.append(data)
    total_bytes += length

# Generate C header content
c_header = '#ifndef CRC32_DATA_H\n#define CRC32_DATA_H\n\n#include <stddef.h>\n#include <stdint.h>\n\n'
c_header += 'typedef struct {\n    size_t size;\n    uint8_t data[%d];\n} U8Array;\n\n' % max(len(arr) for arr in arrays)
c_header += f'#define NUM_ARRAYS {len(arrays)}\n\n'
c_header += 'static const U8Array crc32_data[NUM_ARRAYS] = {\n'

for arr in arrays:
    hex_values = ', '.join(f'0x{v:02X}' for v in arr)
    c_header += f'    {{ {len(arr)}, {{ {hex_values} }} }},\n'

c_header += '};\n\n#endif // CRC32_DATA_H\n'

# Generate Zig file content
zig_content = 'pub const crc32_data = [_][]const u8{\n'

for arr in arrays:
    hex_values = ', '.join(f'0x{v:02X}' for v in arr)
    zig_content += f'    &[_]u8{{ {hex_values} }},\n'

zig_content += '};\n'

# Save to files
output_dir = "output"
os.makedirs(output_dir, exist_ok=True)

with open(os.path.join("crc32_data.h"), "w") as f:
    f.write(c_header)

with open(os.path.join("crc32_data.zig"), "w") as f:
    f.write(zig_content)

print(f"Files generated")
