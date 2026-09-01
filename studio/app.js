const canvas = document.querySelector('#canvas'),
    tree = document.querySelector('#tree'),
    inspector = document.querySelector('#inspector'),
    templatesEl = document.querySelector('#templates');

const startupModal = document.getElementById("startup-modal");
const applicationList = document.getElementById("application-list");
const appTabs = document.getElementById("app-tabs");
const fileTabs = document.getElementById("file-tabs");
const visualEditor = document.getElementById("visual-editor");
const codeEditor = document.getElementById("code-editor");
const imageEditor = document.getElementById("image-editor");
const filePreview = document.getElementById("file-preview");
const editorEmpty = document.getElementById("editor-empty");
var cached_apps = undefined;
let openProjects = [];
let openFiles = new Map();
let fileModes = new Map();
let clipboardPath = null;
let editorDrafts = new Map();
let dirtyFiles = new Set();
let activeFile = null;
let suppressDirty = false;
let currentAppFS = null;
let currentProjectId = null;
let currentLayoutPath = null;
let currentSrcPath = null;
let modifiedFiles = {};
let config = {};
let cppSyncTimer = null;
const apiProjectPath = id => String(id).split('/').map(encodeURIComponent).join('/');

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
    currentProjectId = app.id || app.project;
    currentAppFS = flattenFiles(app.files);
    try {
        const response = await fetch(`/api/applications/${apiProjectPath(currentProjectId)}/files`);
        if (response.ok) currentAppFS = await response.json();
    } catch (error) {
        console.error(error);
    }
    currentLayoutPath = Object.keys(currentAppFS).find(path => /\.layout$/i.test(path)) || null;
    currentSrcPath = Object.keys(currentAppFS).find(path => /\.hpp$/i.test(path) && /windows\//i.test(path)) || Object.keys(currentAppFS).find(path => /\.(cpp|hpp|h|c|cc|cxx)$/i.test(path)) || null;
    if (currentLayoutPath) parse(currentAppFS[currentLayoutPath]);
    else {
        model.window.title = app.project || app.id || 'My App';
        model.window.class_name = 'MainWindow';
        model.nodes = [];
        selectedId = 'window';
        render();
    }
    currentApplication = app;
    activeFile = null;
    setEditorMode(null);
    if (!openFiles.has(currentProjectId)) openFiles.set(currentProjectId, []);
    drawFileTabs();
    drawAppTabs();
}

function fileKey(project, path) {
    return `${project}:${path}`;
}

function markDirty(path = activeFile) {
    if (currentProjectId && path) {
        dirtyFiles.add(fileKey(currentProjectId, path));
        drawFileTabs();
    }
}

function isDirty(project, path) {
    return Boolean(project && path && dirtyFiles.has(fileKey(project, path)));
}

async function resolveDirtyFiles(project) {
    const paths = [...dirtyFiles].filter(key => key.startsWith(`${project}:`)).map(key => key.slice(String(project).length + 1));
    for (const path of paths) {
        const save = confirm(`${path} has unsaved changes. Save them now?\n\nChoose Cancel to discard them.`);
        if (save) {
            let content = editorDrafts.get(fileKey(project, path));
            if (path === activeFile && codeEditor.classList.contains('active')) content = document.querySelector('#src-editor').value;
            if (content === undefined && path === activeFile && isWindowFile(path)) content = generateCpp();
            if (content !== undefined) {
                const response = await fetch(`/api/applications/${apiProjectPath(project)}/files`, {method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({path, content})});
                if (!response.ok) { alert('Unable to save changes.'); return false; }
                currentAppFS[path] = content;
            }
        }
        dirtyFiles.delete(fileKey(project, path));
        editorDrafts.delete(fileKey(project, path));
    }
    drawFileTabs();
    return true;
}

function drawFileTabs() {
    fileTabs.innerHTML = '';
    const files = openFiles.get(currentProjectId) || [];
    files.forEach(path => {
        const tab = document.createElement('div');
        tab.className = `file-tab${path === activeFile ? ' active' : ''}`;
        tab.draggable = true;
        tab.ondragstart = event => event.dataTransfer.setData('text/plain', path);
        tab.ondragover = event => event.preventDefault();
        tab.ondrop = event => {
            event.preventDefault();
            reorderFile(event.dataTransfer.getData('text/plain'), path);
        };
        const name = document.createElement('button');
        name.className = 'tab-name';
        name.textContent = path.split('/').pop();
        name.title = path;
        name.onclick = () => switchFile(path);
        const close = document.createElement('button');
        close.className = 'tab-close';
        close.textContent = '×';
        close.onclick = event => { event.stopPropagation(); closeFile(path); };
        const dirty = document.createElement('span');
        dirty.className = 'tab-dirty';
        dirty.hidden = !isDirty(currentProjectId, path);
        dirty.title = 'Unsaved changes';
        tab.append(name, dirty, close);
        fileTabs.append(tab);
    });
}

function reorderFile(source, target) {
    if (!source || source === target || !currentProjectId) return;
    const files = openFiles.get(currentProjectId) || [];
    const from = files.indexOf(source), to = files.indexOf(target);
    if (from < 0 || to < 0) return;
    files.splice(from, 1);
    files.splice(files.indexOf(target), 0, source);
    openFiles.set(currentProjectId, files);
    drawFileTabs();
}

function activateFile(path, forceCode = false) {
    if (activeFile && codeEditor.classList.contains('active')) editorDrafts.set(`${currentProjectId}:${activeFile}`, document.querySelector('#src-editor').value);
    activeFile = path;
    const type = forceCode || fileModes.get(fileKey(currentProjectId, path)) === 'code' ? 'code' : fileModes.get(fileKey(currentProjectId, path)) || fileType(path);
    setEditorMode(type);
    if (type === 'code') document.querySelector('#src-editor').value = editorDrafts.get(fileKey(currentProjectId, path)) ?? currentAppFS[path] ?? '';
    if (type === 'image') filePreview.src = projectImageSource(path);
    drawFileTabs();
}

function fileType(path) {
    if (isWindowFile(path) || /\.layout$/i.test(path)) return 'visual';
    if (/\.(png|jpe?g|gif|bmp|webp|tiff?|ico|svg)$/i.test(path)) return 'image';
    return 'code';
}

