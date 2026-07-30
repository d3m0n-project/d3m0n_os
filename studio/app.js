const canvas = document.querySelector('#canvas'),
    tree = document.querySelector('#tree'),
    inspector = document.querySelector('#inspector'),
    templatesEl = document.querySelector('#templates');

const startupModal = document.getElementById("startup-modal");
const applicationList = document.getElementById("application-list");
var cached_apps = undefined;
let currentAppFS = null;
let currentLayoutPath = null;
let currentSrcPath = null;
let modifiedFiles = {};
let config = {};

function flattenFiles(entries, prefix = "", fs = {})
{
    for (const entry of entries)
    {
        if (!Array.isArray(entry))
            continue;

        const name = entry[0];
        if (entry.length === 2 && typeof entry[1] === "string")
        {
            fs[prefix + name] = entry[1];
            continue;
        }
        if (typeof name === "string")
        {
            flattenFiles(entry.slice(1), prefix + name + "/", fs);
        }
    }
    return fs;
}

function openLayout(path)
{
    const content=currentAppFS[path];
    if (!content)
        return;

    currentLayoutPath=path;
    currentSrcPath=getSrcForLayout(path);
    parse(content);
    document.querySelector("#app-label").textContent = path;
}

function getLayoutFiles()
{
    return Object.keys(currentAppFS)
        .filter(x => x.startsWith("layouts/"));
}


function getSrcForLayout(layout)
{
    let name = layout
        .split("/")
        .pop()
        .replace(".layout", ".src");

    let candidate = "src/" + name;
    if (currentAppFS[candidate])
        return candidate;

    candidate = layout.replace(".layout", ".src");
    if (currentAppFS[candidate])
        return candidate;

    return null;
}

function findFile(tree, filename) {
    for (const entry of tree) {
        if (Array.isArray(entry)) {
            // Folder
            if (typeof entry[0] === "string" && entry.length > 1 && !Array.isArray(entry[1]) && !Array.isArray(entry[1]?.[0])) {
                // continue
            }

            if (typeof entry[0] === "string") {
                // File tuple?
                if (entry.length === 2 && typeof entry[1] === "object" && entry[1]?.type === "image" && entry[0] === filename)
                    return entry[1];

                if (entry.length === 2 && typeof entry[1] === "string" && entry[0] === filename)
                    return entry[1];

                // Folder
                const r = findFile(entry.slice(1), filename);
                if (r) return r;
            }
        }
    }
    return null;
}

async function loadAppFilesystem(app)
{
    currentAppFS = {};

    flattenFiles(app.files);
    console.log(currentAppFS);
    currentLayoutPath = "layouts/main.layout";
    if (!currentAppFS[currentLayoutPath])
    {
        console.error("Missing main layout");
        return;
    }
    currentSrcPath = "src/main.src";
    parse(currentAppFS[currentLayoutPath]);
    currentApplication = app;
}

async function loadApplications()
{
    if (!startupModal.classList.contains("open"))
        startupModal.classList.add("open");
    if (!cached_apps)
    {
        const res = await fetch("/api/applications");
        var apps = await res.json();
        cached_apps = apps;
    }
    else
        apps = cached_apps;

    applicationList.innerHTML = "";
    for (const app of apps)
    {
        let manifest = {'name': '', 'icon': ''};
        try {
            let lines = app.manifest.split('\n');
            await lines.forEach(l => {
                let k=l.split(':')[0].trim();
                let v=l.split(':')[1].trim();
                if (k == 'name')
                    manifest.name = v;
                else if (k == 'icon')
                    manifest.icon = v;
                else
                {
                    console.error(`Invalid manifest entry: '${k}'!`);
                    return;
                }
            });
        } catch {
            continue;
        }

        const card = document.createElement("div");
        card.className = "application";
        const img = document.createElement("img");
        let icon = null;
        if (manifest.icon)
            icon = findFile(app.files, manifest.icon);

        if (icon && icon.type === "image")
            img.src = `data:${icon.mime};base64,${icon.data}`;
        else
            img.src = "data:image/svg+xml;charset=utf-8," + encodeURIComponent(`
                <svg xmlns="http://www.w3.org/2000/svg" width="64" height="64">
                    <rect width="64" height="64" rx="8" fill="#444"/>
                    <text x="32" y="39" text-anchor="middle" font-size="28">📦</text>
                </svg>`);

        const info = document.createElement("div");
        info.innerHTML = `
            <div class="name">${manifest.name || "Unnamed App"}</div>
            <div class="description">${manifest.description || ""}</div>
        `;

        card.append(img, info);
        card.onclick = () => {
            window.currentApplication = app;
            window.currentManifest = manifest;

            currentAppFS = flattenFiles(app.files);

            startupModal.classList.remove("open");

            currentLayoutPath = "layouts/main.layout";
            currentSrcPath = "src/main.src";

            parse(currentAppFS[currentLayoutPath]);
        };
        applicationList.appendChild(card);
    }

    if (!apps.length) {
        applicationList.innerHTML =
            '<div class="empty">No applications found.</div>';
    }
}

