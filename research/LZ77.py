def find_longest_match(data, current_position, window_size):
    end_of_buffer = min(current_position + window_size, len(data))

    best_length = 0
    best_offset = 0

    search_start = max(0, current_position - window_size)

    for j in range(search_start, current_position):
        length = 0

        while (current_position + length < len(data) and
               data[j + length] == data[current_position + length]):
            length += 1

        if length > best_length:
            best_length = length
            best_offset = current_position - j

    return best_offset, best_length


def LZ77(path, window_size=20):
    with open(path, 'r') as f:
        data = f.read()

    i = 0
    output = []

    while i < len(data):
        offset, length = find_longest_match(data, i, window_size)

        if length > 0:
            next_char = data[i + length] if i + length < len(data) else ''
        else:
            next_char = data[i]

        output.append((offset, length, next_char))
        i += length + 1

    return output