function projectImageSource(path) {
    const file = currentAppFS?.[path];
    if (file && file.type === 'image') return `data:${file.mime};base64,${file.data}`;
    if (typeof file === 'string' && /^\s*<svg[\s>]/i.test(file)) return `data:image/svg+xml;charset=utf-8,${encodeURIComponent(file)}`;
    return '';
}

async function switchFile(path) {
    if (path === activeFile) return;
    if (currentProjectId && !(await resolveDirtyFiles(currentProjectId))) return;
    if (fileModes.get(fileKey(currentProjectId, path)) === 'code') openProjectFile(path, true, true);
    else if (isWindowFile(path)) await openWindowFile(path, true);
    else openProjectFile(path, true);
}

function setEditorMode(type) {
    editorEmpty.style.display = type ? 'none' : 'flex';
    visualEditor.classList.toggle('active', type === 'visual');
    codeEditor.classList.toggle('active', type === 'code');
    imageEditor.classList.toggle('active', type === 'image');
}

function isWindowFile(path) {
    return /(^|\/)windows\/[^/]+\.hpp$/i.test(path);
}

function rememberFile(path) {
    if (!currentProjectId) return;
    const files = openFiles.get(currentProjectId) || [];
    if (!files.includes(path)) files.push(path);
    openFiles.set(currentProjectId, files);
}

async function closeFile(path) {
    if (isDirty(currentProjectId, path) && !(await resolveDirtyFiles(currentProjectId))) return;
    const files = (openFiles.get(currentProjectId) || []).filter(file => file !== path);
    openFiles.set(currentProjectId, files);
    if (activeFile === path) {
        activeFile = files.at(-1) || null;
        if (activeFile) fileModes.get(fileKey(currentProjectId, activeFile)) === 'code' ? openProjectFile(activeFile, true, true) : isWindowFile(activeFile) ? openWindowFile(activeFile, true) : activateFile(activeFile);
        else {
            setEditorMode(null);
            drawFileTabs();
        }
    } else drawFileTabs();
}

function applicationName(app) {
    const match = String(app.manifest || '').match(/^name\s*:\s*(.*)$/m);
    return match?.[1]?.trim() || app.project || app.id;
}

function drawAppTabs() {
    appTabs.innerHTML = '';
    for (const id of openProjects) {
        const app = (cached_apps || []).find(item => item.id === id);
        if (!app) continue;
        const tab = document.createElement('div');
        tab.className = `app-tab${id === currentProjectId ? ' active' : ''}`;
        const name = document.createElement('button');
        name.className = 'tab-name';
        name.textContent = applicationName(app);
        name.title = id;
        name.onclick = () => switchApplication(id);
        const close = document.createElement('button');
        close.className = 'tab-close';
        close.textContent = '×';
        close.title = 'Close application tab';
        close.onclick = event => { event.stopPropagation(); closeApplication(id); };
        tab.append(name, close);
        appTabs.appendChild(tab);
    }
}

async function openApplication(app) {
    const id = app.id || app.project;
    if (currentProjectId && currentProjectId !== id && !(await resolveDirtyFiles(currentProjectId))) return;
    if (!openProjects.includes(id)) openProjects.push(id);
    startupModal.classList.remove('open');
    await loadAppFilesystem(app);
}

async function switchApplication(id) {
    const app = (cached_apps || []).find(item => item.id === id);
    if (app) await openApplication(app);
}

async function closeApplication(id) {
    if (currentProjectId === id && !(await resolveDirtyFiles(id))) return;
    openProjects = openProjects.filter(item => item !== id);
    if (currentProjectId !== id) {
        drawAppTabs();
        return;
    }
    const next = openProjects[openProjects.length - 1];
    if (next) await switchApplication(next);
    else {
        currentAppFS = null;
        currentProjectId = null;
        currentLayoutPath = null;
        currentSrcPath = null;
        startupModal.classList.add('open');
        drawAppTabs();
        render();
    }
}

async function loadApplications(showStartup = true)
{
    if (showStartup && !startupModal.classList.contains("open"))
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
            lines.forEach(l => {
                const separator = l.indexOf(':');
                if (separator < 0) return;
                const k = l.slice(0, separator).trim();
                const v = l.slice(separator + 1).trim();
                if (k === 'name' || k === 'icon' || k === 'description') manifest[k] = v;
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
        info.className = 'app-info';
        info.innerHTML = `
            <div class="name">${manifest.name || "Unnamed App"}</div>
            <div class="description">${manifest.description || ""}</div>
        `;

        const remove = document.createElement('button');
        remove.className = 'danger delete';
        remove.textContent = '×';
        remove.title = 'Delete application';
        remove.onclick = event => {
            event.stopPropagation();
            deleteApplication(app);
        };
        card.append(img, info, remove);
        card.onclick = async () => {
            window.currentApplication = app;
            window.currentManifest = manifest;
            await openApplication(app);
        };
        applicationList.appendChild(card);
    }

    if (!apps.length) {
        applicationList.innerHTML =
            '<div class="empty">No applications found.</div>';
    }
}

async function deleteApplication(app) {
    const id = app.id || app.project;
    if (!confirm(`Delete application “${applicationName(app)}” and all its files?`)) return;
    const response = await fetch(`/api/applications/${apiProjectPath(id)}`, {method: 'DELETE'});
    const result = await response.json();
    if (!response.ok) {
        alert(result.error || 'Unable to delete application.');
        return;
    }
    const wasCurrent = currentProjectId === id;
    openProjects = openProjects.filter(item => item !== id);
    cached_apps = undefined;
    await loadApplications(false);
    if (wasCurrent) {
        const next = openProjects[openProjects.length - 1];
        if (next) await switchApplication(next);
        else {
            currentAppFS = null;
            currentProjectId = null;
            currentLayoutPath = null;
            currentSrcPath = null;
            startupModal.classList.add('open');
            render();
        }
    }
    drawAppTabs();
}

async function createApplication() {
    const name = prompt('Application folder name:', 'my_app');
    if (!name) return;
    const response = await fetch('/api/applications', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({name})
    });
    const result = await response.json();
    if (!response.ok) {
        alert(result.error || 'Unable to create application.');
        return;
    }
    cached_apps = undefined;
    await loadApplications();
    const app = (cached_apps || []).find(item => item.id === result.id);
    if (app) {
        await openApplication(app);
    }
}