const controls = {
    Text: {
        w: 120,
        h: 24,
        content: 'Text',
        fields: ['content', 'font_size', 'text_align']
    },
    Button: {
        w: 90,
        h: 30,
        content: 'Button',
        fields: ['content', 'font_size', 'text_align', 'image']
    },
    RoundButton: {
        w: 90,
        h: 30,
        content: 'Round button',
        fields: ['content', 'font_size', 'text_align', 'image', 'radius']
    },
    Rect: {
        w: 110,
        h: 80,
        fields: ['radius']
    },
    Vscroll: {
        w: 140,
        h: 120,
        fields: ['bar']
    },
    Hscroll: {
        w: 140,
        h: 80,
        fields: ['bar']
    },
    Image: {
        w: 64,
        h: 64,
        fields: ['src', 'mode']
    },
    TextBox: {
        w: 120,
        h: 28,
        content: '',
        fields: ['content', 'font_size', 'text_align']
    },
    CheckBox: {
        w: 100,
        h: 25,
        content: 'CheckBox',
        fields: ['content', 'checked', 'font_size', 'text_align']
    },
    RadioButton: {
        w: 100,
        h: 25,
        content: 'RadioButton',
        fields: ['content', 'checked', 'font_size', 'text_align']
    },
    Switch: {
        w: 95,
        h: 28,
        content: 'Switch',
        fields: ['content', 'checked', 'font_size', 'text_align']
    },
    ProgressBar: {
        w: 120,
        h: 18,
        fields: ['min', 'value', 'max']
    },
    ListView: {
        w: 120,
        h: 80,
        fields: []
    },
    WebView: {
        w: 140,
        h: 80,
        fields: ['url']
    },
    RawHtml: {
        w: 120,
        h: 60,
        fields: ['HTML']
    }
};
let model = {
        window: {
            title: 'My App',
            width: '100%',
            height: '100%',
            bg_color: '#ffffff',
            topbar: true
        },
        nodes: [],
        templates: [],
        images: []
    },
selectedId = 'window',
    drag = null,
    resize = null;

const uid = () => `control_${Math.random().toString(36).slice(2,8)}`,
    byId = id => model.nodes.find(n => n.id === id),
    kids = id => model.nodes.filter(n => n.parent === id),
    esc = s => String(s ?? '').replaceAll('&', '&amp;').replaceAll('<', '<').replaceAll('>', '>').replaceAll('"', '"');

const numericKeys = new Set(['x', 'y', 'width', 'height', 'min', 'value', 'max', 'font_size', 'radius']);
const textAlignments = {
    top: {x: 'center', y: 'flex-start', text: 'center'},
    top_left: {x: 'flex-start', y: 'flex-start', text: 'left'},
    top_right: {x: 'flex-end', y: 'flex-start', text: 'right'},
    left: {x: 'flex-start', y: 'center', text: 'left'},
    center: {x: 'center', y: 'center', text: 'center'},
    right: {x: 'flex-end', y: 'center', text: 'right'},
    bottom: {x: 'center', y: 'flex-end', text: 'center'},
    bottom_left: {x: 'flex-start', y: 'flex-end', text: 'left'},
    bottom_right: {x: 'flex-end', y: 'flex-end', text: 'right'}
};
const wholeOrNull = value => {
    if (value === '' || value === null || value === undefined) return null;
    const number = Number(value);
    return Number.isFinite(number) && number >= 0 ? Math.round(number) : null
};
function normalizeNodeNumbers(n) {
    numericKeys.forEach(key => n[key] = wholeOrNull(n[key]));
}

const px = (v, u, d) => u === '%' ? Number(v || 0) * d / 100 : Number(v || 0),
    val = (p, u, d) => u === '%' ? Math.round(p / d * 100) : Math.round(p),
    fmt = (v, u) => v === null || v === undefined ? '' : `${v}${u}`;
const pickerColor = value => {
    const known = {white: '#ffffff', black: '#000000', transparent: '#000000', dark_grey: '#4b4b4b', grey: '#808080', gray: '#808080', red: '#ff0000', green: '#008000', blue: '#0000ff', orange: '#ffa500', yellow: '#ffff00', purple: '#800080'};
    return /^#[0-9a-f]{6}$/i.test(value || '') ? value : (known[String(value || '').toLowerCase()] || '#000000')
};

function colorizeSvg(data, color) {
    try {
        const comma = data.indexOf(',');
        const header = data.slice(0, comma);
        const payload = data.slice(comma + 1);
        const source = header.includes(';base64') ? atob(payload) : decodeURIComponent(payload);
        const document = new DOMParser().parseFromString(source, 'image/svg+xml');
        const svg = document.documentElement;
        if (svg.localName !== 'svg') return data;
        svg.setAttribute('fill', color);
        svg.style.fill = color;
        const result = new XMLSerializer().serializeToString(svg);
        return `data:image/svg+xml;base64,${btoa(unescape(encodeURIComponent(result)))}`;
    } catch {
        return data;
    }
}

function parentSize(n) {
    const p = byId(n.parent);
    return p ? size(p) : {
        w: 320,
        h: 480
    };
}

function size(n) {
    const p = parentSize(n);
    return {
        w: px(n.width, n.units.width, p.w),
        h: px(n.height, n.units.height, p.h)
    }
}

function parentOffset(n) {
    if (!n.parent) return {
        x: 0,
        y: 0
    };
    const p = byId(n.parent);
    if (!p) return {
        x: 0,
        y: 0
    };
    const o = parentOffset(p),
        pp = parentSize(p);
    return {
        x: o.x + px(p.x, p.units.x, pp.w),
        y: o.y + px(p.y, p.units.y, pp.h)
    }
}

function node(type, over = {}) {
    const c = controls[type];
    return {
        id: uid(),
        type,
        parent: '',
        x: 0,
        y: 0,
        width: c.w,
        height: c.h,
        position_anchor: null,
        units: {
            x: 'px',
            y: 'px',
            width: 'px',
            height: 'px'
        },
        color: '#ffffff',
        bg_color: 'transparent',
        visible: true,
        enabled: true,
        content: c.content || '',
        font_size: 14,
        text_align: 'left',
        checked: false,
        min: 0,
        value: 45,
        max: 100,
        bar: true,
        radius: type === 'RoundButton' ? 15 : 0,
        scroll: false,
        src: '',
        image: '',
        mode: 'stretch',
        url: '',
        HTML: '',
        ...over
    }
}

