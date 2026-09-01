import re


CONTROL_TYPES = {
    "Text": (120, 24), "Button": (90, 30), "RoundButton": (90, 30),
    "Rect": (110, 80), "Vscroll": (140, 120), "Hscroll": (140, 80),
    "Image": (64, 64), "TextBox": (120, 28), "CheckBox": (100, 25),
    "RadioButton": (100, 25), "Switch": (95, 28), "ProgressBar": (120, 18),
    "ListView": (120, 80), "WebView": (140, 80),
}


def tokens(source):
    pattern = re.compile(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"|[A-Za-z_]\w*|0[xX][0-9A-Fa-f]+|-?\d+(?:\.\d+)?|->|::|==|!=|<=|>=|&&|\|\||[^\s]')
    return [item for item in pattern.findall(source) if not item.startswith('//') and not item.startswith('/*')]


def matching(items, start, opening='{', closing='}'):
    depth = 0
    for index in range(start, len(items)):
        if items[index] == opening:
            depth += 1
        elif items[index] == closing:
            depth -= 1
            if depth == 0:
                return index
    return -1


def expression(items):
    return ''.join(items).strip()


def string_value(value):
    if not isinstance(value, str):
        return None
    match = re.fullmatch(r'string\("((?:\\.|[^"\\])*)"\)', value)
    if not match:
        return None
    return bytes(match.group(1), 'utf-8').decode('unicode_escape')


def number_value(value):
    match = re.search(r'-?\d+', value or '')
    return int(match.group()) if match else None


def alignment_value(value):
    if value == '0':
        return 'center'
    flags = {'ANCHOR_LEFT': 'left', 'ANCHOR_RIGHT': 'right', 'ANCHOR_TOP': 'top', 'ANCHOR_BOTTOM': 'bottom'}
    return '_'.join(flags.get(part.strip(), '') for part in value.split('|')).strip('_') or 'left'


def parse_size(value, fallback):
    match = re.fullmatch(r'Size\((\d+)(?:,true)?\)', value or '')
    if not match:
        return fallback, 'px'
    return int(match.group(1)), '%' if ',true' in value else 'px'


def parse_color(value, fallback):
    match = re.fullmatch(r'Color\(0x([0-9A-Fa-f]+)\)', value or '')
    if not match:
        return fallback
    raw = match.group(1).upper().zfill(8)
    return 'transparent' if raw[:2] == '00' else f'#{raw[-6:]}'


def parse_cpp(source):
    items = tokens(source)
    class_index = next((i for i in range(len(items) - 4) if items[i] == 'class' and items[i + 2:i + 5] == [':', 'public', 'Window']), -1)
    if class_index < 0:
        return None
    class_name = items[class_index + 1]
    class_open = next((i for i in range(class_index, len(items)) if items[i] == '{'), -1)
    class_close = matching(items, class_open)
    if class_open < 0 or class_close < 0:
        return None
    declarations = {}
    for i in range(class_open + 1, class_close - 2):
        if items[i] in CONTROL_TYPES and re.fullmatch(r'[A-Za-z_]\w*', items[i + 1]) and items[i + 2] == ';':
            declarations[items[i + 1]] = items[i]
    constructor = next((i for i in range(class_open + 1, class_close - 1) if items[i] == class_name and items[i + 1] == '('), -1)
    if constructor < 0:
        return None
    body_open = next((i for i in range(constructor, len(items)) if items[i] == '{'), -1)
    body_close = matching(items, body_open)
    if body_open < 0 or body_close < 0:
        return None
    assignments = {}
    attachments = []
    i = body_open + 1
    while i < body_close:
        if i + 4 < body_close and items[i:i + 3] == ['this', '-', '>'] and items[i + 4] == '=':
            end = next((j for j in range(i + 5, body_close) if items[j] == ';'), body_close)
            assignments[('', items[i + 3])] = expression(items[i + 5:end])
            i = end + 1
            continue
        if i + 3 < body_close and items[i] in declarations and items[i + 1] == '.':
            end = next((j for j in range(i + 3, body_close) if items[j] == ';'), body_close)
            if i + 3 < end and items[i + 3] == '=':
                assignments[(items[i], items[i + 2])] = expression(items[i + 4:end])
                i = end + 1
                continue
        is_this_call = items[i] == 'this' and items[i + 1:i + 3] == ['-', '>']
        is_control_call = items[i] in declarations and items[i + 1] == '.'
        if i + 5 < body_close and (is_this_call or is_control_call):
            offset = 3 if is_this_call else 1
            if items[i + offset: i + offset + 3] == ['add_control', '(', '&']:
                child = items[i + offset + 3]
                attachments.append((items[i] if items[i] != 'this' else '', child))
        i += 1
    window_title = 'My App'
    window_match = re.search(r'Window\("((?:\\.|[^"\\])*)"\s*,\s*Size\((\d+)\)\s*,\s*Size\((\d+)\)\)', source)
    if window_match:
        window_title = bytes(window_match.group(1), 'utf-8').decode('unicode_escape')
    nodes = []
    for variable, control_type in declarations.items():
        width, width_unit = parse_size(assignments.get((variable, 'width')), CONTROL_TYPES[control_type][0])
        height, height_unit = parse_size(assignments.get((variable, 'height')), CONTROL_TYPES[control_type][1])
        location = assignments.get((variable, 'location'), 'Point(0,0)')
        point = re.fullmatch(r'Point\((-?\d+),(-?\d+)\)', location)
        node = {
            'id': string_value(assignments.get((variable, 'name'))) or variable,
            'type': control_type, 'parent': '', 'x': int(point.group(1)) if point else 0,
            'y': int(point.group(2)) if point else 0, 'width': width, 'height': height,
            'position_anchor': None, 'units': {'x': 'px', 'y': 'px', 'width': width_unit, 'height': height_unit},
            'color': parse_color(assignments.get((variable, 'color')), '#ffffff'),
            'bg_color': parse_color(assignments.get((variable, 'bg_color')), 'transparent'),
            'visible': assignments.get((variable, 'visible'), 'true') == 'true',
            'enabled': assignments.get((variable, 'enabled'), 'true') == 'true',
            'content': string_value(assignments.get((variable, 'content'))) or '', 'font_size': number_value(assignments.get((variable, 'font_size'))) or 14,
            'text_align': alignment_value(assignments.get((variable, 'text_align'), 'ANCHOR_LEFT')), 'checked': assignments.get((variable, 'checked')) == 'true',
            'min': number_value(assignments.get((variable, 'min'))) or 0, 'value': number_value(assignments.get((variable, 'value'))) or 0,
            'max': number_value(assignments.get((variable, 'max'))) or 100, 'bar': assignments.get((variable, 'bar'), 'true') == 'true',
            'radius': number_value(assignments.get((variable, 'radius'))) or 0, 'scroll': False, 'src': string_value(assignments.get((variable, 'source'))) or '',
            'image': string_value(assignments.get((variable, 'image'))) or '', 'mode': ['stretch', 'zoom', 'auto_size', 'center'][number_value(assignments.get((variable, 'mode'))) or 0],
            'url': string_value(assignments.get((variable, 'url'))) or '', 'input_type': string_value(assignments.get((variable, 'type'))) or '', 'HTML': '',
        }
        nodes.append(node)
    by_variable = {variable: node for variable, node in ((variable, next(n for n in nodes if n['id'] == (string_value(assignments.get((variable, 'name'))) or variable))) for variable in declarations)}
    for parent, child in attachments:
        if child in by_variable:
            by_variable[child]['parent'] = by_variable[parent]['id'] if parent else ''
    topbar = assignments.get(('', 'top_bar'), 'true') == 'true'
    return {'window': {'title': window_title, 'class_name': class_name, 'width': '100%', 'height': '100%', 'bg_color': parse_color(assignments.get(('', 'bg_color')), '#ffffff'), 'topbar': topbar}, 'nodes': nodes}