const controls = {
    Text: {
        w: 120,
        h: 24,
        content: 'Text',
        fields: ['content', 'type', 'font_size', 'text_align']
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
        fields: ['content', 'type', 'font_size', 'text_align']
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
            class_name: 'MainWindow',
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
    selectedIds = new Set(['window']),
    drag = null,
    resize = null,
    pan = null,
    spacePressed = false;

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
        input_type: '',
        HTML: '',
        ...over
    }
}

function add(type) {
    const n = node(type);
    normalizeNodeNumbers(n);
    model.nodes.push(n);
    selectedId = n.id;
    selectedIds = new Set([n.id]);
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
    el.className = 'node' + (selectedId === n.id || selectedIds.has(n.id) ? ' selected' : '');
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
    const tree = document.createElement("div");
    tree.className = "tree file-tree";
    const files = Object.keys(currentAppFS).filter(path => typeof currentAppFS[path] === 'string' || currentAppFS[path]?.type === 'image');
    const windowFiles = files.filter(path => /(^|\/)windows\/[^/]+\.hpp$/i.test(path));
    const folders = new Map();
    const addFolder = (path, depth = 0) => {
        if (!folders.has(path)) folders.set(path, {path, name: path.split('/').pop() || '', depth, children: []});
        return folders.get(path);
    };
    const treeFiles = files.filter(file => !windowFiles.includes(file));
    windowFiles.forEach(file => treeFiles.push({path: file, name: file.split('/').pop().replace(/\.hpp$/i, ''), window: true, children: [file]}));
    treeFiles.forEach(file => {
        const filePath = typeof file === 'string' ? file : file.path;
        const parts = filePath.split('/');
        let parent = addFolder('');
        parts.slice(0, -1).forEach((part, index) => {
            const folderPath = parts.slice(0, index + 1).join('/');
            let folder = addFolder(folderPath, index + 1);
            if (!parent.children.includes(folder)) parent.children.push(folder);
            parent = folder;
        });
        parent.children.push(typeof file === 'string' ? {path: filePath, name: parts.at(-1), window: false, children: []} : {...file, path: filePath, name: file.name});
    });
    const drawFolder = (folder, parentEl) => {
        folder.children.sort((a, b) => Number(Boolean(a.children && !('window' in a))) - Number(Boolean(b.children && !('window' in b))) || (a.name || '').localeCompare(b.name || ''));
        folder.children.forEach(item => {
            const row = document.createElement('div');
            row.className = 'tree-row file-row';
            row.style.paddingLeft = `${5 + folder.depth * 16}px`;
            const isFolder = Boolean(item.children && !('window' in item));
            row.innerHTML = `<span class="kind">${isFolder ? 'Folder' : item.window ? 'Window' : fileType(item.path) === 'image' ? 'Image' : /\.(cpp|hpp|h|c|cc|cxx)$/i.test(item.path) ? 'C++' : 'File'}</span><span class="name">${esc(isFolder ? item.path.split('/').pop() : item.name || item.path)}</span>`;
            row.onclick = () => isFolder ? row.classList.toggle('active') : item.window ? openWindowFile(item.path) : switchFile(item.path);
            row.oncontextmenu = event => showFileMenu(event, isFolder ? item.path : item.path, isFolder ? 'folder' : 'file');
            parentEl.append(row);
            if (!isFolder && item.window) item.children.forEach(child => {
                const childRow = document.createElement('div');
                childRow.className = 'tree-row file-row';
                childRow.style.paddingLeft = `${21 + folder.depth * 16}px`;
                childRow.innerHTML = `<span class="kind">C++</span><span class="name">${esc(child.split('/').pop())}</span>`;
                childRow.onclick = () => openProjectFile(child, true, true);
                childRow.oncontextmenu = event => showFileMenu(event, child, 'file');
                parentEl.append(childRow);
            });
            if (isFolder) drawFolder(item, parentEl);
        });
    };
    drawFolder(folders.get(''), tree);
    tree.oncontextmenu = event => { if (event.target === tree) showFileMenu(event, '', 'folder'); };

    container.append(tree);
    document.querySelector(".left").prepend(container);
}

async function refreshCurrentFiles() {
    if (!currentProjectId) return;
    const response = await fetch(`/api/applications/${apiProjectPath(currentProjectId)}/files`);
    if (response.ok) currentAppFS = await response.json();
    openFiles.set(currentProjectId, (openFiles.get(currentProjectId) || []).filter(path => currentAppFS[path] !== undefined));
    if (activeFile && !currentAppFS[activeFile]) {
        activeFile = null;
        setEditorMode(null);
    }
    drawFileTabs();
    drawAppFilesystem();
}

async function fileOperation(payload) {
    const response = await fetch(`/api/applications/${apiProjectPath(currentProjectId)}/files/operation`, {
        method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify(payload)
    });
    const result = await response.json();
    if (!response.ok) throw new Error(result.error || 'File operation failed.');
    if (payload.action === 'rename') {
        const tabs = openFiles.get(currentProjectId) || [];
        const index = tabs.indexOf(payload.path);
        if (index >= 0) tabs[index] = payload.destination;
        if (activeFile === payload.path) activeFile = payload.destination;
    }
    await refreshCurrentFiles();
}

function showFileMenu(event, path, kind) {
    event.preventDefault();
    document.querySelector('#file-menu')?.remove();
    const menu = document.createElement('div');
    menu.id = 'file-menu';
    menu.className = 'file-menu';
    const add = (label, action) => { const button = document.createElement('button'); button.textContent = label; button.onclick = () => { menu.remove(); action(); }; menu.append(button); };
    add('New file', () => createProjectEntry(path, kind, 'file'));
    add('New folder', () => createProjectEntry(path, kind, 'folder'));
    add('Paste', () => pasteProjectEntry(path, kind));
    if (path) {
        add('Rename', () => renameProjectEntry(path));
        add('Copy', () => copyProjectEntry(path));
        add('Delete', () => deleteProjectEntry(path, kind));
    }
    menu.style.left = `${event.clientX}px`;
    menu.style.top = `${event.clientY}px`;
    document.body.append(menu);
    setTimeout(() => document.addEventListener('pointerdown', () => menu.remove(), {once: true}), 0);
}