function add(type) {
    const n = node(type);
    normalizeNodeNumbers(n);
    model.nodes.push(n);
    selectedId = n.id;
    render();
}

function preview(n) {
    if (['Button', 'RoundButton'].includes(n.type))
		return 'button ' + (n.type === 'RoundButton' ? 'round' : '');
    if (['Rect', 'Vscroll', 'Hscroll', 'ListView'].includes(n.type))
		return 'box ' + (n.type.endsWith('scroll') ? 'scroll' : '');
    if (n.type === 'Image')
		return 'image';
    if (n.type === 'ProgressBar')
		return 'progress';
    return 'text'
}

function nodeEl(n) {
    const o = parentOffset(n),
        p = parentSize(n),
        s = size(n),
        el = document.createElement('div');
    el.className = 'node' + (selectedId === n.id ? ' selected' : '');
    el.dataset.id = n.id;
    el.style.cssText = `left:${o.x+px(n.x,n.units.x,p.w)}px;top:${o.y+px(n.y,n.units.y,p.h)}px;width:${s.w}px;height:${s.h}px;color:${n.color === 'transparent' ? 'transparent' : pickerColor(n.color)};background:${n.bg_color==='transparent'?'transparent':pickerColor(n.bg_color)};border-radius:${n.radius || 0}px;display:${n.visible?'block':'none'};z-index:${model.nodes.indexOf(n)+1}`;
    const v = document.createElement(['Button', 'RoundButton'].includes(n.type) ? 'button' : (n.type === 'TextBox' ? 'textarea' : 'div'));
    v.className = 'preview ' + preview(n);
    if (n.type === 'TextBox') {
        v.disabled = true;
        v.setAttribute('aria-label', 'Text box preview');
    }
    const alignment = textAlignments[n.text_align] || textAlignments.left;
    v.style.cssText = `font-size:${n.font_size}px;font-weight:normal;text-align:${alignment.text};display:flex;justify-content:${alignment.x};align-items:${alignment.y};border-radius:${n.radius||0}px`;
    if (n.type === 'Image') {
        const image = (model.images || []).find(item => item.path === n.src);
        const imageModes = {
            stretch: {size: '100% 100%', position: 'center'},
            zoom: {size: 'contain', position: 'center'},
            auto_size: {size: 'auto', position: 'top left'},
            center: {size: 'auto', position: 'center'}
        };
        const mode = imageModes[n.mode] || imageModes.stretch;
        const isSvg = image && (image.type === 'image/svg+xml' || /\.svg$/i.test(image.path));
        const imageData = isSvg ? colorizeSvg(image.data, n.color || '#111111') : image?.data;
        v.style.backgroundImage = imageData ? `url("${imageData}")` : '';
        v.style.backgroundSize = mode.size;
        v.style.backgroundPosition = mode.position;
        v.style.backgroundRepeat = 'no-repeat';
    }
    else if (n.type === 'ProgressBar') v.innerHTML = `<i style="width:${Math.min(100,Math.max(0,100*n.value/(n.max||100)))}%"></i>`;
    else if (n.type === 'WebView') v.textContent = n.url || 'WebView';
    else if (n.type === 'RawHtml') v.textContent = n.HTML || 'Raw HTML';
    else if (n.type === 'ListView') v.innerHTML = '<span style="padding:5px;display:block">ListView</span>';
    else if (n.type === 'TextBox') v.value = n.content || '';
    else if (['CheckBox', 'RadioButton', 'Switch'].includes(n.type)) v.innerHTML = `<span style="padding:4px;display:block">${n.checked?'☑':'☐'} ${esc(n.content)}</span>`;
    else if (!['Rect', 'Vscroll', 'Hscroll'].includes(n.type)) v.textContent = n.content || n.type;
    el.append(v);
    if (selectedId === n.id) {
        for (const h of ['nw','ne','sw','se']) {
            const hd = document.createElement('div');
            hd.className = 'node-handle';
            hd.dataset.handle = h;
            hd.addEventListener('pointerdown', e => startResize(e, n, h));
            el.append(hd)
        }
    }
    el.addEventListener('pointerdown', startDrag);
    return el
}

function drawCanvas() {
    canvas.style.background = model.window.bg_color === 'transparent' ? 'transparent' : pickerColor(model.window.bg_color || '#fff');
    canvas.replaceChildren(...model.nodes.map(nodeEl))
}

function drawAppFilesystem()
{
    const old = document.querySelector("#app-files");
    if (old)
        old.remove();

    if (!currentAppFS)
        return;

    const container = document.createElement("div");
    container.id="app-files";
    container.className="section";
    container.innerHTML="<h2>Files</h2>";
    const tree=document.createElement("div");
    tree.className="tree";

    Object.keys(currentAppFS)
    .filter(x=>x.startsWith("layouts/"))
    .forEach(file=>{
        const row=document.createElement("div");
        row.className="tree-row";
        row.innerHTML=`
            <span class="kind">Layout</span>
            <span class="name">${file}</span>
        `;

        row.onclick=()=>openLayout(file);
        tree.append(row);
    });

    container.append(tree);
    document.querySelector(".left").prepend(container);
}