function copyProjectEntry(path) {
    clipboardPath = path;
    navigator.clipboard?.writeText(path).catch(() => {});
}

async function createProjectEntry(parent, kind, entryKind) {
    const name = prompt(`New ${entryKind} name:`, entryKind === 'folder' ? 'folder' : 'file.cpp');
    if (!name || !currentProjectId) return;
    const base = kind === 'folder' || !parent ? parent : parent.split('/').slice(0, -1).join('/');
    try { await fileOperation({action: 'create', path: [base, name].filter(Boolean).join('/'), kind: entryKind}); } catch (error) { alert(error.message); }
}

async function renameProjectEntry(path) {
    const name = prompt('New name:', path.split('/').pop());
    if (!name || !currentProjectId) return;
    const destination = [...path.split('/').slice(0, -1), name].filter(Boolean).join('/');
    try { await fileOperation({action: 'rename', path, destination}); } catch (error) { alert(error.message); }
}

async function deleteProjectEntry(path, kind) {
    if (!confirm(`Delete ${kind} “${path}”?`)) return;
    try { await fileOperation({action: 'delete', path}); } catch (error) { alert(error.message); }
}

async function pasteProjectEntry(path, kind) {
    if (!currentProjectId) return;
    try {
        const source = clipboardPath || await navigator.clipboard.readText();
        const isFolder = source && currentAppFS[source] === undefined && Object.keys(currentAppFS).some(file => file.startsWith(`${source}/`));
        if (!source || (!currentAppFS[source] && !isFolder)) throw new Error('Copy a project file or folder first.');
        const base = kind === 'folder' ? path : path.split('/').slice(0, -1).join('/');
        const name = source.split('/').pop();
        const destination = [base, `${name.replace(/(\.[^.]+)?$/, '')}_copy${isFolder ? '' : source.match(/\.[^.]+$/)?.[0] || ''}`].filter(Boolean).join('/');
        await fileOperation({action: 'copy', path: source, destination});
    } catch (error) { alert(error.message || 'Clipboard access was denied.'); }
}

async function openWindowFile(path, confirmed = false) {
    if (!currentProjectId || typeof currentAppFS[path] !== 'string') return;
    if (!confirmed && (path !== activeFile || isDirty(currentProjectId, path)) && !(await resolveDirtyFiles(currentProjectId))) return;
    const response = await fetch(`/api/applications/${apiProjectPath(currentProjectId)}/files/parse`, {
        method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify({content: currentAppFS[path]})
    });
    const result = await response.json();
    if (!response.ok || !result.model) { alert(result.error || 'Unable to parse this window.'); return; }
    currentSrcPath = path;
    currentLayoutPath = null;
    fileModes.set(fileKey(currentProjectId, path), 'visual');
    rememberFile(path);
    activeFile = path;
    suppressDirty = true;
    applyCppModel(result.model);
    suppressDirty = false;
    activateFile(path);
    document.querySelector('#app-label').textContent = model.window.title || path;
}

async function openProjectFile(path, confirmed = false, forceCode = false) {
    if (!currentAppFS || (typeof currentAppFS[path] !== 'string' && currentAppFS[path]?.type !== 'image')) return;
    if (!confirmed && (path !== activeFile || isDirty(currentProjectId, path)) && !(await resolveDirtyFiles(currentProjectId))) return;
    currentSrcPath = path;
    if (forceCode) fileModes.set(fileKey(currentProjectId, path), 'code');
    if (fileType(path) === 'image') {
        fileModes.set(fileKey(currentProjectId, path), 'image');
        rememberFile(path);
        activateFile(path);
        document.querySelector('#app-label').textContent = path;
        return;
    }
    if (/\.layout$/i.test(path)) {
        currentLayoutPath = path;
        rememberFile(path);
        activeFile = path;
        suppressDirty = true;
        parse(currentAppFS[path]);
        suppressDirty = false;
        activateFile(path);
        document.querySelector('#app-label').textContent = path;
        return;
    }
    rememberFile(path);
    activateFile(path, forceCode);
    document.querySelector('#app-label').textContent = path;
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
        r.className = 'tree-row' + (selectedIds.has(n.id) ? ' active' : '');
        r.style.paddingLeft = `${5+d*16}px`;
        r.innerHTML = `<span class="kind">${n.type}</span><span class="name">${esc(n.id)}</span>`;
        r.onclick = event => select(n.id, event.shiftKey);
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
        inspector.innerHTML = `<div class="inspector-header"><h2>Window inspector</h2></div><section class="inspector-section"><h2>Window settings</h2>${field('Class','w-class_name',w.class_name || 'MainWindow')}${field('Title','w-title',w.title)}${field('Background','w-bg_color',w.bg_color,'color')}${field('Top bar','w-topbar',w.topbar,'checkbox')}<p class="hint">The Window is the root of the app layout.</p></section>`;
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
        if (k === 'type') return field('Type', 'input_type', n.input_type || '', 'text');
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
    const targets = [...selectedIds].filter(id => id !== 'window' && byId(id));
    if (!targets.length) return;
    const del = id => {
        kids(id).forEach(x => del(x.id));
        model.nodes = model.nodes.filter(x => x.id !== id)
    };
    targets.forEach(del);
    selectedId = 'window';
    selectedIds = new Set(['window']);
    render()
}