function drawTree() {
    tree.innerHTML = '';
    const root = document.createElement('div');
    root.className = 'tree-row' + (selectedId === 'window' ? ' active' : '');
    root.innerHTML = '<span class="kind">Window</span><span class="name">App root</span>';
    root.onclick = () => select('window');
    tree.append(root);
    const rec = (n, d) => {
        const r = document.createElement('div');
        r.className = 'tree-row' + (selectedId === n.id ? ' active' : '');
        r.style.paddingLeft = `${5+d*16}px`;
        r.innerHTML = `<span class="kind">${n.type}</span><span class="name">${esc(n.id)}</span>`;
        r.onclick = () => select(n.id);
        tree.append(r);
        kids(n.id).forEach(x => rec(x, d + 1))
    };
    kids('').forEach(n => rec(n, 1))
}

function drawTemplates() {
    templatesEl.innerHTML = '';
    model.templates.forEach(t => {
        const r = document.createElement('div');
        r.className = 'tree-row';
        const params = templateParams(t);
        if (!t.values) t.values = {};
        r.innerHTML = `<span class="kind">Template</span><span class="name">${esc(t.name)}</span><button style="margin-left:auto;padding:3px 6px">Insert</button>${params.length ? `<div style="width:100%;display:grid;gap:4px;padding:5px 0">${params.map(p => `<label style="font-size:11px">$${p}<input data-template-param="${p}" value="${esc(t.values[p]||'')}"></label>`).join('')}</div>` : ''}`;
        r.querySelector('button').onclick = e => {
            e.stopPropagation();
            insertTemplate(t)
        };
        r.querySelectorAll('[data-template-param]').forEach(input => input.onchange = () => {
            t.values[input.dataset.templateParam] = input.value;
            model.nodes.filter(n => n.templateName === t.name).forEach(n => n.templateValues = {...t.values});
            synchronizeTemplate(t);
            render()
        });
        templatesEl.append(r)
    });
    if (!model.templates.length) templatesEl.innerHTML = '<div class="empty">No templates yet.</div>'
}

function field(label, key, value, type = 'text', extra = '') {
    if (type === 'color') return `<div class="field"><label>${label}</label><div class="color-field"><input type="color" value="${pickerColor(value)}" data-key="${key}" ${extra}><label><input type="checkbox" data-transparent="${key}" ${value === 'transparent' ? 'checked' : ''}>Transparent</label></div></div>`;
    return `<div class="field"><label>${label}</label><input ${type==='checkbox'?'type="checkbox" '+(value?'checked':''): `type="${type}" value="${esc(value)}"`} data-key="${key}" ${extra}></div>`
}

function geom(n, k, label) {
    return `<div class="field"><label>${label}</label><div style="display:flex;gap:4px"><input type="number" min="0" step="1" inputmode="numeric" data-key="${k}" value="${n[k] ?? ''}"><label style="display:flex;align-items:center;gap:2px;font-size:11px"><input type="checkbox" data-unit="${k}" ${n.units[k]==='%'?'checked':''}>%</label></div></div>`
}

function drawInspector() {
    if (selectedId === 'window') {
        const w = model.window;
        inspector.innerHTML = `<div class="inspector-header"><h2>Window inspector</h2></div><section class="inspector-section"><h2>Window settings</h2>${field('Title','w-title',w.title)}${field('Background','w-bg_color',w.bg_color,'color')}${field('Top bar','w-topbar',w.topbar,'checkbox')}<p class="hint">The Window is the root of the app layout.</p></section>`;
        inspector.querySelectorAll('[data-key]').forEach(e => e.onchange = () => {
            const k = e.dataset.key.slice(2);
            model.window[k] = e.type === 'checkbox' ? e.checked : e.value;
            render()
        });
        inspector.querySelectorAll('[data-transparent]').forEach(e => e.onchange = () => {
            const key = e.dataset.transparent;
            model.window[key] = e.checked ? 'transparent' : inspector.querySelector(`[data-key="w-${key}"]`).value;
            render();
        });
        return
    }
    const n = byId(selectedId);
    if (!n) {
        inspector.innerHTML = '<div class="empty">Select a control to edit it.</div>';
        return
    }
    const opts = ['<option value="">Window (root)</option>', ...model.nodes.filter(x => x.id !== n.id && !desc(n.id, x.id)).map(x => `<option value="${x.id}" ${n.parent===x.id?'selected':''}>${esc(x.id)} (${x.type})</option>`)].join('');
    inspector.innerHTML = `<div class="inspector-header"><h2>${n.type} inspector</h2><button class="danger" data-delete>Delete</button></div><section class="inspector-section"><h2>Identity</h2>${field('Name','id',n.id)}<div class="field"><label>Parent</label><select data-key="parent">${opts}</select></div></section><section class="inspector-section"><h2>Geometry</h2><div class="two">${geom(n,'x','X')}${geom(n,'y','Y')}${geom(n,'width','Width')}${geom(n,'height','Height')}</div><p class="hint">Enable % per value to calculate it from the parent’s size. Disabled values are pixels.</p></section><section class="inspector-section"><h2>Appearance</h2>${field('Text','color',n.color,'color')}${field('Background','bg_color',n.bg_color,'color')}${field('Visible','visible',n.visible,'checkbox')}${field('Enabled','enabled',n.enabled,'checkbox')}</section><section class="inspector-section"><h2>Control settings</h2>${special(n)}</section>`;
    inspector.querySelectorAll('[data-key]').forEach(e => {
        e.onchange = () => change(n, e);
        if (!['id', 'parent'].includes(e.dataset.key)) e.oninput = () => {
            if (numericKeys.has(e.dataset.key)) {
                const normalized = wholeOrNull(e.value);
                e.value = normalized === null ? '' : normalized;
            }
            if (e.type === 'color') {
                const transparent = inspector.querySelector(`[data-transparent="${e.dataset.key}"]`);
                if (transparent) transparent.checked = false;
            }
            change(n, e, false);
        };
    });
    inspector.querySelectorAll('[data-transparent]').forEach(e => e.onchange = () => {
        const key = e.dataset.transparent;
        n[key] = e.checked ? 'transparent' : inspector.querySelector(`[data-key="${key}"]`).value;
        render();
    });
    inspector.querySelectorAll('[data-unit]').forEach(e => e.onchange = () => {
        const k = e.dataset.unit,
            p = parentSize(n),
            d = ['x', 'width'].includes(k) ? p.w : p.h,
            old = Math.max(0, px(n[k], n.units[k], d));
        n.units[k] = e.checked ? '%' : 'px';
        n[k] = val(old, n.units[k], d);
        render()
    });
    inspector.querySelector('[data-delete]').onclick = remove;
    inspector.querySelector('[data-import-image]')?.addEventListener('click', () => document.querySelector('#image-file').click());
}

function special(n) {
    return controls[n.type].fields.map(k => {
        if (['checked', 'bar', 'scroll'].includes(k)) return field(k, k, n[k], 'checkbox');
        if (k === 'text_align') return `<div class="field"><label>Text align</label><select data-key="${k}">${Object.keys(textAlignments).map(x=>`<option value="${x}" ${n[k]===x?'selected':''}>${x.replace('_',' ')}</option>`).join('')}</select></div>`;
        if (k === 'mode') return `<div class="field"><label>Mode</label><select data-key="${k}">${['stretch','zoom','auto_size','center'].map(x=>`<option ${n[k]===x?'selected':''}>${x}</option>`)}</select></div>`;
        if (k === 'src') return `<div class="field"><label>Image</label><div style="display:grid;grid-template-columns:minmax(0,1fr) auto;gap:6px"><select data-key="src"><option value="">No image selected</option>${(model.images || []).map(image => `<option value="${esc(image.path)}" ${n.src === image.path ? 'selected' : ''}>${esc(image.path)}</option>`).join('')}</select><button type="button" data-import-image>Import image</button></div></div>`;
        if (k === 'HTML') return `<div class="field"><label>HTML</label><textarea data-key="${k}">${esc(n[k])}</textarea></div>`;
        return field(k, k, n[k] ?? '', numericKeys.has(k) ? 'number' : 'text', numericKeys.has(k) ? 'min="0" step="1" inputmode="numeric"' : '')
    }).join('')
}

function change(n, e, refreshInspector = true) {
    const k = e.dataset.key;
    let v = e.type === 'checkbox' ? e.checked : e.value;
    if (numericKeys.has(k)) v = wholeOrNull(v);
    if (k === 'id') {
        if (!v.trim() || model.nodes.some(x => x.id === v && x !== n)) return;
        model.nodes.forEach(x => {
            if (x.parent === n.id) x.parent = v
        });
        model.templates.forEach(t => {
            if (t.sourceRoot === n.id) t.sourceRoot = v
        });
        n.id = v;
        selectedId = v
    }
    if (k === 'parent' && desc(n.id, v)) return;
    n[k] = v;
    synchronizeTemplates();
    if (refreshInspector) render();
    else {
        drawCanvas();
        drawTree();
    }
}

function desc(root, id) {
    let p = byId(id);
    while (p && p.parent) {
        if (p.parent === root) return true;
        p = byId(p.parent)
    }
    return false
}

function remove() {
    const del = id => {
        kids(id).forEach(x => del(x.id));
        model.nodes = model.nodes.filter(x => x.id !== id)
    };
    del(selectedId);
    selectedId = 'window';
    render()
}

function select(id) {
    selectedId = id;
    render()
}

function syncInspectorValues(n) {
    if (selectedId !== n.id) return;
    inspector.querySelectorAll('[data-key]').forEach(input => {
        const value = n[input.dataset.key];
        if (input.type === 'checkbox') input.checked = Boolean(value);
        else if (document.activeElement !== input) input.value = value ?? '';
    });
    inspector.querySelectorAll('[data-unit]').forEach(input => input.checked = n.units[input.dataset.unit] === '%');
}

function startResize(e, n, handle) {
    e.preventDefault();
    e.stopPropagation();
    const r = canvas.getBoundingClientRect(),
        o = parentOffset(n),
        p = parentSize(n),
        sx = 320 / r.width,
        sy = 480 / r.height;
    resize = {
        n,
        handle,
        sx,
        sy,
        o,
        p,
        initX: px(n.x, n.units.x, p.w),
        initY: px(n.y, n.units.y, p.h),
        initW: px(n.width, n.units.width, p.w),
        initH: px(n.height, n.units.height, p.h),
        startMouseX: (e.clientX - r.left) * sx,
        startMouseY: (e.clientY - r.top) * sy
    };
    e.currentTarget.setPointerCapture(e.pointerId)
}