function select(id, additive = false) {
    if (additive && id !== 'window') {
        if (selectedIds.has(id)) selectedIds.delete(id);
        else selectedIds.add(id);
        selectedId = id;
    } else {
        selectedId = id;
        selectedIds = new Set([id]);
    }
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
    if (spacePressed) return;
    if (resize) return;
    e.preventDefault();
    e.stopPropagation();
    const n = byId(e.currentTarget.dataset.id);
    if (e.shiftKey) {
        select(n.id, true);
        return;
    }
    if (!selectedIds.has(n.id)) selectedIds = new Set([n.id]);
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
const canvasArea = document.querySelector('.canvas-area');
canvasArea.addEventListener('pointerdown', event => {
    if (!spacePressed || event.button !== 0) return;
    event.preventDefault();
    pan = {x: event.clientX, y: event.clientY, left: canvasArea.scrollLeft, top: canvasArea.scrollTop};
    canvasArea.setPointerCapture(event.pointerId);
});
window.addEventListener('pointermove', e => {
    if (pan) {
        canvasArea.scrollLeft = pan.left - e.clientX + pan.x;
        canvasArea.scrollTop = pan.top - e.clientY + pan.y;
        return;
    }
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
    if (pan) {
        pan = null;
        return;
    }
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
        title: 'My App', class_name: 'MainWindow', width: '100%', height: '100%', bg_color: '#ffffff', topbar: true,
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
            } else if (key === 'type' && item.type === 'TextBox') n.input_type = value;
            else if (key === 'visible' || key === 'enabled' || key === 'checked' || key === 'bar' || key === 'scroll') n[key] = value === 'true';
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

function applyCppModel(imported) {
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

function svgValue(element, name, fallback = 0) {
    const value = Number.parseFloat(element.getAttribute(name));
    return Number.isFinite(value) ? value : fallback;
}

function svgPaint(element, name, fallback = 'transparent') {
    const direct = element.getAttribute(name);
    const style = (element.getAttribute('style') || '').match(new RegExp(`(?:^|;)\\s*${name}\\s*:\\s*([^;]+)`, 'i'));
    const value = (direct || style?.[1] || fallback).trim();
    return value === 'none' || value.startsWith('url(') ? fallback : value;
}

function svgNumbers(value) {
    return String(value || '').match(/-?\d*\.?\d+(?:e[-+]?\d+)?/gi)?.map(Number).filter(Number.isFinite) || [];
}

function svgBounds(element) {
    const type = element.localName;
    if (type === 'rect') return {x: svgValue(element, 'x'), y: svgValue(element, 'y'), width: svgValue(element, 'width'), height: svgValue(element, 'height')};
    if (type === 'circle') {
        const radius = svgValue(element, 'r');
        return {x: svgValue(element, 'cx') - radius, y: svgValue(element, 'cy') - radius, width: radius * 2, height: radius * 2};
    }
    if (type === 'ellipse') {
        const rx = svgValue(element, 'rx'), ry = svgValue(element, 'ry');
        return {x: svgValue(element, 'cx') - rx, y: svgValue(element, 'cy') - ry, width: rx * 2, height: ry * 2};
    }
    if (type === 'line') {
        const x1 = svgValue(element, 'x1'), y1 = svgValue(element, 'y1'), x2 = svgValue(element, 'x2'), y2 = svgValue(element, 'y2');
        return {x: Math.min(x1, x2), y: Math.min(y1, y2), width: Math.abs(x2 - x1), height: Math.abs(y2 - y1)};
    }
    const values = svgNumbers(element.getAttribute(type === 'path' ? 'd' : 'points'));
    if (values.length < 2) return {x: 0, y: 0, width: 320, height: 480};
    const xs = values.filter((_, index) => index % 2 === 0), ys = values.filter((_, index) => index % 2 === 1);
    return {x: Math.min(...xs), y: Math.min(...ys), width: Math.max(...xs) - Math.min(...xs), height: Math.max(...ys) - Math.min(...ys)};
}

async function cropSvgContent(content, bounds) {
    return new Promise(resolve => {
        const image = new Image();
        image.onload = () => {
            const width = Math.max(1, Math.ceil(bounds.width));
            const height = Math.max(1, Math.ceil(bounds.height));
            const canvas = document.createElement('canvas');
            canvas.width = width;
            canvas.height = height;
            const context = canvas.getContext('2d', {willReadFrequently: true});
            try {
                context.drawImage(image, 0, 0, width, height);
                const pixels = context.getImageData(0, 0, width, height).data;
                let left = width, top = height, right = -1, bottom = -1;
                for (let y = 0; y < height; y++) for (let x = 0; x < width; x++) {
                    if (pixels[(y * width + x) * 4 + 3] > 2) {
                        left = Math.min(left, x); top = Math.min(top, y); right = Math.max(right, x); bottom = Math.max(bottom, y);
                    }
                }
                if (right < 0) return resolve({content, bounds});
                const cropped = {
                    x: bounds.x + left * bounds.width / width,
                    y: bounds.y + top * bounds.height / height,
                    width: (right - left + 1) * bounds.width / width,
                    height: (bottom - top + 1) * bounds.height / height
                };
                resolve({content: content.replace(`viewBox="${bounds.x} ${bounds.y} ${bounds.width} ${bounds.height}"`, `viewBox="${cropped.x} ${cropped.y} ${cropped.width} ${cropped.height}"`).replace(`width="${bounds.width}" height="${bounds.height}"`, `width="${cropped.width}" height="${cropped.height}"`), bounds: cropped});
            } catch {
                resolve({content, bounds});
            }
        };
        image.onerror = () => resolve({content, bounds});
        image.src = `data:image/svg+xml;charset=utf-8,${encodeURIComponent(content)}`;
    });
}

async function svgImageAsset(element, bounds, index, source) {
    const asset = `assets/imported-${Date.now()}-${index}.svg`;
    const content = `<svg xmlns="http://www.w3.org/2000/svg" width="${Math.max(1, bounds.width)}" height="${Math.max(1, bounds.height)}" viewBox="${bounds.x} ${bounds.y} ${Math.max(1, bounds.width)} ${Math.max(1, bounds.height)}">${element.outerHTML}</svg>`;
    const cropped = await cropSvgContent(content, {x: bounds.x, y: bounds.y, width: Math.max(1, bounds.width), height: Math.max(1, bounds.height)});
    source.assets.push({path: asset, content: cropped.content});
    return node('Image', {id: `svg_${element.localName}_${index}`, x: cropped.bounds.x, y: cropped.bounds.y, width: Math.max(1, cropped.bounds.width), height: Math.max(1, cropped.bounds.height), src: asset, mode: 'stretch', bg_color: 'transparent'});
}

async function importSvgDesign() {
    const source = document.querySelector('#svg-input').value.trim();
    if (!source) return;
    const documentRoot = new DOMParser().parseFromString(source, 'image/svg+xml');
    if (documentRoot.querySelector('parsererror') || documentRoot.documentElement.localName !== 'svg') {
        alert('The pasted content is not valid SVG.');
        return;
    }
    const svg = documentRoot.documentElement;
    const width = Number.parseFloat(svg.getAttribute('width'));
    const height = Number.parseFloat(svg.getAttribute('height'));
    const viewBox = (svg.getAttribute('viewBox') || '').trim().split(/[ ,]+/).map(Number);
    if (width !== 320 || height !== 480 || viewBox.length !== 4 || viewBox[0] !== 0 || viewBox[1] !== 0 || viewBox[2] !== 320 || viewBox[3] !== 480) {
        alert('SVG size must be exactly 320×480 with viewBox="0 0 320 480".');
        return;
    }
    const path = `assets/imported-${Date.now()}.svg`;
    const image = {path, type: 'image/svg+xml', data: `data:image/svg+xml;charset=utf-8,${encodeURIComponent(source)}`};
    const background = node('Image', {id: 'svg_artwork', x: 0, y: 0, width: 320, height: 480, src: path, mode: 'stretch', bg_color: 'transparent'});
    const assets = {assets: []};
    const controlsFromSvg = (await Promise.all([...svg.querySelectorAll('rect,text,image,circle,ellipse,line,polygon,polyline,path')]
        .filter(element => !element.closest('defs,mask,clipPath'))
        .map((element, index) => {
            const bounds = svgBounds(element);
            const x = Math.max(0, Math.min(320 - 1, Math.round(bounds.x)));
            const y = Math.max(0, Math.min(480 - 1, Math.round(bounds.y)));
            const width = Math.max(1, Math.min(320 - x, Math.round(bounds.width)));
            const height = Math.max(1, Math.min(480 - y, Math.round(bounds.height)));
            if (element.localName === 'rect') return node('Rect', {id: `svg_rect_${index + 1}`, x, y, width, height, bg_color: svgPaint(element, 'fill'), radius: Math.round(svgValue(element, 'rx'))});
            if (element.localName === 'text') return node('Text', {id: `svg_text_${index + 1}`, x, y, width, height: Math.max(height, 16), content: element.textContent.trim(), color: svgPaint(element, 'fill', svgPaint(element, 'stroke', '#000000')), font_size: Math.max(1, Math.round(svgValue(element, 'font-size', 16))), text_align: 'left'});
            if (element.localName === 'circle' || element.localName === 'ellipse') return node('Rect', {id: `svg_${element.localName}_${index + 1}`, x, y, width, height, bg_color: svgPaint(element, 'fill'), radius: Math.round(Math.min(width, height) / 2)});
            if (element.localName === 'line') return node('Rect', {id: `svg_line_${index + 1}`, x, y, width, height: Math.max(1, height, Math.round(svgValue(element, 'stroke-width', 1))), bg_color: svgPaint(element, 'stroke', svgPaint(element, 'fill'))});
            return svgImageAsset(element, {x, y, width, height}, index + 1, assets);
        }))).flat();
    model.window = {title: 'Imported SVG', class_name: 'MainWindow', width: '320', height: '480', bg_color: 'transparent', topbar: false};
    model.nodes = [background, ...controlsFromSvg];
    model.images = [...(model.images || []).filter(item => item.path !== path && !assets.assets.some(asset => asset.path === item.path)), image, ...assets.assets.map(asset => ({path: asset.path, type: 'image/svg+xml', data: `data:image/svg+xml;charset=utf-8,${encodeURIComponent(asset.content)}`}))];
    selectedId = 'window';
    document.querySelector('#svg-modal').classList.remove('open');
    render();
    if (currentProjectId) {
        currentAppFS[path] = source;
        const writes = [{path, content: source}, ...assets.assets];
        await Promise.all(writes.map(file => fetch(`/api/applications/${apiProjectPath(currentProjectId)}/files`, {
            method: 'POST', headers: {'Content-Type': 'application/json'},
            body: JSON.stringify(file)
        })));
        assets.assets.forEach(asset => currentAppFS[asset.path] = asset.content);
    }
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

function cppName(value, fallback) {
    const parts = String(value || fallback).replace(/[^A-Za-z0-9]+/g, ' ').trim().split(/\s+/).filter(Boolean);
    let name = parts.map(part => part[0].toUpperCase() + part.slice(1)).join('') || fallback;
    if (/^\d/.test(name)) name = `Window${name}`;
    return name;
}

function cppQuote(value) {
    return String(value ?? '').replaceAll('\\', '\\\\').replaceAll('"', '\\"').replaceAll('\n', '\\n');
}

function cppColor(value) {
    if (value === 'transparent') return 'Color(0x00000000)';
    const hex = pickerColor(value).replace('#', '');
    return `Color(0xFF${hex.toUpperCase()})`;
}

function cppSize(value, unit, fallback) {
    const number = value === null || value === undefined || value === '' ? fallback : value;
    const relative = unit === '%' || String(number).trim().endsWith('%');
    return relative ? `Size(${String(number).replace('%', '')}, true)` : `Size(${number})`;
}

function cppWindowSize(value, fallback) {
    const text = String(value ?? '').trim();
    if (text.endsWith('%')) return `Size(${Math.round(Number(text.slice(0, -1)) * fallback / 100)})`;
    return `Size(${Number(text) || fallback})`;
}

function cppAlignment(value) {
    if (value === 'center') return '0';
    const flags = {left: 'ANCHOR_LEFT', right: 'ANCHOR_RIGHT', top: 'ANCHOR_TOP', bottom: 'ANCHOR_BOTTOM'};
    return String(value || 'left').split('_').map(part => flags[part]).filter(Boolean).join(' | ') || 'ANCHOR_LEFT';
}

function cppValue(n, key, fallback = 0) {
    const value = n[key] === null || n[key] === undefined || n[key] === '' ? fallback : n[key];
    return n.units[key] === '%' ? `static_cast<int>(${value} * 1)` : String(value);
}

function cppControlAssignments(n, variable) {
    const lines = [
        `        ${variable}.name = string("${cppQuote(n.id)}");`,
        `        ${variable}.visible = ${Boolean(n.visible) ? 'true' : 'false'};`,
        `        ${variable}.enabled = ${Boolean(n.enabled) ? 'true' : 'false'};`,
        `        ${variable}.location = Point(${cppValue(n, 'x')}, ${cppValue(n, 'y')});`,
        `        ${variable}.width = ${cppSize(n.width, n.units.width, controls[n.type].w)};`,
        `        ${variable}.height = ${cppSize(n.height, n.units.height, controls[n.type].h)};`,
        `        ${variable}.color = ${cppColor(n.color)};`,
        `        ${variable}.bg_color = ${cppColor(n.bg_color)};`
    ];
    const add = (property, expression) => lines.push(`        ${variable}.${property} = ${expression};`);
    if (['Text', 'Button', 'RoundButton', 'CheckBox', 'RadioButton', 'Switch', 'TextBox'].includes(n.type)) add('content', `string("${cppQuote(n.content)}")`);
    if (['Text', 'Button', 'RoundButton', 'CheckBox', 'RadioButton', 'Switch', 'TextBox'].includes(n.type)) {
        add('font_size', n.font_size || 0);
        add('text_align', cppAlignment(n.text_align));
    }
    if (['CheckBox', 'RadioButton', 'Switch'].includes(n.type)) add('checked', Boolean(n.checked) ? 'true' : 'false');
    if (['Button', 'RoundButton'].includes(n.type)) add('image', `string("${cppQuote(n.image)}")`);
    if (n.type === 'RoundButton') add('radius', n.radius || 0);
    if (n.type === 'Rect') add('radius', n.radius || 0);
    if (n.type === 'Image') {
        add('source', `string("${cppQuote(n.src)}")`);
        add('mode', {stretch: 0, zoom: 1, auto_size: 2, center: 3}[n.mode] ?? 0);
    }
    if (n.type === 'TextBox') add('type', `string("${cppQuote(n.input_type)}")`);
    if (n.type === 'ProgressBar') {
        add('min', n.min || 0);
        add('value', n.value || 0);
        add('max', n.max || 100);
    }
    if (['Vscroll', 'Hscroll'].includes(n.type)) add('bar', Boolean(n.bar) ? 'true' : 'false');
    if (n.type === 'WebView') add('url', `string("${cppQuote(n.url)}")`);
    return lines;
}

function generateCpp() {
    const className = cppName(model.window.class_name || model.window.title, 'MainWindow');
    const fileGuard = `${className.toUpperCase()}_HPP`;
    const variables = new Map(model.nodes.map((n, i) => [n.id, `${cppName(n.id, n.type)}${i}`]));
    const roots = model.nodes.filter(n => !n.parent || !byId(n.parent));
    const ordered = [];
    const visit = n => { ordered.push(n); kids(n.id).forEach(visit); };
    roots.forEach(visit);
    const lines = [`#ifndef ${fileGuard}`, `#define ${fileGuard}`, '', '#include "app.hpp"', '', `class ${className} : public Window`, '{', 'private:'];
    ordered.forEach(n => lines.push(`    ${n.type} ${variables.get(n.id)};`));
    lines.push('public:', `    ${className}(void) : Window("${cppQuote(model.window.title || 'My App')}", ${cppWindowSize(model.window.width, 320)}, ${cppWindowSize(model.window.height, 480)})`, '    {');
    ordered.forEach(n => cppControlAssignments(n, variables.get(n.id)).forEach(line => lines.push(line)));
    ordered.forEach(n => {
        const parent = n.parent && variables.get(n.parent);
        lines.push(`        ${parent ? `${parent}.add_control` : 'this->add_control'}(&${variables.get(n.id)});`);
    });
    lines.push('    }', '};', '', '#endif', '');
    return lines.join('\n');
}

function downloadCpp() {
    const className = cppName(model.window.class_name || model.window.title, 'MainWindow');
    const blob = new Blob([generateCpp()], {type: 'text/x-c++hdr;charset=utf-8'});
    const url = URL.createObjectURL(blob);
    const link = document.createElement('a');
    link.href = url;
    link.download = `${className}.hpp`;
    link.click();
    URL.revokeObjectURL(url);
}

function syncCppFromVisual() {
    if (!currentProjectId || !currentSrcPath || !/\.hpp$/i.test(currentSrcPath)) return;
    const content = generateCpp();
    currentAppFS[currentSrcPath] = content;
    fetch(`/api/applications/${apiProjectPath(currentProjectId)}/files`, {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({path: currentSrcPath, content})
    }).catch(error => console.error(error));
}

function scheduleCppSync() {
    if (cppSyncTimer) clearTimeout(cppSyncTimer);
    cppSyncTimer = setTimeout(() => {
        cppSyncTimer = null;
        syncCppFromVisual();
    }, 350);
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
    if (!selectedIds.has(selectedId)) selectedIds = new Set([selectedId]);
    model.nodes.forEach(normalizeNodeNumbers);
    synchronizeTemplates();
    drawCanvas();
    drawTree();
    drawTemplates();
    drawInspector();
    drawAppFilesystem();
    if (!suppressDirty && activeFile && visualEditor.classList.contains('active')) markDirty(activeFile);
    document.querySelector('#app-label').textContent = model.window.title || model.window.name || 'Untitled app';
    document.querySelector('#selection-label').textContent = selectedId === 'window' ? 'Window selected' : selectedId ? `${byId(selectedId).type} selected` : 'No selection'
    scheduleCppSync();
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
document.querySelector('#build-cpp').onclick = downloadCpp;
document.querySelector('#view-code').onclick = () => {
    document.querySelector('#code-title').textContent = 'Generated layout';
    document.querySelector('#code-output').value = generate();
    document.querySelector('#code-modal').classList.add('open')
};
document.querySelector('#view-cpp').onclick = () => {
    document.querySelector('#code-title').textContent = 'Generated C++';
    document.querySelector('#code-output').value = generateCpp();
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
                class_name: 'MainWindow',
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

async function saveCurrentFile() {
    if (!currentProjectId) return;
    if (!activeFile) activeFile = currentLayoutPath || currentSrcPath;
    if (!activeFile) return;
    let content;
    if (codeEditor.classList.contains('active')) {
        content = document.querySelector('#src-editor').value;
        editorDrafts.set(`${currentProjectId}:${activeFile}`, content);
    } else if (isWindowFile(activeFile)) {
        content = generateCpp();
    } else if (currentLayoutPath === activeFile) {
        content = generate();
    } else return;
    const status = document.querySelector('#save-status');
    if (status) status.textContent = 'Saving…';
    const response = await fetch(`/api/applications/${apiProjectPath(currentProjectId)}/files`, {
        method: 'POST', headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({path: activeFile, content})
    });
    const result = await response.json();
    if (!response.ok) {
        if (status) status.textContent = 'Save failed';
        alert(result.error || 'Unable to save file.');
        return;
    }
    currentAppFS[activeFile] = content;
    editorDrafts.delete(`${currentProjectId}:${activeFile}`);
    if (result.model && codeEditor.classList.contains('active')) applyCppModel(result.model);
    dirtyFiles.delete(fileKey(currentProjectId, activeFile));
    document.querySelector('#app-label').textContent = codeEditor.classList.contains('active') ? activeFile : (model.window.title || activeFile);
    if (status) status.textContent = 'Saved';
    drawFileTabs();
    drawAppFilesystem();
}

function duplicateSelection() {
    const originals = model.nodes.filter(n => selectedIds.has(n.id));
    if (!originals.length) return;
    const ids = new Map(originals.map(n => [n.id, uid()]));
    const copies = originals.map(original => {
        const copy = JSON.parse(JSON.stringify(original));
        copy.id = ids.get(original.id);
        copy.parent = ids.get(original.parent) || original.parent;
        copy.x = px(original.x, original.units.x, parentSize(original).w) + 12;
        copy.y = px(original.y, original.units.y, parentSize(original).h) + 12;
        copy.units.x = 'px';
        copy.units.y = 'px';
        return copy;
    });
    model.nodes.push(...copies);
    selectedIds = new Set(copies.map(n => n.id));
    selectedId = copies.at(-1).id;
    render();
}

function nudgeSelection(dx, dy) {
    selectedIds.forEach(id => {
        const n = byId(id);
        if (!n) return;
        const parent = parentSize(n);
        n.x = val(px(n.x, n.units.x, parent.w) + dx, n.units.x, parent.w);
        n.y = val(px(n.y, n.units.y, parent.h) + dy, n.units.y, parent.h);
    });
    render();
}

window.addEventListener('keydown', event => {
    if (event.code === 'Space' && !event.repeat && !(event.target instanceof HTMLInputElement) && !(event.target instanceof HTMLTextAreaElement)) {
        spacePressed = true;
        canvasArea.style.cursor = 'grab';
        event.preventDefault();
        return;
    }
    if ((event.ctrlKey || event.metaKey) && (event.key.toLowerCase() === 's' || event.code === 'KeyS')) {
        event.preventDefault();
        event.stopPropagation();
        saveCurrentFile().catch(error => alert(`Unable to save file: ${error.message}`));
        return;
    }
    const target = event.target;
    const editingText = target instanceof HTMLInputElement || target instanceof HTMLTextAreaElement || target instanceof HTMLSelectElement;
    if (event.key === 'Delete' && visualEditor.classList.contains('active') && !editingText && selectedId !== 'window') {
        event.preventDefault();
        remove();
        return;
    }
    if (visualEditor.classList.contains('active') && !editingText && event.ctrlKey && event.key.toLowerCase() === 'd') {
        event.preventDefault();
        duplicateSelection();
        return;
    }
    if (visualEditor.classList.contains('active') && !editingText && ['ArrowLeft', 'ArrowRight', 'ArrowUp', 'ArrowDown'].includes(event.key)) {
        event.preventDefault();
        const step = event.shiftKey ? 10 : 1;
        nudgeSelection(event.key === 'ArrowLeft' ? -step : event.key === 'ArrowRight' ? step : 0, event.key === 'ArrowUp' ? -step : event.key === 'ArrowDown' ? step : 0);
    }
}, true);

window.addEventListener('keyup', event => {
    if (event.code === 'Space') {
        spacePressed = false;
        canvasArea.style.cursor = '';
    }
});

document.querySelector('#src-editor').addEventListener('input', () => {
    if (activeFile && codeEditor.classList.contains('active')) {
        editorDrafts.set(fileKey(currentProjectId, activeFile), document.querySelector('#src-editor').value);
        markDirty(activeFile);
    }
});

document.querySelector('#src-editor').addEventListener('keydown', event => {
    if ((event.ctrlKey || event.metaKey) && event.key.toLowerCase() === 's') {
        event.preventDefault();
        event.stopPropagation();
        saveCurrentFile().catch(error => alert(`Unable to save file: ${error.message}`));
    }
});

window.addEventListener('beforeunload', event => {
    if (dirtyFiles.size) {
        event.preventDefault();
        event.returnValue = '';
    }
});

document.querySelector('#new-app').onclick = createApplication;
document.querySelector('#startup-new-app').onclick = createApplication;
document.querySelector('#import-svg').onclick = () => document.querySelector('#svg-modal').classList.add('open');
document.querySelector('#close-svg').onclick = () => document.querySelector('#svg-modal').classList.remove('open');
document.querySelector('#import-svg-confirm').onclick = importSvgDesign;
document.querySelector('#compile').onclick = async() => {
    if (!currentProjectId) {
        alert('Open an application first.');
        return;
    }
    const response = await fetch(`/api/applications/${apiProjectPath(currentProjectId)}/compile`, {method: 'POST'});
    const result = await response.json();
    document.querySelector('#code-title').textContent = result.ok ? 'Compilation succeeded' : 'Compilation failed';
    document.querySelector('#code-output').value = result.output || result.error || '';
    document.querySelector('#code-modal').classList.add('open');
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
document.querySelector('.canvas-area')?.addEventListener('wheel', event => {
    if (!event.ctrlKey) return;
    event.preventDefault();
    const zoom = document.querySelector('#zoom');
    const next = Math.max(0.5, Math.min(4, Number(zoom.value) - event.deltaY * 0.002));
    zoom.value = next.toFixed(2);
    document.querySelector('#canvas-shell').style.transform = `scale(${next})`;
}, {passive: false});
canvas.onclick = () => select('window');
render();