function startDrag(e) {
    if (resize) return;
    e.preventDefault();
    e.stopPropagation();
    const n = byId(e.currentTarget.dataset.id);
    selectedId = n.id;
    document.querySelectorAll('.node').forEach(x => x.classList.toggle('selected', x.dataset.id === n.id));
    drawTree();
    drawInspector();
    const r = canvas.getBoundingClientRect(),
        o = parentOffset(n),
        p = parentSize(n),
        sx = 320 / r.width,
        sy = 480 / r.height;
    drag = {
        n,
        dx: (e.clientX - r.left) * sx - o.x - px(n.x, n.units.x, p.w),
        dy: (e.clientY - r.top) * sy - o.y - px(n.y, n.units.y, p.h)
    };
    e.currentTarget.setPointerCapture(e.pointerId)
}
window.addEventListener('pointermove', e => {
    if (resize) {
        const r = canvas.getBoundingClientRect(),
            rs = resize,
            n = rs.n,
            o = rs.o,
            p = rs.p,
            sx = rs.sx,
            sy = rs.sy,
            mx = (e.clientX - r.left) * sx,
            my = (e.clientY - r.top) * sy,
            dx = mx - rs.startMouseX,
            dy = my - rs.startMouseY;
        let x = rs.initX, y = rs.initY, w = rs.initW, h = rs.initH;
        if (rs.handle.includes('e')) { w = rs.initW + dx }
        if (rs.handle.includes('w')) { x = rs.initX + dx; w = rs.initW - dx }
        if (rs.handle.includes('s')) { h = rs.initH + dy }
        if (rs.handle.includes('n')) { y = rs.initY + dy; h = rs.initH - dy }
        w = Math.max(8, w);
        h = Math.max(8, h);
        if (x + w > p.w) { if (rs.handle.includes('w')) x = p.w - w }
        if (y + h > p.h) { if (rs.handle.includes('n')) y = p.h - h }
        x = Math.max(0, Math.min(p.w - w, x));
        y = Math.max(0, Math.min(p.h - h, y));
        w = Math.min(p.w - x, w);
        h = Math.min(p.h - y, h);
        n.x = val(x, n.units.x, p.w);
        n.y = val(y, n.units.y, p.h);
        n.width = val(w, n.units.width, p.w);
        n.height = val(h, n.units.height, p.h);
        const el = canvas.querySelector(`[data-id="${n.id}"]`);
        if (el) {
            el.style.left = `${o.x+px(n.x,n.units.x,p.w)}px`;
            el.style.top = `${o.y+px(n.y,n.units.y,p.h)}px`;
            el.style.width = `${px(n.width,n.units.width,p.w)}px`;
            el.style.height = `${px(n.height,n.units.height,p.h)}px`
        }
        syncInspectorValues(n);
        return
    }
    if (!drag) return;
    const r = canvas.getBoundingClientRect(),
        n = drag.n,
        o = parentOffset(n),
        p = parentSize(n),
        s = size(n),
        sx = 320 / r.width,
        sy = 480 / r.height,
        x = Math.max(0, Math.min(p.w - s.w, (e.clientX - r.left) * sx - o.x - drag.dx)),
        y = Math.max(0, Math.min(p.h - s.h, (e.clientY - r.top) * sy - o.y - drag.dy));
    n.x = val(x, n.units.x, p.w);
    n.y = val(y, n.units.y, p.h);
    const el = canvas.querySelector(`[data-id="${n.id}"]`);
    if (el) {
        el.style.left = `${o.x+px(n.x,n.units.x,p.w)}px`;
        el.style.top = `${o.y+px(n.y,n.units.y,p.h)}px`
    }
    syncInspectorValues(n);
});
window.addEventListener('pointerup', () => {
    if (resize) {
        resize = null;
        render();
        return
    }
    if (drag) {
        drag = null;
        render()
    }
})

function collect(root) {
    const out = [];
    const walk = n => {
        out.push(JSON.parse(JSON.stringify(n)));
        kids(n.id).forEach(walk)
    };
    walk(root);
    return out
}

function makeTemplate() {
    const root = byId(selectedId);
    if (!root) {
        alert('Select a parent control to create a template.');
        return
    }
    const name = prompt('Template name:', root.id + '_template');
    if (!name) return;
    const nodes = collect(root);
    nodes[0].parent = '';
    model.templates.push({
        name,
        nodes,
        sourceRoot: root.id,
        values: {}
    });
    render()
}

function templateParams(t) {
    const found = [...JSON.stringify(t.nodes).matchAll(/\$(\d+)/g)].map(m => Number(m[1]));
    return [...new Set(found)].sort((a, b) => a - b)
}

function withTemplateValues(value, values) {
    if (typeof value === 'string') return value.replace(/\$(\d+)/g, (_, key) => values[key] ?? `$${key}`);
    if (Array.isArray(value)) return value.map(item => withTemplateValues(item, values));
    if (value && typeof value === 'object') return Object.fromEntries(Object.entries(value).map(([key, item]) => [key, withTemplateValues(item, values)]));
    return value
}

function synchronizeTemplate(t) {
    const source = t.sourceRoot && byId(t.sourceRoot);
    if (source) {
        t.nodes = collect(source);
        t.nodes[0].parent = ''
    }
    const rootSource = t.nodes[0]?.id;
    if (!rootSource) return;
    const linked = model.nodes.filter(n => n.templateName === t.name);
    const groups = [...new Set(linked.map(n => n.templateInstance))];
    groups.forEach(instance => {
        const copies = linked.filter(n => n.templateInstance === instance);
        const map = new Map(copies.map(n => [n.templateNode, n]));
        const root = map.get(rootSource);
        if (!root) return;
        const parent = root.parent, x = root.x, y = root.y, units = root.units, values = root.templateValues || {};
        t.nodes.forEach(sourceNode => {
            const copy = map.get(sourceNode.id);
            if (!copy) return;
            const fresh = withTemplateValues(JSON.parse(JSON.stringify(sourceNode)), values);
            fresh.id = copy.id;
            fresh.parent = sourceNode.id === rootSource ? parent : (map.get(sourceNode.parent)?.id || '');
            fresh.templateName = t.name;
            fresh.templateNode = sourceNode.id;
            fresh.templateInstance = instance;
            fresh.templateValues = values;
            if (sourceNode.id === rootSource) Object.assign(fresh, {x, y, units});
            Object.assign(copy, fresh)
        })
    })
}

function synchronizeTemplates() {
    model.templates.forEach(synchronizeTemplate)
}

function insertTemplate(t) {
    const target = byId(selectedId),
        map = new Map(),
        instance = uid(),
        values = {...(t.values || {})},
        copy = withTemplateValues(t.nodes, values);
    copy.forEach(n => map.set(n.id, uid()));
    const sourceRoot = t.nodes[0].id;
    copy.forEach(n => {
        const old = n.id;
        n.id = map.get(old);
        n.parent = old === sourceRoot ? (target ? target.id : '') : (map.get(n.parent) || '');
        n.x = old === sourceRoot ? 10 : n.x;
        n.y = old === sourceRoot ? 10 : n.y;
        n.templateName = t.name;
        n.templateNode = old;
        n.templateInstance = instance;
        n.templateValues = values
    });
    model.nodes.push(...copy);
    selectedId = map.get(sourceRoot);
    render()
}

function dimension(value) {
    const match = String(value ?? '').trim().match(/^(-?\d+(?:\.\d+)?)\s*(%|px)?$/i);
    return {value: wholeOrNull(match?.[1]), unit: match?.[2] === '%' ? '%' : 'px'};
}

function readLayout(text) {
    const sections = [];
    let section = null;
    String(text).replace(/^\uFEFF/, '').split(/\r?\n/).forEach(line => {
        const heading = line.trim().match(/^([A-Za-z][\w]*):\s*$/);
        if (heading) {
            section = {type: heading[1], values: {}};
            sections.push(section);
            return;
        }
        const attribute = line.trim().match(/^([\w]+)\s*=\s*"((?:\\.|[^"\\])*)"\s*;?\s*$/);
        if (section && attribute) section.values[attribute[1]] = attribute[2].replaceAll('\\"', '"');
    });
    const windowSection = sections.find(item => item.type === 'Window');
    const importedWindow = {
        title: 'My App', width: '100%', height: '100%', bg_color: '#ffffff', topbar: true,
        ...(windowSection?.values || {})
    };
    if (typeof importedWindow.topbar === 'string') importedWindow.topbar = importedWindow.topbar === 'true';
    const nodes = sections.filter(item => controls[item.type]).map(item => {
        const n = node(item.type);
        const values = item.values;
        Object.entries(values).forEach(([key, value]) => {
            if (key === 'name') n.id = value || uid();
            else if (key === 'location') {
                if (value == "top_left" || value == "top" || value == "top_right" || value == "left" || value == "center" || value == "right" || value == "bottom_left" || value == "bottom" || value == "bottom_right")
                    n.anchor = value;
                else
                {
                    const [x, y] = value.split(',').map(dimension);
                    n.x = x.value; n.y = y.value;
                    n.units.x = x.unit; n.units.y = y.unit;
                }
            } else if (key === 'width' || key === 'height') {
                const size = dimension(value);
                n[key] = size.value; n.units[key] = size.unit;
            } else if (key === 'visible' || key === 'enabled' || key === 'checked' || key === 'bar' || key === 'scroll') n[key] = value === 'true';
            else if (numericKeys.has(key)) n[key] = wholeOrNull(value);
            else if (key in n || controls[item.type].fields.includes(key)) n[key] = value;
        });
        normalizeNodeNumbers(n);
        return n;
    });
    return {window: importedWindow, nodes};
}

function parse(text) {
    const imported = readLayout(text);
    if (!imported.nodes.length && !String(text).includes('Window:')) throw new Error('No supported layout controls were found.');
    model = {window: imported.window, nodes: imported.nodes, templates: [], images: model.images || []};
    selectedId = 'window';
    render();
}

function importTemplate(file) {
    const reader = new FileReader();
    reader.onload = () => {
        try {
            const imported = readLayout(reader.result);
            if (!imported.nodes.length) throw new Error('No supported controls were found.');
            const root = imported.nodes[0];
            root.parent = '';
            model.templates.push({name: file.name.replace(/\.[^.]+$/, '') || 'Template', nodes: imported.nodes, sourceRoot: '', values: {}});
            render();
        } catch (error) {
            alert(`Unable to import template: ${error.message}`);
        }
    };
    reader.readAsText(file);
}

function build() {
    const blob = new Blob([generate()], {type: 'text/plain;charset=utf-8'});
    const url = URL.createObjectURL(blob);
    const link = document.createElement('a');
    link.href = url;
    link.download = 'app.layout';
    link.click();
    URL.revokeObjectURL(url);
}

async function copyGenerated() {
    const code = generate();
    try {
        await navigator.clipboard.writeText(code);
    } catch {
        const temporary = document.createElement('textarea');
        temporary.value = code;
        temporary.style.position = 'fixed';
        temporary.style.opacity = '0';
        document.body.append(temporary);
        temporary.select();
        document.execCommand('copy');
        temporary.remove();
    }
}

function quote(v) {
    return String(v ?? '').replaceAll('"', '\\"')
}

function layout(nodes, window = false) {
    let out = window ? '# d3m0n Studio generated layout\n\nWindow:\n' : '';
    if (window)
        for (const [k, v] of Object.entries({
                title: model.window.title,
                bg_color: model.window.bg_color,
                width: model.window.width,
                height: model.window.height,
                topbar: model.window.topbar
            })) out += `  ${k}="${quote(v)}";\n`;
    for (const n of nodes) {
        out += `\n${n.type}:\n`;
        const base = {
            name: n.id,
            parent: n.parent,
            visible: n.visible,
            enabled: n.enabled,
            color: n.color,
            bg_color: n.bg_color,
            width: fmt(n.width, n.units.width),
            height: fmt(n.height, n.units.height),
            location: (n.anchor)? n.anchor : `${fmt(n.x,n.units.x)}, ${fmt(n.y,n.units.y)}`
        };
        for (const [k, v] of Object.entries({
                ...base,
                ...Object.fromEntries(controls[n.type].fields.map(k => [k, n[k]]))
            })) {
            if (k === 'parent' && !v || v === '' || v === undefined)
                continue;
            out += `  ${k}="${quote(v)}";\n`
        }
    }
    return out
}

function generate() {
    return layout(model.nodes, true)
}

function render()
{
    model.nodes.forEach(normalizeNodeNumbers);
    synchronizeTemplates();
    drawCanvas();
    drawTree();
    drawTemplates();
    drawInspector();
    drawAppFilesystem();
    document.querySelector('#app-label').textContent = model.window.title || model.window.name || 'Untitled app';
    document.querySelector('#selection-label').textContent = selectedId === 'window' ? 'Window selected' : selectedId ? `${byId(selectedId).type} selected` : 'No selection'
}
document.querySelector('#palette').innerHTML = Object.keys(controls).map(t => `<button data-type="${t}">+ ${t}</button>`).join('');
document.querySelectorAll('[data-type]').forEach(b => b.onclick = () => add(b.dataset.type));
document.querySelector('#make-template').onclick = makeTemplate;
document.querySelector('#import-template').onclick = () => document.querySelector('#template-file').click();
document.querySelector('#template-file').onchange = e => {
    const file = e.target.files[0];
    if (file) importTemplate(file);
    e.target.value = ''
};
document.querySelector('#build').onclick = build;
document.querySelector('#view-code').onclick = () => {
    document.querySelector('#code-output').value = generate();
    document.querySelector('#code-modal').classList.add('open')
};
document.querySelector('#close-code').onclick = () => document.querySelector('#code-modal').classList.remove('open');
document.querySelector('#copy-code').onclick = copyGenerated;
document.querySelector('#new-layout').onclick = () => {
    if (confirm('Start a new empty layout?')) {
        model = {
            window: {
                name: 'My App',
                title: 'My App',
                width: '100%',
                height: '100%',
                bg_color: '#ffffff',
                topbar: true
            },
            nodes: [],
            templates: [],
            images: []
        };
        selectedId = 'window';
        render()
    }
};
document.querySelector('#import').onclick = () => document.querySelector('#file').click();
document.querySelector('#image-file').onchange = e => {
    const files = [...e.target.files];
    if (!files.length) return;
    Promise.all(files.map(file => new Promise(resolve => {
        const reader = new FileReader();
        reader.onload = () => resolve({path: file.name, type: file.type, data: reader.result});
        reader.readAsDataURL(file);
    }))).then(images => {
        model.images = model.images || [];
        images.forEach(image => {
            const existing = model.images.findIndex(item => item.path === image.path);
            if (existing >= 0) model.images[existing] = image;
            else model.images.push(image);
        });
        render();
    });
    e.target.value = '';
};
document.querySelector('#file').onchange = e => {
    const f = e.target.files[0];
    if (f) {
        const r = new FileReader();
        r.onload = () => {
            try {
                parse(r.result);
            } catch (error) {
                alert(`Unable to import layout: ${error.message}`);
            }
        };
        r.readAsText(f)
    }
    e.target.value = '';
};

document.querySelector("#edit-src").onclick=()=>{
      if (!currentSrcPath)
      {
          alert("No source file associated");
          return;
      }

      document.querySelector("#src-title").textContent=currentSrcPath;

      document.querySelector("#src-editor").value=currentAppFS[currentSrcPath];

      document.querySelector("#src-modal").classList.add("open");
  };


  document.querySelector("#close-src").onclick=()=>{
      document.querySelector("#src-modal").classList.remove("open");
  };


  document.querySelector("#save-src").onclick=()=>{
      currentAppFS[currentSrcPath] = document.querySelector("#src-editor").value;
      document.querySelector("#src-modal").classList.remove("open");
  };

  document.querySelector("#save-app").onclick=()=>{
    if (!currentLayoutPath)
        return;

    currentAppFS[currentLayoutPath]=generate();
    modifiedFiles={
        ...currentAppFS
    };
    console.log("Modified app files:", modifiedFiles);

    /*
      Later:
      POST modifiedFiles to Flask:
      /api/save
    */
};

async function get_icon(name)
{
    let icon_pack = config['icon_pack'];
    if (!icon_pack)
        return null;

    let res = await fetch(`/api/icons/${name}`);
    if (!res.ok)
        throw new Error("Failed to load icon");

    let blob = await res.blob();
    return URL.createObjectURL(blob);
}

async function get_config()
{
    let res = await fetch("/api/config");
    let text = await res.text();
    let lines = text.split('\n');
    config = {};
    lines.forEach(l => {
        l = l.trim();
        if (!l.startsWith('#') && l.includes(':'))
        {
            let k = l.split(":")[0].trim();
            let v = l.split(":")[1].trim();
            config[k] = v;
        }
    });
}

get_config();
document.querySelector('#zoom').oninput = e => document.querySelector('#canvas-shell').style.transform = `scale(${e.target.value})`;
canvas.onclick = () => select('window');
render();